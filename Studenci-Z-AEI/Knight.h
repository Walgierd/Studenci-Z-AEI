#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "Buildable.h"
#include "HexTile.h"


// Klasa Rycerza
class Knight {
public:
    // Indeks hexa, na którym stoi rycerz
    int tileIndex = -1;

    // Ustaw rycerza na danym polu (indeks w Board::tiles)
    void setPosition(int idx) { tileIndex = idx; }

    // Zwraca pozycjê rycerza (œrodek hexa)
    sf::Vector2f getPosition(const std::vector<HexTile>& tiles) const {
        if (tileIndex >= 0 && tileIndex < static_cast<int>(tiles.size()))
            return tiles[tileIndex].getPosition();
        return sf::Vector2f(0, 0);
    }

    // Rysowanie czarnego trójk¹ta na œrodku hexa
    void draw(sf::RenderWindow& window, const std::vector<HexTile>& tiles, float hexSize) const {
        if (tileIndex < 0 || tileIndex >= static_cast<int>(tiles.size())) return;
        sf::Vector2f center = tiles[tileIndex].getPosition();
        sf::ConvexShape triangle;
        triangle.setPointCount(3);
        float r = hexSize * 0.5f;
        triangle.setPoint(0, sf::Vector2f(center.x, center.y - r));
        triangle.setPoint(1, sf::Vector2f(center.x - r * 0.87f, center.y + r * 0.5f));
        triangle.setPoint(2, sf::Vector2f(center.x + r * 0.87f, center.y + r * 0.5f));
        triangle.setFillColor(sf::Color::Black);
        window.draw(triangle);
    }

    // Czy rycerz blokuje dany hex?
    bool blocksTile(int idx) const { return tileIndex == idx; }
};

// Przycisk do przestawiania rycerza (okr¹g³y, jak do budowy)
struct KnightMoveButton : public BuildSpotButton {
    int tileIdx;
    std::function<void(int)> moveCallback;
    KnightMoveButton(const sf::Vector2f& p, int idx, std::function<void(int)> cb) {
        pos = p;
        tileIdx = idx;
        moveCallback = cb;
    }
    void onClick() override {
        if (moveCallback) moveCallback(tileIdx);
        visible = false;
    }
};

