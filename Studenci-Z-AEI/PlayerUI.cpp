#include "PlayerUI.h"
#include <sstream>

void PlayerUI::draw(const Player& player, sf::RenderWindow& window, float x, float y) const {

    sf::Text playerIdText;
    playerIdText.setFont(font);
    playerIdText.setCharacterSize(32);
    playerIdText.setFillColor(sf::Color::White);
    playerIdText.setString("Gracz " + std::to_string(player.getId() + 1));
    playerIdText.setPosition(x, y);
    window.draw(playerIdText);


    float resourceY = y + 50.f;
    const auto& resources = player.getResources();
    for (const auto& [type, count] : resources) {
        sf::Text resourceText;
        resourceText.setFont(font);
        resourceText.setCharacterSize(28);
        resourceText.setFillColor(sf::Color::White);
        resourceText.setString(resourceName(type) + ": " + std::to_string(count)); 
        resourceText.setPosition(x, resourceY);
        window.draw(resourceText);
        resourceY += 36.f;
    }
}

void PlayerUI::drawAll(const std::vector<Player>& players, sf::RenderWindow& window) const {
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    float panelWidth = 180.f; 
    float margin = 12.f;      
    float spacing = 5.f;     
    float y = windowHeight - 200.f; 

 
    float totalWidth = players.size() * panelWidth + (players.size() - 1) * spacing;
    float startX = windowWidth - totalWidth - margin;

    for (size_t i = 0; i < players.size(); ++i) {
        float x = startX + i * (panelWidth + spacing);
        draw(players[i], window, x, y);
    }
}