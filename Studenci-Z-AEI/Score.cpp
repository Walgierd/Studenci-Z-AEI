#include "Score.h"
#include <algorithm>
#include <unordered_map>
#include <functional> 

Score::Score() {}

void Score::updateScores(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::vector<int>& victoryPointCardsUsed) {
    playerScores.assign(players.size(), 0);
    victoryPointCards = victoryPointCardsUsed;

    std::map<int, int> playerIdToIndex;
    for (size_t i = 0; i < players.size(); ++i) {
        playerIdToIndex[players[i].getId()] = static_cast<int>(i);
    }

//osiedla
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

    //pkt zwyc
    for (size_t i = 0; i < victoryPointCards.size(); ++i) {
        if (i < playerScores.size())
            playerScores[i] += victoryPointCards[i];
    }

    
    calculateLongestRoad(players, buildables, playerIdToIndex);
    if (longestRoadOwner != -1) {
        auto it = playerIdToIndex.find(longestRoadOwner);
        if (it != playerIdToIndex.end() && it->second < (int)playerScores.size())
            playerScores[it->second] += 3;
    }
}

int Score::getScore(int playerId) const {
   
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


void Score::calculateLongestRoad(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::map<int, int>& playerIdToIndex) {
    std::vector<int> roadCount(players.size(), 0);

    
    for (const auto& b : buildables) {
        if (auto* r = dynamic_cast<Road*>(b.get())) {
            auto it = playerIdToIndex.find(r->ownerId);
            if (it != playerIdToIndex.end()) {
                int idx = it->second;
                roadCount[idx]++;
            }
        }
    }

 
    int maxRoads = 7;//tu próg punktów za drogi <------------------------------------------
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