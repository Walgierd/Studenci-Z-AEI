#pragma once

#include "Board.h"
#include "Menu.h"
#include "Player.h"
#include "PlayerUI.h"
#include "Buildable.h"
#include "Knight.h"
#include "Trade.h"
#include "Cards.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <mutex>
#include <vector>

class Game {
public:
    Game();
    void run();
    CardManager cardManager;

private:
    sf::RenderWindow window;
    unsigned int currentStyle;
    Menu menu;
    PlayerUI playerUI;
    sf::Font font;

    bool inMenu;
    float hexSize;
    Board board;
    std::vector<Player> players;
    int currentPlayer;
    int turnCounter;
    std::vector<std::unique_ptr<Buildable>> buildables;
    std::mutex buildMutex;
    BuildMode buildMode;
    std::vector<std::unique_ptr<BuildSpotButton>> buildButtons;
    TradeUI trade;
    std::vector<std::unique_ptr<UIButton>> playerButtons;
    Knight knight;
    bool knightMoveMode;
    std::vector<std::unique_ptr<BuildSpotButton>> knightMoveButtons;
    bool freeBuildRoad;
    bool freeBuildSettlement;

    void handleEvents();
    void handleMenuEvents(const sf::Event& event);
    void handleGameEvents(const sf::Event& event);
    void update();
    void render();
    void setupPlayerButtons();
    void giveTestResources(Player& p);
};