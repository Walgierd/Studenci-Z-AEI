#include "Menu.h"

Menu::Menu(float width, float height) {
    bgTexture.loadFromFile("Assets/Menu2.png");
    background.setTexture(bgTexture);

    font.loadFromFile("Fonts/pixel.ttf");

    startButtonTexture.loadFromFile("Assets/Start.png");
    startButtonHoverTexture.loadFromFile("Assets/Start-on.png");
    startButtonSprite.setTexture(startButtonTexture);

    startButtonSprite.setScale(0.3f, 0.3f);

    sf::Vector2u bgSize = bgTexture.getSize();
    sf::Vector2u btnSize = startButtonTexture.getSize();
    float btnHeight = btnSize.y * 0.3f;
    float posY = (bgSize.y - btnHeight) / 2.0f + 40.0f; 

    startButtonSprite.setPosition(60, posY);

    fullscreenButtonTexture.loadFromFile("Assets/Fullscreen.png");
    fullscreenButtonSprite.setTexture(fullscreenButtonTexture);

    float scale = 64.0f / fullscreenButtonTexture.getSize().x;
    fullscreenButtonSprite.setScale(scale, scale);

    float spriteWidth = fullscreenButtonTexture.getSize().x * scale;
    float spriteHeight = fullscreenButtonTexture.getSize().y * scale;
    fullscreenButtonSprite.setPosition(width - spriteWidth - 400, height - spriteHeight - 400);
}

void Menu::update(const sf::Vector2f& mousePos) {
    if (startButtonSprite.getGlobalBounds().contains(mousePos)) {
        startButtonHovered = true;
        startButtonSprite.setTexture(startButtonHoverTexture);
    }
    else {
        startButtonHovered = false;
        startButtonSprite.setTexture(startButtonTexture);
    }

    if (fullscreenButtonSprite.getGlobalBounds().contains(mousePos)) {
        fullscreenButtonHovered = true;
        fullscreenButtonSprite.setColor(sf::Color(200, 200, 255));
    } else {
        fullscreenButtonHovered = false;
        fullscreenButtonSprite.setColor(sf::Color::White);
    }
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(startButtonSprite);
    window.draw(fullscreenButtonSprite);
}

bool Menu::isStartClicked(const sf::Vector2f& mousePos) const {
    return startButtonSprite.getGlobalBounds().contains(mousePos);
}

bool Menu::isFullscreenClicked(const sf::Vector2f& mousePos) const {
    return fullscreenButtonSprite.getGlobalBounds().contains(mousePos);
}

bool Menu::isFullscreenToggleRequested() const {
    return fullscreenToggleRequested;
}

void Menu::resetFullscreenToggleRequest() {
    fullscreenToggleRequested = false;
}