#include "Game.h"
#include "Menago.h"
#include <filesystem>
#include <cmath>

Game::Game()
    : window(sf::VideoMode(1920, 1080), "Studenci z AEI", sf::Style::Default),
      currentStyle(sf::Style::Default),
      menu(1920, 1080),
      playerUI(font), 
      inMenu(true),
      hexSize(80.f),
      board(hexSize, sf::Vector2f(1920.f / 2.f, 1080.f / 2.f)),
      buildMode(BuildMode::None),
      knight(),
      knightMoveMode(false),
      freeBuildRoad(false),
      freeBuildSettlement(false),
      setupPhase(true),
      setupTurn(0),
      setupStep(0),
      setupPlayerIndex(0),
      logs(font)
{
    if (!std::filesystem::exists("Fonts/arial.ttf") || !font.loadFromFile("Fonts/arial.ttf")) {
        throw std::runtime_error("Brak czcionki Fonts/arial.ttf");
    }
    knight.setPosition(static_cast<int>(board.getTiles().size() / 2));
    setupPlayerButtons();
    lastSettlementPos.resize(4, sf::Vector2f(-1000, -1000)); 
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
        if (!turnManager.getPlayers().empty() && buildMode == BuildMode::None) {
            static int lastPlayer = -1;
            if (turnManager.getCurrentPlayerIndex() != lastPlayer) {
                turnManager.getCurrentPlayer().resetDice();
                lastPlayer = turnManager.getCurrentPlayerIndex();
            }
        }
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {       
        if (event.type == sf::Event::Closed)
            window.close();

        if (inMenu)
            handleMenuEvents(event);
        else
            handleGameEvents(event);
    }
}

void Game::handleMenuEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
        if (menu.isStartClicked(mousePos)) {
            int playerCount = menu.getSelectedPlayerCount();
            turnManager.initialize(playerCount);
            inMenu = false;
        }
        if (menu.isFullscreenClicked(mousePos)) {
            menu.setFullscreenToggleRequested(true);
        }
    }
}

