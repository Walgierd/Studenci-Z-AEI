#include "Studenci-Z-AEI.h"


int main()
{
    try {
        Game game;
        game.run();
    } catch (const std::exception& ex) {
        return 1;
    }
    return 0;
}
