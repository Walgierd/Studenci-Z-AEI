#include "Knight.h"



sf::Vector2f Knight::getPosition(const std::vector<HexTile>& tiles) const {
    if (tileIndex >= 0 && tileIndex < static_cast<int>(tiles.size()))
        return tiles[tileIndex].getPosition();
    return sf::Vector2f(0, 0);
}

void Knight::setPosition(int idx) {
    tileIndex = idx;
}

void Knight::draw(sf::RenderWindow& window, const std::vector<HexTile>& tiles, float hexSize) const {
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

bool Knight::blocksTile(int idx) const {
    return tileIndex == idx;
}

void Knight::startMoveMode(
    std::vector<std::unique_ptr<BuildSpotButton>>& knightMoveButtons,
    const std::vector<HexTile>& tiles,
    int currentTileIndex,
    std::function<void(int)> onMove
) {
    knightMoveButtons.clear();
    for (size_t i = 0; i < tiles.size(); ++i) {
        if (static_cast<int>(i) == currentTileIndex) continue;
        knightMoveButtons.push_back(std::make_unique<KnightMoveButton>(
            tiles[i].getPosition(), static_cast<int>(i), onMove
        ));
    }
}

void Knight::handleMoveClick(
    const sf::Vector2f& mousePos,
    std::vector<std::unique_ptr<BuildSpotButton>>& knightMoveButtons,
    bool& knightMoveMode
) {
    for (auto& btn : knightMoveButtons) {
        if (btn->isClicked(mousePos)) {
            btn->onClick();
            knightMoveMode = false;
            knightMoveButtons.clear();
            break;
        }
    }
}

// Definicje KnightMoveButton
KnightMoveButton::KnightMoveButton(const sf::Vector2f& p, int idx, std::function<void(int)> cb) {
    pos = p;
    tileIdx = idx;
    moveCallback = cb;
}

void KnightMoveButton::onClick() {
    if (moveCallback) moveCallback(tileIdx);
    visible = false;
}