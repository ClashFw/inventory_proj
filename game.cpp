#include "game.h"
#include "singleCharacter.h"
#include <iostream>
#include <cstdio>
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
    initializeSampleItems();
}

void Game::initializeSampleItems()
{
    // Add some sample items to the inventory
    player->getInventory()->addItem(new Item("Sword", 50));
    player->getInventory()->addItem(new Item("Shield", 40));
    player->getInventory()->addItem(new Item("Potion", 10));
    player->getInventory()->addItem(new Item("Helmet", 30));
    player->getInventory()->addItem(new Item("Boots", 25));
    player->getInventory()->addItem(new Item("Ring", 100));
    player->getInventory()->addItem(new Item("Amulet", 75));
    player->getInventory()->addItem(new Item("Bow", 60));
    player->getInventory()->addItem(new Item("Arrow", 5));
    player->getInventory()->addItem(new Item("Armor", 80));
}

Game::~Game()
{
    delete player;
}

void Game::play()
{
    std::cout << "=== Inventory System ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  W/A/S/D - Move cursor" << std::endl;
    std::cout << "  I - Display item information" << std::endl;
    std::cout << "  Backspace - Exit" << std::endl;
    std::cout << "\nPress any key to start..." << std::endl;
    getSingleChar();

    int userInput = 0;
    bool showItemInfo = false;

    while(userInput != 127) {
        // Use printf with ANSI escape codes for reliable clearing
        printf("\033[2J");    // Clear entire screen
        printf("\033[H");     // Move cursor to home position (0,0)
        fflush(stdout);       // Force output

        if(showItemInfo) {
            Item* currentItem = player->getInventory()->getCurrentItem();
            if(currentItem != nullptr) {
                player->getInventory()->displayWithItemInfo(currentItem);
            } else {
                player->getInventory()->displayWithEmptyInfo();
            }
        } else {
            player->getInventory()->display();
        }

        userInput = int(getSingleChar()); // Get single key press
        switch(userInput) {
            case 97: // a
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() - 1);
                showItemInfo = false;
                break;
            case 100: // d
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() + 1);
                showItemInfo = false;
                break;
            case 119: // w
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() - 1);
                showItemInfo = false;
                break;
            case 115: // s
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() + 1);
                showItemInfo = false;
                break;
            case 105: // 'i' key
                showItemInfo = !showItemInfo; // Toggle item info display
                break;
        }
    }

    //enter : 10
    //backspace : 127
    //w : 119
    //a : 97
    //s : 115
    //d : 100
    //i : 105

}