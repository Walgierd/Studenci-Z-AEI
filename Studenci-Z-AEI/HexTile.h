#pragma once

#include "Resource.h"
#include "Drawable.h"

class HexTile : public Tile, public Drawable {
public:
    HexTile(float x, float y, float size, ResourceType resource, int number = 0);
    void draw(sf::RenderWindow& window) const override;
    ResourceType getResourceType() const override;
    sf::Vector2f getPosition() const override;
    int getNumber() const { return number; }

private:
    void setupHexShape(float size);
    ResourceType resourceType;
    sf::Vector2f position;
    sf::ConvexShape hexShape;
    float hexSize;
    int number;
};


