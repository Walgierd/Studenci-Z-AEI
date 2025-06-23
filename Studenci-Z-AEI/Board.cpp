#include "Board.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
#include <ranges>

// Dodaj pole do przechowywania tekstury i sprite'a
static sf::Texture backgroundTexture;
static sf::Sprite backgroundSprite;

std::vector<sf::Vector2f> g_hexCenters;

Board::Board(float hexSize, sf::Vector2f center) {
    // Załaduj tło tylko raz
    static bool loaded = false;
    if (!loaded) {
        if (backgroundTexture.loadFromFile("Assets/Background.png")) {
            backgroundSprite.setTexture(backgroundTexture);
            // Dopasuj rozmiar do okna jeśli chcesz:
            // backgroundSprite.setScale(...);
        }
        loaded = true;
    }
    generateTiles(hexSize, center);
}

void Board::generateTiles(float hexSize, sf::Vector2f center) {
    const int rows[] = { 3, 4, 5, 4, 3 };
    const int numRows = 5;
    const float hexHeight = hexSize * 2;
    const float hexWidth = std::sqrt(3.f) * hexSize;
    const float vertDist = 0.75f * hexHeight;

    std::vector<ResourceType> resources = shuffledResources();
    std::vector<int> numbers = shuffledNumbers();

    int resourceIdx = 0;
    int numberIdx = 0;
    float startY = center.y - vertDist * 2;
    for (int row = 0; row < numRows; ++row) {
        int numInRow = rows[row];
        float y = startY + row * vertDist;
        float startX = center.x - hexWidth * (numInRow - 1) / 2.f;
        for (int col = 0; col < numInRow; ++col) {
            float x = startX + col * hexWidth;

            if (row == 2 && col == 2) {
                tiles.emplace_back(x, y, hexSize, ResourceType::None, 0);
            }
            else {
                tiles.emplace_back(x, y, hexSize, resources[resourceIdx++], numbers[numberIdx++]);
            }
        }
    }
}

std::vector<ResourceType> Board::shuffledResources() {
    std::vector<ResourceType> resources = {
        ResourceType::Kawa, ResourceType::Kawa, ResourceType::Kawa, ResourceType::Kawa,
        ResourceType::Piwo, ResourceType::Piwo, ResourceType::Piwo, ResourceType::Piwo,
        ResourceType::Notatki, ResourceType::Notatki, ResourceType::Notatki, ResourceType::Notatki,
        ResourceType::Pizza, ResourceType::Pizza, ResourceType::Pizza, ResourceType::Kabel,
        ResourceType::Kabel, ResourceType::Kabel
    };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(resources.begin(), resources.end(), g);
    return resources;
}
std::vector<int> Board::shuffledNumbers() {
    auto base = std::views::iota(2, 12);
    std::vector<int> numbers;
    for (int n : base) {
        numbers.push_back(n);
        numbers.push_back(n);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(numbers.begin(), numbers.end(), g);
    return numbers;
}

void Board::draw(sf::RenderWindow& window) const {
    // Najpierw rysuj tło
    window.draw(backgroundSprite);
    // Potem kafelki
    for (const auto& tile : tiles)
        tile.draw(window);
}

const std::vector<HexTile>& Board::getTiles() const {
    return tiles;
}
