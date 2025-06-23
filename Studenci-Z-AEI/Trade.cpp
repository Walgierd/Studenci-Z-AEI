#include "Trade.h"
#include "HexTile.h"
#include <thread>
#include <mutex>
#include"Game.h"



std::mutex tradeLogsMutex;

void TradeUI::startTrade(sf::Font& font, std::vector<Player>& players, int currentPlayer, Logs* logs) {
    playersPtr = &players;
    exchangeMode = true;
    exchangeTargetPlayer = -1; 
    exchangeGive.clear();
    exchangeGet.clear();
    exchangeButtons.clear();
    exchangePlayerButtons.clear();
    exchangeAcceptButton.reset();

    float y = 200.f;
    for (size_t i = 0; i < players.size(); ++i) {
        if (static_cast<int>(i) == currentPlayer) {
            continue;
        }

        std::string label = (players[i].getId() == -1) ? "Bank" : players[i].getNickname();
        exchangePlayerButtons.push_back(std::make_unique<SimpleButton>(font, label, sf::Vector2f(300, y), [this, &font, &players, currentPlayer, i, logs]() mutable {
            exchangeTargetPlayer = static_cast<int>(i);
            exchangeButtons.clear();
            float by = 300.f;
            for (ResourceType t : {ResourceType::Kawa, ResourceType::Piwo, ResourceType::Notatki, ResourceType::Pizza, ResourceType::Kabel}) {
                exchangeGive[t] = 0;
                exchangeGet[t] = 0;

                // Daj
                auto giveBtn = std::make_unique<SimpleButton>(
                    font,
                    std::string("Daj 0 ") + resourceName(t),
                    sf::Vector2f(600, by),
                    [](){} 
                );
                auto* giveBtnRaw = giveBtn.get();
                giveBtn->setCallback([this, t, giveBtnRaw]() mutable {
                    exchangeGive[t] = (exchangeGive[t] + 1) % 6;
                    giveBtnRaw->setLabelText("Daj " + std::to_string(exchangeGive[t]) + " " + resourceName(t));
                });
                exchangeButtons.push_back(std::move(giveBtn));

                // Weź
                auto wezBtn = std::make_unique<SimpleButton>(
                    font,
                    std::string("Wez 0 ") + resourceName(t),
                    sf::Vector2f(900, by),
                    [](){} 
                );
                auto* wezBtnRaw = wezBtn.get();
                wezBtn->setCallback([this, t, wezBtnRaw]() mutable {
                    exchangeGet[t] = (exchangeGet[t] + 1) % 6;
                    wezBtnRaw->setLabelText("Wez " + std::to_string(exchangeGet[t]) + " " + resourceName(t));
                });
                exchangeButtons.push_back(std::move(wezBtn));

                by += 60.f;
            }
            exchangeAcceptButton = std::make_unique<SimpleButton>(
                font, 
                "Akceptuj", 
                sf::Vector2f(800, by + 20.f), 
                [this, &players, currentPlayer, logs]() {
                    bool canGive = true, canGet = true;
                    std::string errorMsg;

                    // do banku
                    if (players[exchangeTargetPlayer].getId() == -1) {
                        int giveCount = 0, getCount = 0;
                        ResourceType giveType = ResourceType::Kawa, getType = ResourceType::Kawa;
                        for (auto& [t, v] : exchangeGive) {
                            if (v > 0) { giveCount++; giveType = t; }
                        }
                        for (auto& [t, v] : exchangeGet) {
                            if (v > 0) { getCount++; getType = t; }
                        }

                        int requiredGive = 4; // domyślnie 4:1
                        const Player& player = players[currentPlayer];
                        if (player.hasPort(PortType::Generic)) {
                            requiredGive = 3;
                        }
                        if (player.hasPort(portTypeFromResource(giveType))) {
                            requiredGive = 2;
                        }
                        if (giveCount != 1 || getCount != 1) {
                            canGet = false;
                            if (logs) errorMsg += "Wymiana z bankiem: musisz dać dokładnie " + std::to_string(requiredGive) + " jednego typu i wziąć 1 innego typu. ";
                        }
                        if (exchangeGive[giveType] != requiredGive || exchangeGet[getType] != 1) {
                            canGet = false;
                            if (logs) errorMsg += "Wymiana z bankiem: musisz dać " + std::to_string(requiredGive) + " za 1. ";
                        }
                        if (players[currentPlayer].getResourceCount(giveType) < requiredGive) {
                            canGive = false;
                            if (logs) errorMsg += "Za mało surowca do wymiany z bankiem: " + resourceName(giveType) + ". ";
                        }
                    } else {
                        for (auto& [t, v] : exchangeGive) {
                            if (players[currentPlayer].getResourceCount(t) < v) {
                                canGive = false;
                                if (logs && v > 0)
                                    errorMsg += "Za mało surowca: " + resourceName(t) + " (masz " +
                                        std::to_string(players[currentPlayer].getResourceCount(t)) +
                                        ", próbujesz dać " + std::to_string(v) + "). ";
                            }
                        }
                        for (auto& [t, v] : exchangeGet) {
                            if (players[exchangeTargetPlayer].getResourceCount(t) < v) {
                                canGet = false;
                                if (logs && v > 0)
                                    errorMsg += players[exchangeTargetPlayer].getNickname() + " ma za mało: " + resourceName(t) + ". ";
                            }
                        }
                    }

                    if (canGive && canGet) {
                        if (players[exchangeTargetPlayer].getId() == -1) {
                            for (auto& [t, v] : exchangeGive) {
                                players[currentPlayer].removeResource(t, v);
                            }
                            for (auto& [t, v] : exchangeGet) {
                                players[currentPlayer].addResource(t, v);
                            }
                            if (logs) {
                                std::string logMsg = PLAYER_NICK(players, currentPlayer) + " wymienia z Bankiem";
                                std::thread([logs, logMsg]() {
                                    std::lock_guard<std::mutex> lock(tradeLogsMutex);
                                    logs->add(logMsg);
                                }).detach();
                            }
                        } else {
                            for (auto& [t, v] : exchangeGive) {
                                players[currentPlayer].removeResource(t, v);
                                players[exchangeTargetPlayer].addResource(t, v);
                            }
                            for (auto& [t, v] : exchangeGet) {
                                players[exchangeTargetPlayer].removeResource(t, v);
                                players[currentPlayer].addResource(t, v);
                            }
                            if (logs) {
                                std::string logMsg = PLAYER_NICK(players, currentPlayer) + " wymienia z " + PLAYER_NICK(players, exchangeTargetPlayer);
                                std::thread([logs, logMsg]() {
                                    std::lock_guard<std::mutex> lock(tradeLogsMutex);
                                    logs->add(logMsg);
                                }).detach();
                            }
                        }
                    } else {
                        if (logs && !errorMsg.empty()) {
                            std::string logMsg = errorMsg;
                            std::thread([logs, logMsg]() {
                                std::lock_guard<std::mutex> lock(tradeLogsMutex);
                                logs->add(logMsg);
                            }).detach();
                        }
                    }
                    reset();
                }
            );
        }));
        y += 60.f;
    }
}

