#pragma once
#include "HexTile.h"
#include <vector>
#include <random>

class Board {
public:
    Board(float hexSize, sf::Vector2f center);
    void draw(sf::RenderWindow& window) const;
    const std::vector<HexTile>& getTiles() const;

private:
    std::vector<HexTile> tiles;
    void generateTiles(float hexSize, sf::Vector2f center);
    std::vector<ResourceType> shuffledResources();
};