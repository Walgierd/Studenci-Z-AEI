﻿// Menago.cpp

#include "Menago.h"


void nextPlayerTurn(std::vector<Player>& players, int& currentPlayer, int& turnCounter, BuildMode& buildMode) {
    if (!players.empty()) {
        currentPlayer = (currentPlayer + 1) % players.size();
        if (currentPlayer == 0 && !players[currentPlayer].hasRolled()) {
        }
        if (currentPlayer == 0) ++turnCounter;
    }
    buildMode = BuildMode::None;
}

bool handleDiceRoll(
    std::vector<Player>& players,
    int currentPlayer,
    Board& board,
    std::vector<std::unique_ptr<Buildable>>& buildables,
    Knight& knight,
    float hexSize
) {
    if (players.empty() || players[currentPlayer].hasRolled())
        return false;

    players[currentPlayer].rollDice();
    int diceSum = players[currentPlayer].getDice1() + players[currentPlayer].getDice2();

    for (const auto& tile : board.getTiles()) {
        if (tile.getNumber() == diceSum && !knight.blocksTile(static_cast<int>(&tile - &board.getTiles()[0]))) {
            for (const auto& b : buildables) {
                if (auto* s = dynamic_cast<Settlement*>(b.get())) {
                    if (std::hypot(s->pos.x - tile.getPosition().x, s->pos.y - tile.getPosition().y) < hexSize + 2) {
                        int amount = s->isCity ? 2 : 1;
                        players[s->ownerId].addResource(tile.getResourceType(), amount);
                    }
                }
            }
        }
    }
    return true;
}

std::map<int, std::map<ResourceType, int>> handleDiceRollWithLog(
    std::vector<Player>& players,
    int currentPlayer,
    Board& board,
    std::vector<std::unique_ptr<Buildable>>& buildables,
    Knight& knight,
    float hexSize
) {
    std::map<int, std::map<ResourceType, int>> received;
    if (players.empty() || players[currentPlayer].hasRolled())
        return received;

    players[currentPlayer].rollDice();
    int diceSum = players[currentPlayer].getDice1() + players[currentPlayer].getDice2();

    for (const auto& tile : board.getTiles()) {
        if (tile.getNumber() == diceSum && !knight.blocksTile(static_cast<int>(&tile - &board.getTiles()[0]))) {
            for (const auto& b : buildables) {
                if (auto* s = dynamic_cast<Settlement*>(b.get())) {
                    if (std::hypot(s->pos.x - tile.getPosition().x, s->pos.y - tile.getPosition().y) < hexSize + 2) {
                        int amount = s->isCity ? 2 : 1;
                        players[s->ownerId].addResource(tile.getResourceType(), amount);
                        received[s->ownerId][tile.getResourceType()] += amount;
                    }
                }
            }
        }
    }
    return received;
}