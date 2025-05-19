#include <SFML/Graphics.hpp>
#include <optional>
#include "Board.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Studenci z AEI"); //Pozmieniaæ proporcja, gracz mo¿e zmieniaæ rozmiar okna.

    
    sf::Vector2f windowCenter(1920.f / 2.f, 1080.f / 2.f);

   
    float hexSize = 80.f; 

    Board board(hexSize, windowCenter);


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
