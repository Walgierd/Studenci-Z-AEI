
#include "Trade.h"

void TradeUI::startTrade(sf::Font& font, std::vector<Player>& players, int currentPlayer) {
    exchangeMode = true;
    exchangeTargetPlayer = -1;
    exchangeGive.clear();
    exchangeGet.clear();
    exchangeButtons.clear();
    exchangePlayerButtons.clear();
    exchangeAcceptButton.reset();

    float y = 200.f;
    for (size_t i = 0; i < players.size(); ++i) {
        if ((int)i == currentPlayer) continue;
        std::string label = "Gracz " + std::to_string(players[i].getId() + 1);
        exchangePlayerButtons.push_back(std::make_unique<SimpleButton>(font, label, sf::Vector2f(300, y), [this, &font, &players, currentPlayer, i]() mutable {
            exchangeTargetPlayer = (int)i;
            exchangeButtons.clear();
            float by = 300.f;
            for (ResourceType t : {ResourceType::Kawa, ResourceType::Piwo, ResourceType::Notatki, ResourceType::Pizza, ResourceType::Kabel}) {
                exchangeGive[t] = 0;
                exchangeGet[t] = 0;

                auto giveBtn = std::make_unique<SimpleButton>(
                    font,
                    std::string("Daj 0 ") + resourceName(t),
                    sf::Vector2f(600, by),
                    [this, t, btnPtr = (SimpleButton*)nullptr]() mutable {
                        exchangeGive[t] = (exchangeGive[t] + 1) % 6;
                        btnPtr->setLabelText("Daj " + std::to_string(exchangeGive[t]) + " " + resourceName(t));
                    }
                );
                auto* giveBtnRaw = giveBtn.get();
                giveBtn->setCallback([this, t, giveBtnRaw]() mutable {
                    exchangeGive[t] = (exchangeGive[t] + 1) % 6;
                    giveBtnRaw->setLabelText("Daj " + std::to_string(exchangeGive[t]) + " " + resourceName(t));
                });
                exchangeButtons.push_back(std::move(giveBtn));

                auto wezBtn = std::make_unique<SimpleButton>(
                    font,
                    std::string("Wez 0 ") + resourceName(t),
                    sf::Vector2f(900, by),
                    [this, t, by, &font]() {}
                );
                auto* wezBtnRaw = wezBtn.get();
                wezBtn->setCallback([this, t, by, wezBtnRaw]() mutable {
                    exchangeGet[t] = (exchangeGet[t] + 1) % 6;
                    wezBtnRaw->setLabelText("Wez " + std::to_string(exchangeGet[t]) + " " + resourceName(t));
                });
                exchangeButtons.push_back(std::move(wezBtn));

                by += 60.f;
            }
            exchangeAcceptButton = std::make_unique<SimpleButton>(font, "Akceptuj", sf::Vector2f(800, by + 20.f), [this, &players, currentPlayer]() {
                bool canGive = true, canGet = true;
                for (auto& [t, v] : exchangeGive)
                    if (players[currentPlayer].getResourceCount(t) < v) canGive = false;
                for (auto& [t, v] : exchangeGet)
                    if (players[exchangeTargetPlayer].getResourceCount(t) < v) canGet = false;
                if (canGive && canGet) {
                    for (auto& [t, v] : exchangeGive) {
                        players[currentPlayer].removeResource(t, v);
                        players[exchangeTargetPlayer].addResource(t, v);
                    }
                    for (auto& [t, v] : exchangeGet) {
                        players[exchangeTargetPlayer].removeResource(t, v);
                        players[currentPlayer].addResource(t, v);
                    }
                }
                reset();
            });
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
    exchangeMode = false;
    exchangeTargetPlayer = -1;
    exchangeGive.clear();
    exchangeGet.clear();
    exchangeButtons.clear();
    exchangePlayerButtons.clear();
    exchangeAcceptButton.reset();
}

