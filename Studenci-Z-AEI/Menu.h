#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Menu {
public:
    Menu(float width, float height);
    void draw(sf::RenderWindow& window);
    bool isStartClicked(const sf::Vector2f& mousePos) const;
    void update(const sf::Vector2f& mousePos);
    bool isFullscreenClicked(const sf::Vector2f& mousePos) const;
    bool isFullscreenToggleRequested() const;
    void resetFullscreenToggleRequest();
    int getSelectedPlayerCount() const;
    void setFullscreenToggleRequested(bool value); // Add this method declaration

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
};
