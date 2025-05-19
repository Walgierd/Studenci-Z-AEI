#include "Board.h"
#include <algorithm>
#include <cmath>

Board::Board(float hexSize, sf::Vector2f center) {
    generateTiles(hexSize, center);
}

void Board::generateTiles(float hexSize, sf::Vector2f center) {
    // Uk³ad 3-4-5-4-3
    const int rows[] = { 3, 4, 5, 4, 3 };
    const int numRows = 5;
    const float hexHeight = hexSize * 2;
    const float hexWidth = std::sqrt(3.f) * hexSize;
    const float vertDist = 0.75f * hexHeight;

    std::vector<ResourceType> resources = shuffledResources();

    int resourceIdx = 0;
    float startY = center.y - vertDist * 2;
    for (int row = 0; row < numRows; ++row) {
        int numInRow = rows[row];
        float y = startY + row * vertDist;
        float startX = center.x - hexWidth * (numInRow - 1) / 2.f;
        for (int col = 0; col < numInRow; ++col) {
            float x = startX + col * hexWidth;
            tiles.emplace_back(x, y, hexSize, resources[resourceIdx++]);
        }
    }
}

std::vector<ResourceType> Board::shuffledResources() {
    // Przyk³adowy rozk³ad: 6 kawa, 6 energia, 6 notatki, 1 pusty
    std::vector<ResourceType> resources = {
        ResourceType::Kawa, ResourceType::Kawa, ResourceType::Kawa,
        ResourceType::Kawa, ResourceType::Kawa, ResourceType::Kawa,
        ResourceType::Energia, ResourceType::Energia, ResourceType::Energia,
        ResourceType::Energia, ResourceType::Energia, ResourceType::Energia,
        ResourceType::Notatki, ResourceType::Notatki, ResourceType::Notatki,
        ResourceType::Notatki, ResourceType::Notatki, ResourceType::Notatki,
        ResourceType::None
    };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(resources.begin(), resources.end(), g);
    return resources;
}

void Board::draw(sf::RenderWindow& window) const {
    for (const auto& tile : tiles)
        tile.draw(window);
}

const std::vector<HexTile>& Board::getTiles() const {
    return tiles;
}