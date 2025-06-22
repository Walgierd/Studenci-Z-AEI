#include "Buildplace.h"

Buildplace::Buildplace(float x, float y, float size, ResourceType resource, int number)
    : HexTile(x, y, size, resource, number), status(BuildStatus::Free), owner(nullptr) {}

BuildStatus Buildplace::getStatus() const {
    return status;
}

void Buildplace::setStatus(BuildStatus newStatus) {
    status = newStatus;
}

Player* Buildplace::getOwner() const {
    return owner;
}

void Buildplace::setOwner(Player* newOwner) {
    owner = newOwner;
}

bool Buildplace::canBuild(const Player& player) const {
    return status == BuildStatus::Free &&
           player.getResourceCount(ResourceType::Pizza) >= 1 &&
           player.getResourceCount(ResourceType::Piwo) >= 1 &&
           player.getResourceCount(ResourceType::Notatki) >= 1;
}

void Buildplace::build(Player& player) {
    if (canBuild(player)) {
        player.removeResource(ResourceType::Pizza, 1);
        player.removeResource(ResourceType::Piwo, 1);
        player.removeResource(ResourceType::Notatki, 1);
        setStatus(BuildStatus::Occupied);
        setOwner(&player);
    }
}