#pragma once
#include <SFML/Graphics.hpp>

class Drawable : public sf::Transformable {
public:
    virtual ~Drawable() = default;
    virtual void draw(sf::RenderWindow& window) const = 0;

    void setVisible(bool v) { visible = v; }
    bool isVisible() const { return visible; }

    void setZIndex(int z) { zIndex = z; }
    int getZIndex() const { return zIndex; }

    virtual bool contains(const sf::Vector2f&) const { return false; }

protected:
    bool visible = true;
    int zIndex = 0;
};