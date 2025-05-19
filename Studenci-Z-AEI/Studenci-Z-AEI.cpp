#include <SFML/Graphics.hpp>
#include <optional>
#include "Board.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Studenci z AEI");

    Board board(40.f, { 400.f, 300.f });

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        board.draw(window);
        window.display();
    }

    return 0;
}
