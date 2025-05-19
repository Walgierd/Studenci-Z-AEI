#include "HexTile.h"
#include <cmath>
#include <string>

HexTile::HexTile(float x, float y, float size, ResourceType resource, int number)
    : resourceType(resource), position(x, y), hexSize(size), number(number)
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

    if (resourceType != ResourceType::None) {
        float circleRadius = hexSize / 2.4f;
        sf::CircleShape circle(circleRadius);
        circle.setFillColor(sf::Color::Black);
        circle.setOrigin (sf::Vector2f(circleRadius, circleRadius));
        circle.setPosition(position);

        window.draw(circle);

        static sf::Font font("Fonts/arial.ttf");
        std::string numberStr = std::to_string(number);
        sf::Text text(font,numberStr, static_cast<unsigned int>(circleRadius * 0.8f));
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        sf::FloatRect bounds = text.getLocalBounds();
        text.setPosition(hexShape.getPosition());

        window.draw(text);
    }
}


ResourceType HexTile::getResourceType() const {
    return resourceType;
}

sf::Vector2f HexTile::getPosition() const {
    return position;
}
