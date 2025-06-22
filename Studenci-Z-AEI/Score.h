#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Player.h"
#include "Buildable.h"

class Score {
public:
    Score();

    // Call this to update all scores
    void updateScores(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::vector<int>& victoryPointCardsUsed);

    // Get current score for a player
    int getScore(int playerId) const;

    // Set number of used Victory Point cards for a player
    void setVictoryPointCards(int playerId, int count);

    // Get playerId with the longest road, or -1 if none
    int getLongestRoadOwner() const;

private:
    std::vector<int> playerScores;
    std::vector<int> victoryPointCards; // indexed by playerId
    int longestRoadOwner = -1; // przechowuj playerId, nie indeks!
    void calculateLongestRoad(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::map<int, int>& playerIdToIndex);
};
