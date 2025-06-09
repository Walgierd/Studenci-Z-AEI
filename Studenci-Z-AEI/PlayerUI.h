#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Resource.h"

class PlayerUI {
public:
    PlayerUI(const sf::Font& font) : font(font) {}

    void draw(const Player& player, sf::RenderWindow& window, float x, float y) const;
    void drawAll(const std::vector<Player>& players, sf::RenderWindow& window) const;

private:
    const sf::Font& font;

    std::string resourceName(ResourceType type) const {
        switch (type) {
            case ResourceType::Kawa: return "Kawa";
            case ResourceType::Piwo: return "Piwo";
            case ResourceType::Notatki: return "Notatki";
            case ResourceType::Pizza: return "Pizza";
            case ResourceType::Kabel: return "Kabel";
            default: return "Brak";
        }
    }
};