void TradeUI::handleClick(const sf::Vector2f& mousePos, std::vector<Player>& players, int currentPlayer) {
    if (!exchangeMode) return;
    if (exchangeTargetPlayer == -1) {
        for (auto& btn : exchangePlayerButtons) {
            if (btn && btn->isClicked(mousePos)) btn->onClick();
        }
    } else {
        for (auto& btn : exchangeButtons) {
            if (btn && btn->isClicked(mousePos)) btn->onClick();
        }
        if (exchangeAcceptButton && exchangeAcceptButton->isClicked(mousePos)) {
            exchangeAcceptButton->onClick();
        }
    }
}

void TradeUI::draw(sf::RenderWindow& window) {
    if (!exchangeMode) return;
    if (exchangeTargetPlayer == -1) {
        for (auto& btn : exchangePlayerButtons) if (btn) btn->draw(window);
    } else {
        for (auto& btn : exchangeButtons) if (btn) btn->draw(window);
        if (exchangeAcceptButton) exchangeAcceptButton->draw(window);
    }
}

void TradeUI::reset() {
    if (playersPtr && !playersPtr->empty() && playersPtr->back().getId() == -1) {
        playersPtr->pop_back(); 
    }
    playersPtr = nullptr;
    exchangeMode = false;
    exchangeTargetPlayer = -1;
    exchangeGive.clear();
    exchangeGet.clear();
    exchangeButtons.clear();
    exchangePlayerButtons.clear();
    exchangeAcceptButton.reset();
}

PortType portTypeFromResource(ResourceType res) {
    switch (res) {
        case ResourceType::Kawa:    return PortType::Kawa;
        case ResourceType::Piwo:    return PortType::Piwo;
        case ResourceType::Notatki: return PortType::Notatki;
        case ResourceType::Pizza:   return PortType::Pizza;
        case ResourceType::Kabel:   return PortType::Kabel;
        default:                    return PortType::Generic;
    }
}



