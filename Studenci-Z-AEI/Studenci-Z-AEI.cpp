#include "Studenci-Z-AEI.h"
#include "Menu.h"
#include "Player.h"
#include "PlayerUI.h"
#include "HexTile.h" 
#include "Buildable.h"
#include <memory>
#include <vector>
#include <filesystem>
#include <ranges>
#include <cmath>
#include <thread>
#include <mutex>
#include <array>

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
        // TODO: logika wymiany
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Nastêpna tura", sf::Vector2f(30, 590), [&](){
        if (!players.empty()) {
            currentPlayer = (currentPlayer + 1) % players.size();
            if (currentPlayer == 0 && !players[currentPlayer].hasRolled()) { 
                // Additional logic can be added here if needed
            }
            if (currentPlayer == 0) ++turnCounter;
        }
        buildMode = BuildMode::None;
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

    // Przygotowanie przycisku rzutu koœæmi z poprawionym wygl¹dem
    sf::RectangleShape diceButton(sf::Vector2f(120, 50));
    diceButton.setPosition(static_cast<float>(window.getSize().x) - 180.f, 300.f);
    diceButton.setFillColor(sf::Color(60, 160, 60)); // Zielony
    diceButton.setOutlineColor(sf::Color::White);
    diceButton.setOutlineThickness(4.f);
    sf::Text diceButtonText("Rzut", font, 24);
    diceButtonText.setFillColor(sf::Color::White);
    diceButtonText.setStyle(sf::Text::Bold);
    diceButtonText.setPosition(diceButton.getPosition().x + 8.f, diceButton.getPosition().y + 10.f);

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

                for (auto& btn : playerButtons) {
                    if (btn->isClicked(mousePos)) {
                        btn->onClick();
                    }
                }

                if (diceButton.getGlobalBounds().contains(mousePos)) {
                    if (!players.empty() && !players[currentPlayer].hasRolled()) {
                        players[currentPlayer].rollDice();
                        int diceSum = players[currentPlayer].getDice1() + players[currentPlayer].getDice2();
                        for (const auto& tile : board.getTiles()) {
                            if (tile.getNumber() == diceSum) {
                                for (const auto& b : buildables) {
                                    if (auto* s = dynamic_cast<Settlement*>(b.get())) {
                                        if (std::hypot(s->pos.x - tile.getPosition().x, s->pos.y - tile.getPosition().y) < hexSize + 2) {
                                            players[s->ownerId].addResource(tile.getResourceType(), s->isCity ? 2 : 1);
                                        }
                                    }
                                }
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
                            // Minimalna odleg³oœæ: dwie krawêdzie heksa (czyli hexSize * sqrt(3))
                            if (isSettlementFarEnough(buildables, pos, hexSize * std::sqrt(3.f) - 5)) {
                                if (players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 1 &&
                                    players[currentPlayer].getResourceCount(ResourceType::Energia) >= 1 &&
                                    players[currentPlayer].getResourceCount(ResourceType::Notatki) >= 1) {
                                    players[currentPlayer].removeResource(ResourceType::Kawa, 1);
                                    players[currentPlayer].removeResource(ResourceType::Energia, 1);
                                    players[currentPlayer].removeResource(ResourceType::Notatki, 1);
                                    auto settlement = std::make_unique<Settlement>();
                                    settlement->ownerId = players[currentPlayer].getId();
                                    settlement->pos = pos;
                                    buildables.push_back(std::move(settlement));
                                    buildMode = BuildMode::None;
                                }
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
                            if (isRoadConnected(buildables, edge.first, edge.second, players[currentPlayer].getId())) {
                                if (players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 1 &&
                                    players[currentPlayer].getResourceCount(ResourceType::Energia) >= 1) {
                                    players[currentPlayer].removeResource(ResourceType::Kawa, 1);
                                    players[currentPlayer].removeResource(ResourceType::Energia, 1);
                                    auto road = std::make_unique<Road>();
                                    road->ownerId = players[currentPlayer].getId();
                                    road->start = edge.first;
                                    road->end = edge.second;
                                    buildables.push_back(std::move(road));
                                    buildMode = BuildMode::None;
                                }
                            }
                            break;
                        }
                    }
                } else if (buildMode == BuildMode::City) {
                    for (auto& b : buildables | std::views::filter([](const std::unique_ptr<Buildable>& b) {
                        return dynamic_cast<Settlement*>(b.get()) != nullptr;
                    })) {
                        auto* s = dynamic_cast<Settlement*>(b.get());
                        if (!s->isCity && s->ownerId == players[currentPlayer].getId() &&
                            std::hypot(s->pos.x - mousePos.x, s->pos.y - mousePos.y) < 40.f) {
                            if (players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 2 &&
                                players[currentPlayer].getResourceCount(ResourceType::Energia) >= 2 &&
                                players[currentPlayer].getResourceCount(ResourceType::Notatki) >= 2) {
                                players[currentPlayer].removeResource(ResourceType::Kawa, 2);
                                players[currentPlayer].removeResource(ResourceType::Energia, 2);
                                players[currentPlayer].removeResource(ResourceType::Notatki, 2);
                                s->isCity = true;
                            }
                            break;
                        }
                    }
                    buildMode = BuildMode::None;
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
        }

        window.display();

        if (!players.empty() && buildMode == BuildMode::None) {
            static int lastTurn = -1;
            if (turnCounter != lastTurn) {
                players[currentPlayer].resetDice();
                lastTurn = turnCounter;
            }
        }
    }

    return 0;
}
