#include "PlayerUI.h"
#include "Game.h"

void PlayerUI::draw(const Player& player, sf::RenderWindow& window, float x, float y) const {
    sf::Text playerIdText;
    playerIdText.setFont(font);
    playerIdText.setCharacterSize(32);
    playerIdText.setFillColor(sf::Color::White);
    playerIdText.setString(player.getNickname());
    playerIdText.setPosition(x, y);
    window.draw(playerIdText);

    float resourceY = y + 36.f;

    // Punkty jako pierwszy wiersz tabeli
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(28);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setString("Punkty: " + std::to_string(score->getScore(player.getId())));
    scoreText.setPosition(x, resourceY);
    window.draw(scoreText);
    resourceY += 32.f;

    // Zasoby
    const auto& resources = player.getResources();
    for (const auto& [type, count] : resources) {
        sf::Text resourceText;
        resourceText.setFont(font);
        resourceText.setCharacterSize(28);
        resourceText.setFillColor(sf::Color::White);
        resourceText.setString(::resourceName(type) + ": " + std::to_string(count)); 
        resourceText.setPosition(x, resourceY);
        window.draw(resourceText);
        resourceY += 32.f;
    }
}

void PlayerUI::drawAll(const std::vector<Player>& players, sf::RenderWindow& window) const {
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    float panelWidth = 180.f; 
    float margin = 12.f;      
    float spacing = 5.f;     
    float y = windowHeight - 200.f; 


    int visiblePlayers = 0;
    for (const auto& player : players) {
        if (player.getId() != -1) ++visiblePlayers;
    }
    float totalWidth = visiblePlayers * panelWidth + (visiblePlayers - 1) * spacing;
    float startX = windowWidth - totalWidth - margin;

    int drawIndex = 0;
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].getId() == -1) continue; 
        float x = startX + drawIndex * (panelWidth + spacing);
        draw(players[i], window, x, y);
        ++drawIndex;
    }
}