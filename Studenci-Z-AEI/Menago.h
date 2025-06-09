#pragma once
#include <vector>
#include <memory>
#include <cmath>
#include <map>
#include "Player.h"
#include "Buildable.h"
#include "Board.h"
#include "Knight.h"
#include "Trade.h"
#include <SFML/Graphics.hpp>

void nextPlayerTurn(std::vector<Player>& players, int& currentPlayer, int& turnCounter, BuildMode& buildMode);
bool handleDiceRoll(
    std::vector<Player>& players,
    int currentPlayer,
    Board& board,
    std::vector<std::unique_ptr<Buildable>>& buildables,
    Knight& knight,
    float hexSize
);
std::map<int, std::map<ResourceType, int>> handleDiceRollWithLog(
    std::vector<Player>& players,
    int currentPlayer,
    Board& board,
    std::vector<std::unique_ptr<Buildable>>& buildables,
    Knight& knight,
    float hexSize
);
