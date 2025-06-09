
#include "Buildable.h"



bool tryBuildSettlement(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos,
    float minDist,
    bool& freeBuildSettlement,
    bool setupPhase,
    Logs* logs 
) {
    if (!isSettlementFarEnough(buildables, pos, minDist))
        return false;

    if (setupPhase ||
        (players[currentPlayer].getResourceCount(ResourceType::Pizza) >= 1 &&
         players[currentPlayer].getResourceCount(ResourceType::Piwo) >= 1 &&
         players[currentPlayer].getResourceCount(ResourceType::Notatki) >= 1) ||
        freeBuildSettlement) {
        if (!setupPhase && !freeBuildSettlement) {
            players[currentPlayer].removeResource(ResourceType::Pizza, 1);
            players[currentPlayer].removeResource(ResourceType::Piwo, 1);
            players[currentPlayer].removeResource(ResourceType::Notatki, 1);
        }
        auto settlement = std::make_unique<Settlement>();
        settlement->ownerId = players[currentPlayer].getId();
        settlement->pos = pos;
        buildables.push_back(std::move(settlement));
        freeBuildSettlement = false;
        if (logs) logs->add("Gracz " + std::to_string(players[currentPlayer].getId() + 1) + " buduje akademik");
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
    bool& freeBuildRoad,
    bool setupPhase,
    const sf::Vector2f& lastSettlementPos
) {
    bool valid = false;
    if (setupPhase) {
        float distStart = std::hypot(start.x - lastSettlementPos.x, start.y - lastSettlementPos.y);
        float distEnd = std::hypot(end.x - lastSettlementPos.x, end.y - lastSettlementPos.y);
        if (distStart < 1e-2 || distEnd < 1e-2) valid = true;
    } else {
        valid = isRoadConnected(buildables, start, end, players[currentPlayer].getId());
    }

    if (valid || freeBuildRoad) {
        if (setupPhase ||
            (players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 1 &&
             players[currentPlayer].getResourceCount(ResourceType::Kabel) >= 1) ||
            freeBuildRoad) {
            if (!setupPhase && !freeBuildRoad) {
                players[currentPlayer].removeResource(ResourceType::Kawa, 1);
                players[currentPlayer].removeResource(ResourceType::Kabel, 1);
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
                players[currentPlayer].getResourceCount(ResourceType::Piwo) >= 2 &&
                players[currentPlayer].getResourceCount(ResourceType::Pizza) >= 2) {
                players[currentPlayer].removeResource(ResourceType::Kawa, 2);
                players[currentPlayer].removeResource(ResourceType::Piwo, 2);
                players[currentPlayer].removeResource(ResourceType::Pizza, 2);
                s->isCity = true;
                return true;
            }
        }
    }
    return false;
}

