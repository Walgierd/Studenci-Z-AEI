#include "TurnManager.h"
#include "Resource.h"

TurnManager::TurnManager() : currentPlayerIndex(0), turnCounter(0) {}

TurnManager::TurnManager(int playerCount) : currentPlayerIndex(0), turnCounter(0) {
    initialize(playerCount);
}

void TurnManager::initialize(int playerCount) {
    if (playerCount <= 0) {
        throw std::invalid_argument("Player count must be greater than zero.");
    }
    players.clear();
    for (int i = 0; i < playerCount; ++i) {
        players.emplace_back(i); 
    }
    currentPlayerIndex = 0;
    turnCounter = 0;
}

Player& TurnManager::getCurrentPlayer() {
    return players.at(currentPlayerIndex);
}

const Player& TurnManager::getCurrentPlayer() const {
    return players.at(currentPlayerIndex);
}

int TurnManager::getCurrentPlayerIndex() const {
    return currentPlayerIndex;
}

int TurnManager::getTurnCounter() const {
    return turnCounter;
}

void TurnManager::nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    if (currentPlayerIndex == 0) {
        ++turnCounter;
    }
    players[currentPlayerIndex].resetDice();
}

std::vector<Player>& TurnManager::getPlayers() {
    return players;
}

const std::vector<Player>& TurnManager::getPlayers() const {
    return players;
}

void TurnManager::giveTestResourcesToAll() {
    for (auto& player : players) {
        for (int resourceType = 0; resourceType < static_cast<int>(ResourceType::COUNT); ++resourceType) {
            player.addResource(static_cast<ResourceType>(resourceType), 10);
        }
    }
}