void Game::handleGameEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
    if (setupPhase) {

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

            auto& players = turnManager.getPlayers();
            int currentPlayer = setupPlayerIndex;

            if (setupStep == 0) { 
                std::vector<sf::Vector2f> hexCenters;
                for (const auto& tile : board.getTiles())
                    hexCenters.push_back(tile.getPosition());
                auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);
                for (const auto& pos : settlementSpots) {
                    if (std::hypot(mousePos.x - pos.x, mousePos.y - pos.y) < 15.f) {
                        bool freeBuildSettlementTemp = true; 
                        if (tryBuildSettlement(buildables, players, currentPlayer, pos, hexSize * std::sqrt(3.f) - 5, freeBuildSettlementTemp, true, &logs)) {
                            lastSettlementPos[currentPlayer] = pos;
                            setupStep = 1; 
                        }
                        break;
                    }
                }
            } else if (setupStep == 1) {
                std::vector<sf::Vector2f> hexCenters;
                for (const auto& tile : board.getTiles())
                    hexCenters.push_back(tile.getPosition());
                auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);
                for (const auto& edge : roadSpots) {
                    sf::Vector2f mid = (edge.first + edge.second) / 2.f;
                    if ((std::hypot(lastSettlementPos[currentPlayer].x - edge.first.x, lastSettlementPos[currentPlayer].y - edge.first.y) < 1.f ||
                         std::hypot(lastSettlementPos[currentPlayer].x - edge.second.x, lastSettlementPos[currentPlayer].y - edge.second.y) < 1.f) &&
                        std::hypot(mousePos.x - mid.x, mousePos.y - mid.y) < 15.f) {
                        bool freeBuildRoadTemp = true; 
                        if (tryBuildRoad(buildables, players, currentPlayer, edge.first, edge.second, freeBuildRoadTemp, true, lastSettlementPos[currentPlayer])) {
                            logs.add("Gracz " + std::to_string(players[currentPlayer].getId() + 1) + " buduje korytarz");
                            setupPlayerIndex++;
                            if (setupPlayerIndex >= static_cast<int>(players.size())) {
                                setupPlayerIndex = 0;
                                setupTurn++;
                                if (setupTurn >= 2) {
                                    setupPhase = false; 
                                }
                            }
                            setupStep = 0; 
                        }
                        break;
                    }
                }
            }
        }
        return; 
    }


        auto& players = turnManager.getPlayers();
        int currentPlayer = turnManager.getCurrentPlayerIndex();

        if (trade.exchangeMode) {
            trade.handleClick(mousePos, players, currentPlayer);
            return;
        }
        if (knightMoveMode) {
            knight.handleMoveClick(mousePos, knightMoveButtons, knightMoveMode);
            return;
        }
        for (auto& btn : playerButtons) {
            if (btn->isClicked(mousePos)) btn->onClick();
        }
        if (cardManager.buyCardButton->isClicked(mousePos)) cardManager.buyCardButton->onClick();
        if (cardManager.showCardsButton->isClicked(mousePos)) cardManager.showCardsButton->onClick();

        sf::RectangleShape diceButton(sf::Vector2f(120, 50));
        diceButton.setPosition(static_cast<float>(window.getSize().x) - 180.f, 300.f);
        if (diceButton.getGlobalBounds().contains(mousePos)) {
            if (handleDiceRoll(players, currentPlayer, board, buildables, knight, hexSize)) {
                int d1 = players[currentPlayer].getDice1();
                int d2 = players[currentPlayer].getDice2();
                logs.add("Gracz " + std::to_string(players[currentPlayer].getId() + 1) + " rzucił kostką, wypadło " + std::to_string(d1 + d2));
                if (players[currentPlayer].getDice1() + players[currentPlayer].getDice2() == 12) {
                    knightMoveMode = true;
                    knightMoveButtons.clear();
                    const auto& tiles = board.getTiles();
                    for (size_t i = 0; i < tiles.size(); ++i) {
                        if (static_cast<int>(i) == knight.tileIndex) continue;
                        knightMoveButtons.push_back(std::make_unique<KnightMoveButton>(
                            tiles[i].getPosition(), static_cast<int>(i),
                            [&](int idx) {
                                knight.setPosition(idx);
                                knightMoveMode = false;
                                knightMoveButtons.clear();
                            }
                        ));
                    }
                }
            }
        }

        if (buildMode == BuildMode::Settlement) {
            if (!setupPhase && !players[currentPlayer].hasRolled()) {
                return;
            }
            std::vector<sf::Vector2f> hexCenters;
            for (const auto& tile : board.getTiles())
                hexCenters.push_back(tile.getPosition());
            auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);
            for (const auto& pos : settlementSpots) {
                if (std::hypot(mousePos.x - pos.x, mousePos.y - pos.y) < 15.f) {
                    if (tryBuildSettlement(buildables, players, currentPlayer, pos, hexSize * std::sqrt(3.f) - 5, freeBuildSettlement, setupPhase, &logs)) {
                        logs.add("Gracz " + std::to_string(players[currentPlayer].getId() + 1) + " buduje akademik");
                        buildMode = BuildMode::None;
                    }
                    break;
                }
            }
        } else if (buildMode == BuildMode::Road) {
            if (!setupPhase && !players[currentPlayer].hasRolled()) {
                return;
            }
            std::vector<sf::Vector2f> hexCenters;
            for (const auto& tile : board.getTiles())
                hexCenters.push_back(tile.getPosition());
            auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);
            for (const auto& edge : roadSpots) {
                sf::Vector2f mid = (edge.first + edge.second) / 2.f;
                if (std::hypot(mousePos.x - mid.x, mousePos.y - mid.y) < 15.f) {
                    if (tryBuildRoad(buildables, players, currentPlayer, edge.first, edge.second, freeBuildRoad, setupPhase, lastSettlementPos[currentPlayer])) {
                        logs.add("Gracz " + std::to_string(players[currentPlayer].getId() + 1) + " buduje korytarz");
                        buildMode = BuildMode::None;
                    }
                    break;
                }
            }
        } else if (buildMode == BuildMode::City) {
            if (!setupPhase && !players[currentPlayer].hasRolled()) {
                return;
            }
            if (tryBuildCity(buildables, players, currentPlayer, mousePos)) {
                logs.add("Gracz " + std::to_string(players[currentPlayer].getId() + 1) + " buduje kampus");
                buildMode = BuildMode::None;
            }
        }
    }
}

