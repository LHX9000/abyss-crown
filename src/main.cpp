#include <iostream>

#include "Game.h"

int main() {
    Game game;
    if (!game.initialize()) {
        std::cerr << "Failed to start ABYSS CROWN.\n";
        return 1;
    }

    game.run();
    return 0;
}
