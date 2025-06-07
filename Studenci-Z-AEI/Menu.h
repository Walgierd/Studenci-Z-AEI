#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

class UIButton {
public:
    virtual ~UIButton() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual bool isClicked(const sf::Vector2f& mouse) const = 0;
    virtual void onClick() = 0;
};

class SimpleButton : public UIButton {
public:
    SimpleButton(const sf::Font& font, const std::string& text, sf::Vector2f pos, std::function<void()> callback)
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

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);
        window.draw(label);
    }

    bool isClicked(const sf::Vector2f& mouse) const override {
        return shape.getGlobalBounds().contains(mouse);
    }

    void onClick() override {
        if (callback) callback();
    }

    const sf::Text& getLabel() const {
        return label;
    }

    // Add this setter method to modify the label's text
    void setLabelText(const std::string& text) {
        label.setString(text);
    }

    // Add this setter method to allow modifying the callback
    void setCallback(std::function<void()> newCallback) {
        callback = std::move(newCallback);
    }

private:
    sf::RectangleShape shape;
    sf::Text label;
    std::function<void()> callback;
};

class Menu {
public:
    Menu(float width, float height);
    void draw(sf::RenderWindow& window);
    bool isStartClicked(const sf::Vector2f& mousePos) const;
    void update(const sf::Vector2f& mousePos);
    bool isFullscreenClicked(const sf::Vector2f& mousePos) const;
    bool isFullscreenToggleRequested() const;
    void resetFullscreenToggleRequest();
    int getSelectedPlayerCount() const;
    void setFullscreenToggleRequested(bool value); // Add this method declaration

private:
    sf::Sprite background;
    sf::Texture bgTexture;
    sf::Font font;
    sf::Texture startButtonTexture;
    sf::Texture startButtonHoverTexture;
    sf::Sprite startButtonSprite;
    sf::RectangleShape startButtonBox;
    sf::Texture fullscreenButtonTexture;
    sf::Sprite fullscreenButtonSprite;
    bool startButtonHovered = false;
    bool fullscreenButtonHovered = false;
    bool fullscreenToggleRequested = false;
    std::vector<sf::RectangleShape> playerCountButtons;
    std::vector<sf::Text> playerCountTexts;
    int selectedPlayerCount = 2;
};