void Game::update() {
    if (menu.isFullscreenToggleRequested()) {
        menu.handleFullscreenToggle(window, currentStyle);
        menu.resetFullscreenToggleRequest();
    }
}

void Game::render() {
    window.clear();
    if (inMenu) {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
        menu.update(mousePos);
        menu.draw(window);
    } else {
        board.draw(window);

        {
            std::lock_guard<std::mutex> lock(buildMutex);
            for (const auto& b : buildables)
                b->draw(window);
        }


        float logsPanelWidth = 500.f;
        float logsPanelX = 30.f;
        float logsPanelY = 30.f;
        logs.draw(window, logsPanelX, logsPanelY, logsPanelWidth);

        playerUI.drawAll(turnManager.getPlayers(), window);

        for (auto& btn : playerButtons) btn->draw(window);

        if (buildMode == BuildMode::Road || buildMode == BuildMode::Settlement) {
            for (const auto& btn : buildButtons)
                btn->draw(window);
        }

        sf::RectangleShape dice1(sf::Vector2f(60, 60));
        dice1.setPosition(static_cast<float>(window.getSize().x) - 180.f, 200.f);
        dice1.setFillColor(sf::Color(220, 220, 220));
        sf::RectangleShape dice2(sf::Vector2f(60, 60));
        dice2.setPosition(static_cast<float>(window.getSize().x) - 110.f, 200.f);
        dice2.setFillColor(sf::Color(220, 220, 220));
        window.draw(dice1);
        window.draw(dice2);

        sf::Text dice1Text;
        dice1Text.setFont(font);
        dice1Text.setCharacterSize(36);
        dice1Text.setFillColor(sf::Color::Black);
        dice1Text.setString(turnManager.getPlayers().empty() ? "" : std::to_string(turnManager.getCurrentPlayer().getDice1()));
        dice1Text.setPosition(dice1.getPosition().x + 18.f, dice1.getPosition().y + 10.f);
        window.draw(dice1Text);

        sf::Text dice2Text;
        dice2Text.setFont(font);
        dice2Text.setCharacterSize(36);
        dice2Text.setFillColor(sf::Color::Black);
        dice2Text.setString(turnManager.getPlayers().empty() ? "" : std::to_string(turnManager.getCurrentPlayer().getDice2()));
        dice2Text.setPosition(dice2.getPosition().x + 18.f, dice2.getPosition().y + 10.f);
        window.draw(dice2Text);

        sf::RectangleShape diceButton(sf::Vector2f(120, 50));
        diceButton.setPosition(static_cast<float>(window.getSize().x) - 180.f, 300.f);
        diceButton.setFillColor(sf::Color(60, 160, 60));
        diceButton.setOutlineColor(sf::Color::White);
        diceButton.setOutlineThickness(4.f);
        sf::Text diceButtonText("Rzut", font, 24);
        diceButtonText.setFillColor(sf::Color::White);
        diceButtonText.setStyle(sf::Text::Bold);
        diceButtonText.setPosition(diceButton.getPosition().x + 8.f, diceButton.getPosition().y + 10.f);
        window.draw(diceButton);
        window.draw(diceButtonText);

        cardManager.buyCardButton->draw(window);
        cardManager.showCardsButton->draw(window);
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
        if (cardManager.cardsVisible) {
            cardManager.showCards(window, font, turnManager.getCurrentPlayer());
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
            const auto& cards = cardManager.getPlayerCards().at(turnManager.getCurrentPlayer().getId());
            float y = 200.f;
            for (size_t i = 0; i < cards.size(); ++i) {
                sf::FloatRect cardRect(400.f, y, 400.f, 40.f);
                if (cardRect.contains(mousePos)) {
                }
                y += 40.f;
            }
        }

        knight.draw(window, board.getTiles(), hexSize);

        if (knightMoveMode) {
            for (const auto& btn : knightMoveButtons)
                btn->draw(window);
        }

        trade.draw(window);

        if (setupPhase) {
            sf::Text setupText;
            setupText.setFont(font);
            std::string msg;
            int playerNum = setupPlayerIndex + 1;
            if (setupStep == 0)
                msg = "Gracz " + std::to_string(playerNum) + " stawia akademik";
            else
                msg = "Gracz " + std::to_string(playerNum) + " stawia korytarz";
            setupText.setString(msg);
            setupText.setCharacterSize(40);
            setupText.setFillColor(sf::Color::Yellow);
            setupText.setStyle(sf::Text::Bold);
            setupText.setPosition(window.getSize().x / 2.f - setupText.getLocalBounds().width / 2.f, 10.f);
            window.draw(setupText);
        }

        std::string buildMsg;
        if (playerButtons.size() > 0 && playerButtons[0]->isClicked(mousePos)) {
            buildMsg = "Budowanie korytarza: 1x Kawa, 1x Kabel";
        } else if (playerButtons.size() > 1 && playerButtons[1]->isClicked(mousePos)) {
            buildMsg = "Budowanie akademika: 1x Pizza, 1x Piwo, 1x Notatki";
        } else if (playerButtons.size() > 2 && playerButtons[2]->isClicked(mousePos)) {
            buildMsg = "Budowanie kampusu: 2x Kawa, 2x Piwo, 2x Pizza";
        } else if (cardManager.buyCardButton && cardManager.buyCardButton->isClicked(mousePos)) {
            buildMsg = "Zakup karty: 1x Kabel, 1x Piwo, 1x Notatki";
        }

        if (!buildMsg.empty()) {
            sf::Text buildInfo;
            buildInfo.setFont(font);
            buildInfo.setCharacterSize(28);
            buildInfo.setFillColor(sf::Color::Cyan);
            buildInfo.setStyle(sf::Text::Bold);
            buildInfo.setString(buildMsg);
            buildInfo.setPosition(30.f, 0.f);
            window.draw(buildInfo);
        }
    }
    window.display();
}

