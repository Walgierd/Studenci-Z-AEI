#pragma once
#include "UIButton.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

// SimpleButton implementation
class SimpleButton : public UIButton {
public:
    SimpleButton(const sf::Font& font, const std::string& text, sf::Vector2f pos, std::function<void()> callback);

    void draw(sf::RenderWindow& window) override;
    bool isClicked(const sf::Vector2f& mouse) const override;
    void onClick() override;

    const sf::Text& getLabel() const;
    void setLabelText(const std::string& text);
    void setCallback(std::function<void()> newCallback);

private:
    sf::RectangleShape shape;
    sf::Text label;
    std::function<void()> callback;
};