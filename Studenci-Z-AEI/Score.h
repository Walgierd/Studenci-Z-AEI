#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Player.h"
#include "Buildable.h"

class Score {
public:
    Score();

   
    void updateScores(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::vector<int>& victoryPointCardsUsed);

   
    int getScore(int playerId) const;

   
    void setVictoryPointCards(int playerId, int count);

 
    int getLongestRoadOwner() const;

private:
    std::vector<int> playerScores;
    std::vector<int> victoryPointCards; 
    int longestRoadOwner = -1; 
    void calculateLongestRoad(const std::vector<Player>& players, const std::vector<std::unique_ptr<Buildable>>& buildables, const std::map<int, int>& playerIdToIndex);
};
