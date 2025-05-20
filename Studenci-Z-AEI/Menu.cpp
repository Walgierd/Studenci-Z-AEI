#include "Menu.h"

Menu::Menu(float width, float height) {
    bgTexture.loadFromFile("Assets/Menu.png");
    background.setTexture(bgTexture);

    font.loadFromFile("Fonts/pixel.ttf");

    
    title.setFont(font);
    title.setString("STUDENCI\n Z AEI");
    title.setCharacterSize(120);
    title.setFillColor(sf::Color(255, 237, 149)); // #ffed95
    title.setOutlineColor(sf::Color(198, 57, 224)); // #c639e0
    title.setOutlineThickness(6);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left, titleBounds.top + titleBounds.height / 2);
    title.setPosition(60, height / 2 - 100);

    
    startButton.setFont(font);
    startButton.setString("GRAJ");
    startButton.setCharacterSize(60);
    startButton.setFillColor(sf::Color(107, 61, 160)); // #6b3da0
    startButton.setOutlineColor(sf::Color(198, 57, 224)); // #c639e0
    startButton.setOutlineThickness(4);
    sf::FloatRect btnBounds = startButton.getLocalBounds();
    startButton.setOrigin(btnBounds.left, btnBounds.top + btnBounds.height / 2);
    startButton.setPosition(60, height / 2 + 120);

   


    startButtonBox.setSize({ btnBounds.width + 60, btnBounds.height + 40 });
    startButtonBox.setOrigin(startButtonBox.getSize().x / 2, startButtonBox.getSize().y / 2);
    startButtonBox.setPosition(60 + startButtonBox.getSize().x / 2, height / 2 + 120);

    startButton.setOrigin(btnBounds.left + btnBounds.width / 2, btnBounds.top + btnBounds.height / 2);
    startButton.setPosition(startButtonBox.getPosition());

    startButtonBox.setFillColor(sf::Color(85, 97, 211)); // #5561d3
    startButtonBox.setOutlineColor(sf::Color(142, 104, 230)); // #8e68e6
    startButtonBox.setOutlineThickness(4);
}

void Menu::update(const sf::Vector2f& mousePos) {
    if (startButtonBox.getGlobalBounds().contains(mousePos)) {
        startButtonHovered = true;
        startButtonBox.setFillColor(sf::Color(85, 97, 211, 255)); // #5561d3
        startButton.setFillColor(sf::Color(255, 237, 149)); // #ffed95
        startButtonBox.setOutlineColor(sf::Color(198, 57, 224)); // #c639e0
    } else {
        startButtonHovered = false;
        startButtonBox.setFillColor(sf::Color(85, 97, 211)); // #5561d3
        startButton.setFillColor(sf::Color(255, 237, 149)); // #ffed95
        startButtonBox.setOutlineColor(sf::Color(142, 104, 230)); // #8e68e6
    }
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(title);
    window.draw(startButtonBox);
    window.draw(startButton);
}

bool Menu::isStartClicked(const sf::Vector2f& mousePos) const {
    return startButtonBox.getGlobalBounds().contains(mousePos);
}
