#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include "Player.h"
#include "Resource.h"
#include "SimpleButton.h"
#include "UIButton.h"

struct TradeUI {
    bool exchangeMode = false;
    int exchangeTargetPlayer = -1;
    std::map<ResourceType, int> exchangeGive;
    std::map<ResourceType, int> exchangeGet;
    std::vector<std::unique_ptr<UIButton>> exchangeButtons;
    std::vector<std::unique_ptr<UIButton>> exchangePlayerButtons;
    std::unique_ptr<UIButton> exchangeAcceptButton;

    void startTrade(sf::Font& font, std::vector<Player>& players, int currentPlayer);
    void handleClick(const sf::Vector2f& mousePos, std::vector<Player>& players, int currentPlayer);
    void draw(sf::RenderWindow& window);
    void reset();
};
