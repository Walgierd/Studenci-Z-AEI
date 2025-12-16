
#include "Logs.h"

Logs::Logs(const sf::Font& font, std::size_t maxLines)
    : font(font), maxLines(maxLines) {}

void Logs::add(const std::string& message) {
    if (lines.size() >= maxLines)
        lines.pop_front();
    lines.push_back(message);
}

void Logs::draw(sf::RenderWindow& window, float x, float y, float width) const {
    float lineHeight = 28.f;
    float currentY = y;
    for (const auto& line : lines) {
        sf::Text text(line, font, 22);
        text.setFillColor(sf::Color::White);
        text.setPosition(x, currentY);
        text.setLineSpacing(1.1f);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(2.f);
        window.draw(text);
        currentY += lineHeight;
    }
}