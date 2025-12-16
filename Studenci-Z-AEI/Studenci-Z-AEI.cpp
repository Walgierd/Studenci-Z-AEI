#include "Studenci-Z-AEI.h"


int main()
{
    try {
        Game game;
        game.run();
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl; 
        return 1;
    }
    return 0;
}
