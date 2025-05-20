#pragma once
#include <SFML/Graphics.hpp>

class Menu {
public:
    Menu(float width, float height);
    void draw(sf::RenderWindow& window);
    bool isStartClicked(const sf::Vector2f& mousePos) const;
    void update(const sf::Vector2f& mousePos);

private:
    sf::Sprite background;
    sf::Texture bgTexture;
    sf::Font font;
    sf::Text title;
    sf::Text startButton;
    sf::RectangleShape startButtonBox;
    bool startButtonHovered = false;
};
