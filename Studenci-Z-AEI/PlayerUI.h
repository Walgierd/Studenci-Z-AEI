#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

class PlayerUI {
public:
    PlayerUI(const sf::Font& font) : font(font) {}

    void draw(const Player& player, sf::RenderWindow& window, float x, float y) {
        int offsetY = 0;
        for (const auto& [type, count] : player.getResources()) {
            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(22);
            text.setFillColor(sf::Color::White);
            text.setPosition(x, y + offsetY);
            text.setString(resourceName(type) + ": " + std::to_string(count));
            window.draw(text);
            offsetY += 30;
        }
    }

private:
    const sf::Font& font;

    std::string resourceName(ResourceType type) const {
        switch (type) {
            case ResourceType::Kawa: return "Kawa";
            case ResourceType::Energia: return "Energia";
            case ResourceType::Notatki: return "Notatki";
            default: return "Brak";
        }
    }
};