void Game::setupPlayerButtons() {
    playerButtons.clear();
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj korytarz", sf::Vector2f(30, 350), [&]() {
        buildMode = BuildMode::Road;
        buildButtons.clear();
        auto& players = turnManager.getPlayers();
        int currentPlayer = turnManager.getCurrentPlayerIndex();
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj akademik", sf::Vector2f(30, 410), [&]() {
        buildMode = BuildMode::Settlement;
        buildButtons.clear();
        auto& players = turnManager.getPlayers();
        int currentPlayer = turnManager.getCurrentPlayerIndex();
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj kampus", sf::Vector2f(30, 470), [&]() {
        buildMode = BuildMode::City;
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Wymiana", sf::Vector2f(30, 530), [&]() {
        auto& players = turnManager.getPlayers();
        int currentPlayer = turnManager.getCurrentPlayerIndex();
        if (players.size() > 1) {
            trade.startTrade(font, players, currentPlayer, &logs);
        }
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Kolejna tura", sf::Vector2f(30, 590), [&]() {
        if (!turnManager.getPlayers().empty() && !turnManager.getCurrentPlayer().hasRolled()) {
            logs.add("Najpierw rzuć kostką!");
            return;
        }
        turnManager.nextTurn();
        logs.add("Tura gracza: " + std::to_string(turnManager.getCurrentPlayer().getId() + 1));
    }));

    cardManager.buyCardButton = std::make_unique<SimpleButton>(font, "Zakup karty", sf::Vector2f(30, 650), [&]() {
        if (!turnManager.getPlayers().empty()) cardManager.buyCard(turnManager.getCurrentPlayer());
    });
    cardManager.showCardsButton = std::make_unique<SimpleButton>(font, "Zobacz karty", sf::Vector2f(30, 710), [this]() {
        cardManager.cardsVisible = !cardManager.cardsVisible;
    });
}





