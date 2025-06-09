#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include "Drawable.h"

class Menu : public Drawable {
public:
    Menu(unsigned int width, unsigned int height);
    void draw(sf::RenderWindow& window) const override;
    bool isStartClicked(const sf::Vector2f& mousePos) const;
    void update(const sf::Vector2f& mousePos);
    bool isFullscreenClicked(const sf::Vector2f& mousePos) const;
    bool isFullscreenToggleRequested() const;
    void resetFullscreenToggleRequest();
    int getSelectedPlayerCount() const;
    void setFullscreenToggleRequested(bool value); 

    void handleFullscreenToggle(sf::RenderWindow& window, unsigned int& currentStyle);

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
    bool fullscreenToggleRequested = false;
    std::vector<sf::RectangleShape> playerCountButtons;
    std::vector<sf::Text> playerCountTexts;
    int selectedPlayerCount = 2;
    unsigned int width;
    unsigned int height;
};