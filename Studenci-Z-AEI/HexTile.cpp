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
    case ResourceType::Piwo:    hexShape.setFillColor(sf::Color(255, 215, 0)); break; // z³oty kolor piwa
    case ResourceType::Notatki: hexShape.setFillColor(sf::Color::White); break;
    case ResourceType::Pizza:   hexShape.setFillColor(sf::Color(255, 99, 71)); break;
    case ResourceType::Kabel:   hexShape.setFillColor(sf::Color(128, 128, 128)); break;
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
        circle.setOrigin(circleRadius, circleRadius);
        circle.setPosition(position);

        window.draw(circle);

        static sf::Font font;
        static bool fontLoaded = false;
        if (!fontLoaded) {
            fontLoaded = font.loadFromFile("Fonts/arial.ttf");
        }
        std::string numberStr = std::to_string(number);
        sf::Text text;
        text.setFont(font);
        text.setString(numberStr);
        text.setCharacterSize(static_cast<unsigned int>(circleRadius * 0.8f));
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        text.setPosition(position);

        window.draw(text);
    }
}

ResourceType HexTile::getResourceType() const {
    return resourceType;
}

sf::Vector2f HexTile::getPosition() const {
    return position;
}
