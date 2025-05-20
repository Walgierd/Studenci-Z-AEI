#include <SFML/Graphics.hpp>
#include "Board.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Studenci z AEI");

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
        }

        window.clear();
        board.draw(window);
        window.display();
    }

    return 0;
}
