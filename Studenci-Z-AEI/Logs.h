#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <string>

class Logs {
public:
    Logs(const sf::Font& font, std::size_t maxLines = 10);

    void add(const std::string& message);
    void draw(sf::RenderWindow& window, float x, float y, float width) const;

private:
    std::deque<std::string> lines;
    std::size_t maxLines;
    const sf::Font& font;
};