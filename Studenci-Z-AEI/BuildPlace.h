#pragma once
#include "HexTile.h"
#include "Player.h"

enum class BuildStatus {
    Free,
    Occupied
};

class Buildplace : public HexTile {
public:
    Buildplace(float x, float y, float size, ResourceType resource, int number = 0);

    BuildStatus getStatus() const;
    void setStatus(BuildStatus status);

    Player* getOwner() const;
    void setOwner(Player* owner);

    bool canBuild(const Player& player) const;
    void build(Player& player);

private:
    BuildStatus status;
    Player* owner;
};
