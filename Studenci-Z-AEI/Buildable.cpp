#include "Buildable.h"
#include "Player.h"
#include <cmath>

bool tryBuildSettlement(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos,
    float minDist,
    bool& freeBuildSettlement
) {
    if (!isSettlementFarEnough(buildables, pos, minDist))
        return false;

    if ((players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 1 &&
         players[currentPlayer].getResourceCount(ResourceType::Energia) >= 1 &&
         players[currentPlayer].getResourceCount(ResourceType::Notatki) >= 1) || freeBuildSettlement) {
        if (!freeBuildSettlement) {
            players[currentPlayer].removeResource(ResourceType::Kawa, 1);
            players[currentPlayer].removeResource(ResourceType::Energia, 1);
            players[currentPlayer].removeResource(ResourceType::Notatki, 1);
        }
        auto settlement = std::make_unique<Settlement>();
        settlement->ownerId = players[currentPlayer].getId();
        settlement->pos = pos;
        buildables.push_back(std::move(settlement));
        freeBuildSettlement = false;
        return true;
    }
    return false;
}

bool tryBuildRoad(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& start,
    const sf::Vector2f& end,
    bool& freeBuildRoad
) {
    if (isRoadConnected(buildables, start, end, players[currentPlayer].getId()) || freeBuildRoad) {
        if ((players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 1 &&
             players[currentPlayer].getResourceCount(ResourceType::Energia) >= 1) || freeBuildRoad) {
            if (!freeBuildRoad) {
                players[currentPlayer].removeResource(ResourceType::Kawa, 1);
                players[currentPlayer].removeResource(ResourceType::Energia, 1);
            }
            auto road = std::make_unique<Road>();
            road->ownerId = players[currentPlayer].getId();
            road->start = start;
            road->end = end;
            buildables.push_back(std::move(road));
            freeBuildRoad = false;
            return true;
        }
    }
    return false;
}

bool tryBuildCity(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos
) {
    for (auto& b : buildables) {
        auto* s = dynamic_cast<Settlement*>(b.get());
        if (s && !s->isCity && s->ownerId == players[currentPlayer].getId() &&
            std::hypot(s->pos.x - pos.x, s->pos.y - pos.y) < 40.f) {
            if (players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 2 &&
                players[currentPlayer].getResourceCount(ResourceType::Energia) >= 2 &&
                players[currentPlayer].getResourceCount(ResourceType::Notatki) >= 2) {
                players[currentPlayer].removeResource(ResourceType::Kawa, 2);
                players[currentPlayer].removeResource(ResourceType::Energia, 2);
                players[currentPlayer].removeResource(ResourceType::Notatki, 2);
                s->isCity = true;
                return true;
            }
        }
    }
    return false;
}