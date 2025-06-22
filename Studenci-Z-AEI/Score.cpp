#include "Score.h"
#include <algorithm>
#include <unordered_map>
#include <functional> // add this at the top

Score::Score() {}

void Score::updateScores(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::vector<int>& victoryPointCardsUsed) {
    playerScores.assign(players.size(), 0);
    victoryPointCards = victoryPointCardsUsed;

    std::map<int, int> playerIdToIndex;
    for (size_t i = 0; i < players.size(); ++i) {
        playerIdToIndex[players[i].getId()] = static_cast<int>(i);
    }

    // Count settlements and cities
    for (const auto& b : buildables) {
        if (auto* s = dynamic_cast<Settlement*>(b.get())) {
            auto it = playerIdToIndex.find(s->ownerId);
            if (it != playerIdToIndex.end()) {
                if (s->isCity)
                    playerScores[it->second] += 2;
                else
                    playerScores[it->second] += 1;
            }
        }
    }

    // Add Victory Point cards
    for (size_t i = 0; i < victoryPointCards.size(); ++i) {
        if (i < playerScores.size())
            playerScores[i] += victoryPointCards[i];
    }

    // Longest road
    calculateLongestRoad(players, buildables, playerIdToIndex);
    if (longestRoadOwner != -1) {
        auto it = playerIdToIndex.find(longestRoadOwner);
        if (it != playerIdToIndex.end() && it->second < (int)playerScores.size())
            playerScores[it->second] += 3;
    }
}

int Score::getScore(int playerId) const {
    // playerScores jest indeksowane jak players, więc musisz mieć mapę playerId->indeks
    // Ale jeśli zawsze przekazujesz indeks, to zostaw jak jest
    if (playerId >= 0 && playerId < (int)playerScores.size())
        return playerScores[playerId];
    return 0;
}

void Score::setVictoryPointCards(int playerId, int count) {
    if (playerId >= 0 && playerId < (int)victoryPointCards.size())
        victoryPointCards[playerId] = count;
}

int Score::getLongestRoadOwner() const {
    return longestRoadOwner;
}

// Helper: Find the player with the longest continuous road (>5)
void Score::calculateLongestRoad(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::map<int, int>& playerIdToIndex) {
    std::vector<int> roadCount(players.size(), 0);

    // Zlicz drogi każdego gracza
    for (const auto& b : buildables) {
        if (auto* r = dynamic_cast<Road*>(b.get())) {
            auto it = playerIdToIndex.find(r->ownerId);
            if (it != playerIdToIndex.end()) {
                int idx = it->second;
                roadCount[idx]++;
            }
        }
    }

    // Znajdź gracza z największą liczbą dróg (>7)
    int maxRoads = 7;
    int ownerIdx = -1;
    for (size_t i = 0; i < roadCount.size(); ++i) {
        if (roadCount[i] > maxRoads) {
            maxRoads = roadCount[i];
            ownerIdx = static_cast<int>(i);
        }
    }

    if (ownerIdx != -1)
        longestRoadOwner = players[ownerIdx].getId();
    else
        longestRoadOwner = -1;
}