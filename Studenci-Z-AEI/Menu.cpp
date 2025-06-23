#include "Menu.h"
#include <regex>
#include <ranges>

Menu::Menu(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    if (std::filesystem::exists("Assets/Menu2.png")) bgTexture.loadFromFile("Assets/Menu2.png");
    background.setTexture(bgTexture);

    if (std::filesystem::exists("Fonts/pixel.ttf")) font.loadFromFile("Fonts/pixel.ttf");

    startButtonTexture.loadFromFile("Assets/Start.png");
    startButtonHoverTexture.loadFromFile("Assets/Start-on.png");
    startButtonSprite.setTexture(startButtonTexture);

  
    startButtonSprite.setScale(0.22f, 0.22f);

    sf::Vector2u bgSize = bgTexture.getSize();
    sf::Vector2u btnSize = startButtonTexture.getSize();
    float btnHeight = btnSize.y * 0.22f;
    float posY = (bgSize.y - btnHeight) / 2.0f + 40.0f;

    startButtonSprite.setPosition(60, posY);

    fullscreenButtonTexture.loadFromFile("Assets/Fullscreen.png");
    fullscreenButtonSprite.setTexture(fullscreenButtonTexture);

    float scale = 64.0f / fullscreenButtonTexture.getSize().x;
    fullscreenButtonSprite.setScale(scale, scale);

    float spriteWidth = fullscreenButtonTexture.getSize().x * scale;
    float spriteHeight = fullscreenButtonTexture.getSize().y * scale;
    fullscreenButtonSprite.setPosition(width - spriteWidth - 400, height - spriteHeight - 400);

    float btnWidth = 120;
    float startX = width / 2.f - 1.5f * btnWidth;
    float y = height / 2.f + 100;
    for (int i = 2; i <= 4; ++i) {
        sf::RectangleShape btn({ btnWidth, btnHeight });
        btn.setPosition(startX + (i - 2) * (btnWidth + 20), y);
        btn.setFillColor(i == selectedPlayerCount ? sf::Color::Green : sf::Color(100, 100, 100));
        playerCountButtons.push_back(btn);

        sf::Text txt;

        static sf::Font font;
        static bool fontLoaded = false;
        if (!fontLoaded) {
            font.loadFromFile("Fonts/arial.ttf");
            fontLoaded = true;
        }
        txt.setFont(font);
        txt.setString(std::to_string(i) + " graczy");
        txt.setCharacterSize(28);
        txt.setFillColor(sf::Color::White);
        txt.setPosition(btn.getPosition().x + 10, btn.getPosition().y + 10);
        playerCountTexts.push_back(txt);
    }

    
    nicknameButton.setSize({ 120, 48 });//rozmiarówka do zmian
    nicknameButton.setPosition(
        startButtonSprite.getPosition().x + 40,
        startButtonSprite.getPosition().y + startButtonSprite.getGlobalBounds().height 
    );
    nicknameButton.setFillColor(sf::Color(80, 80, 180));
    nicknameButtonText.setFont(font);
    nicknameButtonText.setString("Nicki");
    nicknameButtonText.setCharacterSize(28);
    nicknameButtonText.setFillColor(sf::Color::White);
    nicknameButtonText.setPosition(nicknameButton.getPosition().x + 18, nicknameButton.getPosition().y + 8);

    nicknameEditMode = false;
    nicknameEditPlayer = -1;
    nicknameInput = "";

    playerNicknames.resize(4);
    for (int i = 0; i < 4; ++i)
        playerNicknames[i] = "Gracz " + std::to_string(i + 1);
}

void Menu::update(const sf::Vector2f& mousePos) {
    if (startButtonSprite.getGlobalBounds().contains(mousePos)) {
        startButtonHovered = true;
        startButtonSprite.setTexture(startButtonHoverTexture);
    }
    else {
        startButtonHovered = false;
        startButtonSprite.setTexture(startButtonTexture);
    }

    if (fullscreenButtonSprite.getGlobalBounds().contains(mousePos)) {
        fullscreenButtonHovered = true;
        fullscreenButtonSprite.setColor(sf::Color(200, 200, 255));
    }
    else {
        fullscreenButtonHovered = false;
        fullscreenButtonSprite.setColor(sf::Color::White);
    }

    for (size_t i = 0; i < playerCountButtons.size(); ++i) {
        if (playerCountButtons[i].getGlobalBounds().contains(mousePos)) {
            for (auto& btn : playerCountButtons) btn.setFillColor(sf::Color(100, 100, 100));
            playerCountButtons[i].setFillColor(sf::Color::Green);
            selectedPlayerCount = static_cast<int>(i) + 2;
        }
    }

  
    static bool nicknameBtnPressed = false;
    if (nicknameButton.getGlobalBounds().contains(mousePos)) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (!nicknameBtnPressed) {
                nicknameEditMode = !nicknameEditMode;
                nicknameEditPlayer = -1;
                nicknameInput = "";
                nicknameBtnPressed = true;
            }
        } else {
            nicknameBtnPressed = false;
        }
    } else {
        nicknameBtnPressed = false;
    }

    
    static int lastClickedPlayer = -1;
    if (nicknameEditMode) {
        for (int i = 0; i < selectedPlayerCount; ++i) {
            sf::FloatRect nickRect(nicknameButton.getPosition().x + nicknameButton.getSize().x + 20, nicknameButton.getPosition().y + i * 52, 260, 40);
            if (nickRect.contains(mousePos)) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (lastClickedPlayer != i) {
                        nicknameEditPlayer = i;
                        std::string current = playerNicknames[i];//domyślny player X jeśli nic nie wpisane
                        size_t colon = current.find(':');
                        if (colon != std::string::npos)
                            nicknameInput = current.substr(colon + 2);
                        else
                            nicknameInput = "";
                        lastClickedPlayer = i;
                    }
                } else if (lastClickedPlayer == i) {
                    lastClickedPlayer = -1;
                }
            }
        }
    }
}

