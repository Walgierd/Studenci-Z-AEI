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
    void setPosition(int idx);

    // Zwraca pozycjê rycerza (œrodek hexa)
    sf::Vector2f getPosition(const std::vector<HexTile>& tiles) const;

    // Rysowanie czarnego trójk¹ta na œrodku hexa
    void draw(sf::RenderWindow& window, const std::vector<HexTile>& tiles, float hexSize) const;

    // Czy rycerz blokuje dany hex?
    bool blocksTile(int idx) const;

    // Nowe metody:
    void startMoveMode(
        std::vector<std::unique_ptr<BuildSpotButton>>& knightMoveButtons,
        const std::vector<HexTile>& tiles,
        int currentTileIndex,
        std::function<void(int)> onMove
    );

    void handleMoveClick(
        const sf::Vector2f& mousePos,
        std::vector<std::unique_ptr<BuildSpotButton>>& knightMoveButtons,
        bool& knightMoveMode
    );
};

// Przycisk do przestawiania rycerza (okr¹g³y, jak do budowy)
struct KnightMoveButton : public BuildSpotButton {
    int tileIdx;
    std::function<void(int)> moveCallback;
    KnightMoveButton(const sf::Vector2f& p, int idx, std::function<void(int)> cb);
    void onClick() override;
};

