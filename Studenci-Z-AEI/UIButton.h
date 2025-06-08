#pragma once
#include <SFML/Graphics.hpp>

class UIButton {
public:
    virtual ~UIButton() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual bool isClicked(const sf::Vector2f& mouse) const = 0;
    virtual void onClick() = 0;
};
