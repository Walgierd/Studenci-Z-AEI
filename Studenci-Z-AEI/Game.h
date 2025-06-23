#pragma once

#include "Board.h"
#include "Menu.h"
#include "Player.h"
#include "PlayerUI.h"
#include "Buildable.h"
#include "Knight.h"
#include "Trade.h"
#include "Cards.h"
#include "Resource.h"
#include "Logs.h"
#include "Menago.h"
#include "TurnManager.h"
#include "Score.h"  
#include <filesystem>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <memory>
#include <mutex>
#include <vector>

inline std::string PLAYER_NICK(const std::vector<Player>& players, int playerIndex) {//
    return players[playerIndex].getNickname();
}

class Game {
public:
    Game();
    void run();
    CardManager cardManager;
	Score score; 
private:
    sf::RenderWindow window;
    unsigned int currentStyle;
    Menu menu;
    PlayerUI playerUI;
    sf::Font font;
    Logs logs;

    bool inMenu;
    float hexSize;
    Board board;
    TurnManager turnManager;
    std::vector<Player> players;
    Player bank; 
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
    std::mutex logsMutex;

    bool setupPhase;
    std::vector<sf::Vector2f> lastSettlementPos;

    int setupTurn;
    int setupStep;
    int setupPlayerIndex;
    void handleEvents();
    void handleMenuEvents(const sf::Event& event);
    void handleGameEvents(const sf::Event& event);
    void update();
    void render();
    void setupPlayerButtons();
};

