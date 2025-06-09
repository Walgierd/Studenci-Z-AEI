#include "Menu.h"

Menu::Menu(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

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

    float btnWidth = 120;
    float startX = width / 2.f - 1.5f * btnWidth;
    float y = height / 2.f + 100;
    for (int i = 2; i <= 4; ++i) {
        sf::RectangleShape btn({ btnWidth, btnHeight });
        btn.setPosition(startX + (i - 2) * (btnWidth + 20), y);
        btn.setFillColor(i == selectedPlayerCount ? sf::Color::Green : sf::Color(100, 100, 100));
        playerCountButtons.push_back(btn);

        sf::Text txt;

        static sf::Font font;
        static bool fontLoaded = false;
        if (!fontLoaded) {
            font.loadFromFile("Fonts/arial.ttf");
            fontLoaded = true;
        }
        txt.setFont(font);
        txt.setString(std::to_string(i) + " graczy");
        txt.setCharacterSize(28);
        txt.setFillColor(sf::Color::White);
        txt.setPosition(btn.getPosition().x + 10, btn.getPosition().y + 10);
        playerCountTexts.push_back(txt);
    }
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
    }
    else {
        fullscreenButtonHovered = false;
        fullscreenButtonSprite.setColor(sf::Color::White);
    }

    for (size_t i = 0; i < playerCountButtons.size(); ++i) {
        if (playerCountButtons[i].getGlobalBounds().contains(mousePos)) {
            for (auto& btn : playerCountButtons) btn.setFillColor(sf::Color(100, 100, 100));
            playerCountButtons[i].setFillColor(sf::Color::Green);
            selectedPlayerCount = static_cast<int>(i) + 2;
        }
    }
}

void Menu::draw(sf::RenderWindow& window) const {
    window.draw(background);
    window.draw(startButtonSprite);
    window.draw(fullscreenButtonSprite);
    for (const auto& btn : playerCountButtons) {
        window.draw(btn);
    }
    for (const auto& txt : playerCountTexts) {
        window.draw(txt);
    }
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

void Menu::setFullscreenToggleRequested(bool value) {
    fullscreenToggleRequested = value;
}

int Menu::getSelectedPlayerCount() const {
    return selectedPlayerCount;
}

void Menu::handleFullscreenToggle(sf::RenderWindow& window, unsigned int& currentStyle) {
    if (currentStyle == sf::Style::Default) {
        currentStyle = sf::Style::Fullscreen;
        window.create(sf::VideoMode(width, height), "Studenci z AEI", currentStyle);
    } else {
        currentStyle = sf::Style::Default;
        window.create(sf::VideoMode(width, height), "Studenci z AEI", currentStyle);
    }
}