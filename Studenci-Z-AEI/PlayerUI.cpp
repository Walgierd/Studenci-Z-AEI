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
    float playerWidth = windowWidth / players.size();
    float y = windowHeight - 180.f; // Odległość od dołu

    for (size_t i = 0; i < players.size(); ++i) {
        float x = i * playerWidth + 20.f; // Odstęp od lewej krawędzi
        draw(players[i], window, x, y);
    }
}