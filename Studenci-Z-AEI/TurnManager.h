#pragma once

#include "Player.h"
#include <vector>
#include <stdexcept>
#include <memory>


class TurnManager {
public:
    TurnManager(); 
    TurnManager(int playerCount); 
    void initialize(int playerCount);
    Player& getCurrentPlayer();
    const Player& getCurrentPlayer() const;
    int getCurrentPlayerIndex() const;
    int getTurnCounter() const;
    void nextTurn();
    std::vector<Player>& getPlayers();
    const std::vector<Player>& getPlayers() const;
    void giveTestResourcesToAll();

private:
    std::vector<Player> players;
    int currentPlayerIndex;
    int turnCounter;
};
