#pragma once
#include <map>
#include <random>
#include <set>
#include <string>
#include "Resource.h"
#include "HexTile.h"

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

    void setUsedCardThisTurn(bool used) { usedCardThisTurn = used; }
    bool hasUsedCardThisTurn() const { return usedCardThisTurn; }

    void addPort(PortType type) { ownedPorts.insert(type); }
    bool hasPort(PortType type) const { return ownedPorts.count(type) > 0; }
    bool hasAnyGenericPort() const { return ownedPorts.count(PortType::Generic) > 0; }
    const std::set<PortType>& getPorts() const { return ownedPorts; }

    void setNickname(const std::string& n) { nickname = n; }
    const std::string& getNickname() const { return nickname; }

private:
    int id;
    std::map<ResourceType, int> resources;
    bool hasRolledThisTurn;
    int dice1, dice2;
    bool usedCardThisTurn = false;
    std::set<PortType> ownedPorts;
    std::string nickname = "";
};