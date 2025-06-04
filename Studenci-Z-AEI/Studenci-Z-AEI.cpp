#include "Studenci-Z-AEI.h"
#include "Menu.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Studenci z AEI", sf::Style::Default);
    unsigned int currentStyle = sf::Style::Default; 

    Menu menu(1920, 1080);
    bool inMenu = true;

    sf::Vector2f windowCenter(1920.f / 2.f, 1080.f / 2.f);
    float hexSize = 80.f;
    Board board(hexSize, windowCenter);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (inMenu && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
                if (menu.isStartClicked(mousePos)) {
                    inMenu = false;
                }
                if (menu.isFullscreenClicked(mousePos)) {
                    menu.fullscreenToggleRequested = true;
                }
            }
        }

        if (menu.isFullscreenToggleRequested()) {
            bool fullscreen = (currentStyle != sf::Style::Fullscreen);
            window.close();
            if (fullscreen) {
                window.create(sf::VideoMode::getDesktopMode(), "Studenci z AEI", sf::Style::Fullscreen);
                currentStyle = sf::Style::Fullscreen; 
            } else {
                window.create(sf::VideoMode(1920, 1080), "Studenci z AEI", sf::Style::Default);
                currentStyle = sf::Style::Default;
            }
            menu.resetFullscreenToggleRequest();
        }

        window.clear();
        if (inMenu) {
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
            menu.update(mousePos); 
            menu.draw(window);
        }
        else {
            board.draw(window);
        }
        window.display();
    }

    return 0;
}
