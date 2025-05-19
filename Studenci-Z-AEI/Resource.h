#pragma once
#include <SFML/Graphics.hpp>

enum class ResourceType {
    Kawa,
    Energia,
    Notatki,
    None
};

class HexTile {
public:
    HexTile(float x, float y, float size, ResourceType resource);
    void draw(sf::RenderWindow& window) const;
    ResourceType getResourceType() const;
    sf::Vector2f getPosition() const;

private:
    void setupHexShape(float size);

    ResourceType resourceType;
    sf::Vector2f position;
    sf::ConvexShape hexShape;
    sf::CircleShape circleShape;
    float hexSize;
};