void Menu::handleTextEntered(sf::Uint32 unicode) {
    if (!nicknameEditMode || nicknameEditPlayer == -1) return;
    if (unicode == 8) { // Backspace
        if (!nicknameInput.empty()) nicknameInput.pop_back();
    } else if (unicode == 13) { // Enter
        std::regex nickRegex("^[A-Za-z0-9_]{1,7}$");
        if (!nicknameInput.empty() && std::regex_match(nicknameInput, nickRegex))
            playerNicknames[nicknameEditPlayer] = "Gracz " + std::to_string(nicknameEditPlayer + 1) + ": " + nicknameInput;
        else
            playerNicknames[nicknameEditPlayer] = "Gracz " + std::to_string(nicknameEditPlayer + 1);
        nicknameEditPlayer = -1;
        nicknameInput = "";
    } else if (nicknameInput.size() < 7 && unicode >= 32 && unicode < 128) {
        nicknameInput += static_cast<char>(unicode);
    }
}

void Menu::draw(sf::RenderWindow& window) const {
    window.draw(background);

    window.draw(startButtonSprite);

    window.draw(nicknameButton);
    window.draw(nicknameButtonText);

    // Nicki na prawo od przycisku "Nicki" - pojawiają się dopiero po kliknięciu przycisku
    if (nicknameEditMode) {
        for (int i = 0; i < selectedPlayerCount; ++i) {
            sf::RectangleShape nickBg(sf::Vector2f(260, 40));
            nickBg.setPosition(nicknameButton.getPosition().x + nicknameButton.getSize().x + 20, nicknameButton.getPosition().y + i * 52);
            nickBg.setFillColor((nicknameEditPlayer == i) ? sf::Color(60, 60, 120, 180) : sf::Color(40, 40, 40, 120));
            window.draw(nickBg);

            sf::Text nickText;
            nickText.setFont(font);
            if (nicknameEditPlayer == i) {
                nickText.setString("Gracz " + std::to_string(i + 1) + ": " + nicknameInput + "|");
                nickText.setFillColor(sf::Color::Cyan);
            } else {
                nickText.setString(playerNicknames[i]);
                nickText.setFillColor(sf::Color::White);
            }
            nickText.setCharacterSize(28);
            nickText.setPosition(nicknameButton.getPosition().x + nicknameButton.getSize().x + 30, nicknameButton.getPosition().y + i * 52 + 4);
            window.draw(nickText);
        }
    }

    window.draw(fullscreenButtonSprite);
    for (const auto& btn : playerCountButtons) {
        window.draw(btn);
    }
    for (const auto& txt : playerCountTexts) {
        window.draw(txt);
    }
}

bool Menu::isStartClicked(const sf::Vector2f& mousePos) const {
    return startButtonSprite.getGlobalBounds().contains(mousePos);
}

bool Menu::isFullscreenClicked(const sf::Vector2f& mousePos) const {
    return fullscreenButtonSprite.getGlobalBounds().contains(mousePos);
}

bool Menu::isFullscreenToggleRequested() const {
    return fullscreenToggleRequested;
}

void Menu::resetFullscreenToggleRequest() {
    fullscreenToggleRequested = false;
}

void Menu::setFullscreenToggleRequested(bool value) {
    fullscreenToggleRequested = value;
}

int Menu::getSelectedPlayerCount() const {
    return selectedPlayerCount;
}

void Menu::handleFullscreenToggle(sf::RenderWindow& window, unsigned int& currentStyle) {
    if (currentStyle == sf::Style::Default) {
        currentStyle = sf::Style::Fullscreen;
        window.create(sf::VideoMode(width, height), "Studenci z AEI", currentStyle);
    } else {
        currentStyle = sf::Style::Default;
        window.create(sf::VideoMode(width, height), "Studenci z AEI", currentStyle);
    }
}

void Menu::setPlayerNicknames(const std::vector<std::string>& nicks) {
    for (size_t i = 0; i < std::min(nicks.size(), playerNicknames.size()); ++i) {
        std::regex nickRegex("^[A-Za-z0-9_]{1,7}$");
        if (!nicks[i].empty() && std::regex_match(nicks[i], nickRegex))
            playerNicknames[i] = "Gracz " + std::to_string(i + 1) + ": " + nicks[i];
        else
            playerNicknames[i] = "Gracz " + std::to_string(i + 1);
    }
}

const std::vector<std::string>& Menu::getPlayerNicknames() const {
    return playerNicknames;
}