#pragma once
#include <SFML/Graphics.hpp>

class Menu {
public:
    Menu(float width, float height);
    void draw(sf::RenderWindow& window);
    bool isStartClicked(const sf::Vector2f& mousePos) const;
    void update(const sf::Vector2f& mousePos);
    bool isFullscreenClicked(const sf::Vector2f& mousePos) const;
    bool fullscreenToggleRequested = false;

private:

    sf::Sprite background;
    sf::Texture bgTexture;
    sf::Font font;
    sf::Texture startButtonTexture;
    sf::Texture startButtonHoverTexture;
    sf::Sprite startButtonSprite;
    sf::RectangleShape startButtonBox;
    sf::Texture fullscreenButtonTexture;
    sf::Sprite fullscreenButtonSprite;
    bool startButtonHovered = false;
    bool fullscreenButtonHovered = false;
public:
    bool isFullscreenToggleRequested() const;
    void resetFullscreenToggleRequest();
};
