#include "game.h"
#include "singleCharacter.h"
Player *Game::getPlayer() const
{
    return player;
}

void Game::setPlayer(Player *newPlayer)
{
    player = newPlayer;
}

Game::Game()
{
    player = new Player();
}

Game::~Game()
{
    delete player;
}

void Game::play()
{
    int userInput = 0;
    while(userInput != 127) {
        player->getInventory()->display();
        userInput = int(getSingleChar()); // Get single key press
        switch(userInput) {
            case 97:
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() - 1);
                break;
            case 100:
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() + 1);
                break;
            case 119:
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() - 1);
                break;
            case 115:
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() + 1);
                break;
            default:
                return;
        }

        system(CLEAR);
    }

    //enter : 10
    //backspace : 127
    //w : 119
    //a : 97
    //s : 115
    //d : 100

}
