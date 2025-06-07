#pragma once
#include <map>
#include <random>
#include "Resource.h"

class Player {
public:
    Player(int id) : id(id), hasRolledThisTurn(false), dice1(0), dice2(0) {}

    int getId() const { return id; }

    void addResource(ResourceType type, int count = 1) { resources[type] += count; }
    void removeResource(ResourceType type, int count = 1) { resources[type] = std::max(0, resources[type] - count); }
    int getResourceCount(ResourceType type) const {
        auto it = resources.find(type);
        return it != resources.end() ? it->second : 0;
    }
    const std::map<ResourceType, int>& getResources() const { return resources; }

    // Mechanika rzutu koœæmi
    void rollDice() {
        if (hasRolledThisTurn) return;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 6);
        dice1 = dist(gen);
        dice2 = dist(gen);
        hasRolledThisTurn = true;
    }

    void resetDice() {
        hasRolledThisTurn = false;
        dice1 = 0;
        dice2 = 0;
    }

    int getDice1() const { return dice1; }
    int getDice2() const { return dice2; }
    bool hasRolled() const { return hasRolledThisTurn; }

private:
    int id;
    std::map<ResourceType, int> resources;
    bool hasRolledThisTurn;
    int dice1, dice2;
};