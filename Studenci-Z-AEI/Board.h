#pragma once
#include "HexTile.h"
#include <vector>
#include <random>
#include "Drawable.h"

class Board : public Drawable {
public:
    Board(float hexSize, sf::Vector2f center);
    void draw(sf::RenderWindow& window) const override;
    const std::vector<HexTile>& getTiles() const;

private:
    std::vector<HexTile> tiles;
    void generateTiles(float hexSize, sf::Vector2f center);
    std::vector<ResourceType> shuffledResources();
    std::vector<int> shuffledNumbers();
};
