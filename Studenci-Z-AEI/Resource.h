#pragma once
#include <SFML/Graphics.hpp>
#include <string>


enum class ResourceType {
    None,
    Kawa,
    Piwo,
    Notatki,
    Pizza,
    Kabel,
    COUNT
};


class Tile {
public:
    virtual ~Tile() = default;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual ResourceType getResourceType() const = 0;
    virtual sf::Vector2f getPosition() const = 0;
};

std::string resourceName(ResourceType type);

namespace std {
    template <>
    struct hash<sf::Vector2f> {
        std::size_t operator()(const sf::Vector2f& v) const noexcept {
            std::size_t h1 = std::hash<float>{}(v.x);
            std::size_t h2 = std::hash<float>{}(v.y);
            return h1 ^ (h2 << 1); 
        }
    };
}

