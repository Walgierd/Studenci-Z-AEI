#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class ResourceType {
    None,
    Kawa,
    Energia,
    Notatki
};

class Tile {
public:
    virtual ~Tile() = default;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ResourceType getResourceType() const = 0;
    virtual sf::Vector2f getPosition() const = 0;
};