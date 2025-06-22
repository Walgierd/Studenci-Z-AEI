#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Resource.h"
#include "Score.h"
#include <sstream>

class PlayerUI {
public:
    PlayerUI(const sf::Font& font, const Score* score) : font(font), score(score) {}

    void draw(const Player& player, sf::RenderWindow& window, float x, float y) const;
    void drawAll(const std::vector<Player>& players, sf::RenderWindow& window) const;

private:
    const sf::Font& font;
    const Score* score;
};