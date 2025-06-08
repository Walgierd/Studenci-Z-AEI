#include "SimpleButton.h"

SimpleButton::SimpleButton(const sf::Font& font, const std::string& text, sf::Vector2f pos, std::function<void()> callback)
    : callback(callback)
{
    shape.setSize(sf::Vector2f(220, 48));
    shape.setPosition(pos);
    shape.setFillColor(sf::Color(40, 40, 80));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(24);
    label.setFillColor(sf::Color::White);
    label.setPosition(pos.x + 16, pos.y + 8);
}

void SimpleButton::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}

bool SimpleButton::isClicked(const sf::Vector2f& mouse) const {
    return shape.getGlobalBounds().contains(mouse);
}

void SimpleButton::onClick() {
    if (callback) callback();
}

const sf::Text& SimpleButton::getLabel() const {
    return label;
}

void SimpleButton::setLabelText(const std::string& text) {
    label.setString(text);
}

void SimpleButton::setCallback(std::function<void()> newCallback) {
    callback = std::move(newCallback);
}