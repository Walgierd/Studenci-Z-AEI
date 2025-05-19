#pragma once

#include <SFML/Graphics.hpp>
#include <string>

enum class ResourceType { None, Kawa, Energia, Notatki };

class HexTile {
public:
    HexTile(float x, float y, float size, ResourceType resource);

    void draw(sf::RenderWindow& window) const;
    ResourceType getResourceType() const;
    sf::Vector2f getPosition() const;

private:
    sf::ConvexShape hexShape;
    ResourceType resourceType;
    sf::Vector2f position;
    float hexSize;
    void setupHexShape(float size);
};
