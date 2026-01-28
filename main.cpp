#include <iostream>
#include <cstdlib>
#include <ctime>
#include "game.h"


int main() {
    //std::cout << "Detected OS: " << OS_NAME << std::endl;
    srand(time(NULL)); // Initialize random seed once
    Game game;
    game.play();

    return 0;
}
