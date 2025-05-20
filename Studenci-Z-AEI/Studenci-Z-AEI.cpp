#include "Studenci-Z-AEI.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Studenci z AEI");

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
            }
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
