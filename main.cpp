#include <iostream>
#include <cstdlib>
#include <ctime>
#include "game.h"


int main() {
    srand(time(NULL)); // Initialize random seed once
    Game game;
    game.play();

    return 0;
}