#include "HexTile.h"

HexTile::HexTile(float x, float y, float size, ResourceType resource)
    : resourceType(resource), position(x, y)
{
    setupHexShape(size);
    hexShape.setPosition(position);
    switch (resourceType) {
    case ResourceType::Kawa:    hexShape.setFillColor(sf::Color(139, 69, 19)); break; 
    case ResourceType::Energia: hexShape.setFillColor(sf::Color::Yellow); break;
    case ResourceType::Notatki: hexShape.setFillColor(sf::Color::White); break;
    default:                    hexShape.setFillColor(sf::Color(100, 100, 100)); break;
    }
}

void HexTile::setupHexShape(float size) {
    hexShape.setPointCount(6);
    for (int i = 0; i < 6; ++i) {
        float angle = 3.14159f / 3.f * i - 3.14159f / 6.f;
        hexShape.setPoint(i, { size * std::cos(angle), size * std::sin(angle) });
    }
}

void HexTile::draw(sf::RenderWindow& window) const {
    window.draw(hexShape);
}

ResourceType HexTile::getResourceType() const {
    return resourceType;
}

sf::Vector2f HexTile::getPosition() const {
    return position;
}
