#include "Studenci-Z-AEI.h"
#include "Menu.h"
#include "Player.h"
#include "PlayerUI.h"
#include "HexTile.h"
#include "Buildable.h"
#include "Knight.h"
#include "Trade.h"
#include "Cards.h" // Add this include to resolve the undeclared identifier error
#include "Menago.h"
#include <memory>
#include <vector>
#include <filesystem>
#include <ranges>
#include <cmath>
#include <thread>
#include <mutex>
#include <array>
#include <map>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Studenci z AEI", sf::Style::Default);
    unsigned int currentStyle = sf::Style::Default;

    Menu menu(1920, 1080);
    bool inMenu = true;

    sf::Vector2f windowCenter(1920.f / 2.f, 1080.f / 2.f);
    float hexSize = 80.f;
    Board board(hexSize, windowCenter);

    std::vector<Player> players;
    int currentPlayer = 0;
    int turnCounter = 1;

    sf::Font font;
    if (!std::filesystem::exists("Fonts/arial.ttf") || !font.loadFromFile("Fonts/arial.ttf")) {
        return 1;
    }

    std::vector<std::unique_ptr<Buildable>> buildables;
    std::mutex buildMutex;

    BuildMode buildMode = BuildMode::None;
    std::vector<std::unique_ptr<BuildSpotButton>> buildButtons;

    // --- Mechanika wymiany: TradeUI ---
    TradeUI trade;

    std::vector<std::unique_ptr<UIButton>> playerButtons;
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj drogê", sf::Vector2f(30, 350), [&](){
        buildMode = BuildMode::Road;
        buildButtons.clear();
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj osiedle", sf::Vector2f(30, 410), [&](){
        buildMode = BuildMode::Settlement;
        buildButtons.clear();
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj miasto", sf::Vector2f(30, 470), [&](){
        buildMode = BuildMode::City;
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Wymiana", sf::Vector2f(30, 530), [&](){
        if (players.size() > 1) {
            trade.startTrade(font, players, currentPlayer);
        }
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Nastêpna tura", sf::Vector2f(30, 590), [&](){
        nextPlayerTurn(players, currentPlayer, turnCounter, buildMode);
    }));

    auto giveTestResources = [](Player& p) {
        for (ResourceType t : std::array{ResourceType::Kawa, ResourceType::Energia, ResourceType::Notatki})
            p.addResource(t, 20);
    };

    auto drawPlayerResources = [&](const Player& player, sf::RenderWindow& window, const sf::Font& font) {
        int y = 200;
        for (ResourceType type : std::array{ResourceType::Kawa, ResourceType::Energia, ResourceType::Notatki}) {
            sf::Text resText;
            resText.setFont(font);
            std::string name;
            switch (type) {
                case ResourceType::Kawa: name = "Kawa"; break;
                case ResourceType::Energia: name = "Energia"; break;
                case ResourceType::Notatki: name = "Notatki"; break;
                default: name = "Brak";
            }
            resText.setString(name + ": " + std::to_string(player.getResourceCount(type)));
            resText.setCharacterSize(28);
            resText.setFillColor(sf::Color::White);
            resText.setPosition(30.f, static_cast<float>(y));
            window.draw(resText);
            y += 40;
        }
    };

    sf::Vector2f buildStart, buildEnd;
    bool buildFirstClick = true;

    Knight knight;
    knight.setPosition(static_cast<int>(board.getTiles().size() / 2));
    bool knightMoveMode = false;
    std::vector<std::unique_ptr<BuildSpotButton>> knightMoveButtons;

    // Przygotowanie przycisku rzutu koœæmi z poprawionym wygl¹dem
    sf::RectangleShape diceButton(sf::Vector2f(120, 50));
    diceButton.setPosition(static_cast<float>(window.getSize().x) - 180.f, 300.f);
    diceButton.setFillColor(sf::Color(60, 160, 60));
    diceButton.setOutlineColor(sf::Color::White);
    diceButton.setOutlineThickness(4.f);
    sf::Text diceButtonText("Rzut", font, 24);
    diceButtonText.setFillColor(sf::Color::White);
    diceButtonText.setStyle(sf::Text::Bold);
    diceButtonText.setPosition(diceButton.getPosition().x + 8.f, diceButton.getPosition().y + 10.f);

    CardManager cardManager;
    bool showCardsMode = false;

    cardManager.buyCardButton = std::make_unique<SimpleButton>(font, "Kup kartê", sf::Vector2f(30, 650), [&](){
        if (!players.empty()) cardManager.buyCard(players[currentPlayer]);
    });
    cardManager.showCardsButton = std::make_unique<SimpleButton>(font, "Zobacz karty", sf::Vector2f(30, 710), [&cardManager]() {
        cardManager.cardsVisible = !cardManager.cardsVisible;
    });

    bool freeBuildRoad = false;
    bool freeBuildSettlement = false; // Dodaj tê liniê

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (inMenu && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
                if (menu.isStartClicked(mousePos)) {
                    int playerCount = menu.getSelectedPlayerCount();
                    players.clear();
                    for (int i = 0; i < playerCount; ++i) {
                        players.emplace_back(i);
                        giveTestResources(players.back());
                    }
                    inMenu = false;
                }
                if (menu.isFullscreenClicked(mousePos)) {
                    menu.setFullscreenToggleRequested(true);
                }
            }

            if (!inMenu && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

                // --- Mechanika wymiany ---
                if (trade.exchangeMode) {
                    trade.handleClick(mousePos, players, currentPlayer);
                    continue;
                }

                if (knightMoveMode) {
                    knight.handleMoveClick(mousePos, knightMoveButtons, knightMoveMode);
                    continue;
                }

                for (auto& btn : playerButtons) {
                    if (btn->isClicked(mousePos)) {
                        btn->onClick();
                    }
                }

                if (cardManager.buyCardButton->isClicked(mousePos)) {
                    cardManager.buyCardButton->onClick();
                }

                if (cardManager.showCardsButton->isClicked(mousePos)) {
                    cardManager.showCardsButton->onClick();
                }

                if (diceButton.getGlobalBounds().contains(mousePos)) {
                    if (handleDiceRoll(players, currentPlayer, board, buildables, knight, hexSize)) {
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
                    std::vector<sf::Vector2f> hexCenters;
                    for (const auto& tile : board.getTiles())
                        hexCenters.push_back(tile.getPosition());
                    auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);
                    for (const auto& pos : settlementSpots) {
                        if (std::hypot(mousePos.x - pos.x, mousePos.y - pos.y) < 15.f) {
                            if (tryBuildSettlement(buildables, players, currentPlayer, pos, hexSize * std::sqrt(3.f) - 5, freeBuildSettlement)) {
                                buildMode = BuildMode::None;
                            }
                            break;
                        }
                    }
                } else if (buildMode == BuildMode::Road) {
                    std::vector<sf::Vector2f> hexCenters;
                    for (const auto& tile : board.getTiles())
                        hexCenters.push_back(tile.getPosition());
                    auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);
                    for (const auto& edge : roadSpots) {
                        sf::Vector2f mid = (edge.first + edge.second) / 2.f;
                        if (std::hypot(mousePos.x - mid.x, mousePos.y - mid.y) < 15.f) {
                            if (tryBuildRoad(buildables, players, currentPlayer, edge.first, edge.second, freeBuildRoad)) {
                                buildMode = BuildMode::None;
                            }
                            break;
                        }
                    }
                } else if (buildMode == BuildMode::City) {
                    if (tryBuildCity(buildables, players, currentPlayer, mousePos)) {
                        buildMode = BuildMode::None;
                    }
                }
            }
        }

        if (menu.isFullscreenToggleRequested()) {
            bool fullscreen = (currentStyle != sf::Style::Fullscreen);
            window.close();
            if (fullscreen) {
                window.create(sf::VideoMode::getDesktopMode(), "Studenci z AEI", sf::Style::Fullscreen);
                currentStyle = sf::Style::Fullscreen;
            } else {
                window.create(sf::VideoMode(1920, 1080), "Studenci z AEI", sf::Style::Default);
                currentStyle = sf::Style::Default;
            }
            menu.resetFullscreenToggleRequest();
        }

        window.clear();
        if (inMenu) {
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
            menu.update(mousePos);
            menu.draw(window);
        }
        else {
            board.draw(window);

            {
                std::lock_guard<std::mutex> lock(buildMutex);
                for (const auto& b : buildables)
                    b->draw(window);
            }

            sf::Text playerInfo;
            playerInfo.setFont(font);
            if (!players.empty()) {
                playerInfo.setString("Tura gracza: " + std::to_string(players[currentPlayer].getId() + 1));
            } else {
                playerInfo.setString("Brak graczy");
            }
            playerInfo.setCharacterSize(36);
            playerInfo.setFillColor(sf::Color::White);
            playerInfo.setPosition(30.f, 30.f);
            window.draw(playerInfo);

            drawPlayerResources(players[currentPlayer], window, font);

            for (auto& btn : playerButtons) btn->draw(window);

            if (buildMode == BuildMode::Road || buildMode == BuildMode::Settlement) {
                for (const auto& btn : buildButtons)
                    btn->draw(window);
            }

            sf::Text turnText;
            turnText.setFont(font);
            turnText.setString("Tura: " + std::to_string(turnCounter));
            turnText.setCharacterSize(28);
            turnText.setFillColor(sf::Color::White);
            turnText.setPosition(static_cast<float>(window.getSize().x) - turnText.getLocalBounds().width - 40.f, 30.f);
            window.draw(turnText);

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
            dice1Text.setString(players.empty() ? "" : std::to_string(players[currentPlayer].getDice1()));
            dice1Text.setPosition(dice1.getPosition().x + 18.f, dice1.getPosition().y + 10.f);
            window.draw(dice1Text);

            sf::Text dice2Text;
            dice2Text.setFont(font);
            dice2Text.setCharacterSize(36);
            dice2Text.setFillColor(sf::Color::Black);
            dice2Text.setString(players.empty() ? "" : std::to_string(players[currentPlayer].getDice2()));
            dice2Text.setPosition(dice2.getPosition().x + 18.f, dice2.getPosition().y + 10.f);
            window.draw(dice2Text);

            window.draw(diceButton);
            window.draw(diceButtonText);

            cardManager.buyCardButton->draw(window);
            cardManager.showCardsButton->draw(window);
            if (cardManager.cardsVisible) {
                cardManager.showCards(window, font, players[currentPlayer]);
                // Obs³uga klikniêcia na kartê
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
                    const auto& cards = cardManager.getPlayerCards().at(players[currentPlayer].getId());
                    float y = 200.f;
                    for (size_t i = 0; i < cards.size(); ++i) {
                        sf::FloatRect cardRect(400.f, y, 400.f, 40.f); // szerokoœæ i wysokoœæ jak w showCards
                        if (cardRect.contains(mousePos)) {
                            cardManager.useCard(
                                i, players[currentPlayer], buildables, board, knight, players,
                                buildMode, buildButtons, window, hexSize, currentPlayer,
                                freeBuildRoad, freeBuildSettlement, knightMoveMode, knightMoveButtons
                            );
                            break;
                        }
                        y += 40.f;
                    }
                }
            }

            knight.draw(window, board.getTiles(), hexSize);

            if (knightMoveMode) {
                for (const auto& btn : knightMoveButtons)
                    btn->draw(window);
            }

            // --- Rysowanie UI wymiany ---
            trade.draw(window);
        }

        window.display();

        if (!players.empty() && buildMode == BuildMode::None) {
            static int lastPlayer = -1;
            if (currentPlayer != lastPlayer) {
                players[currentPlayer].resetDice();
                lastPlayer = currentPlayer;
            }
        }
    }

    return 0;
}
