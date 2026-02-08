#include "game.h"
#include "singleCharacter.h"
#include <iostream>
#include <cstdio>
#include <string>

Player *Game::getPlayer() const
{
    return player;
}

void Game::setPlayer(Player *newPlayer)
{
    player = newPlayer;
}

Shop *Game::getShop() const
{
    return shop;
}

Game::Game()
{
    player = new Player();
    itemGenerator = new ItemGenerator();
    shop = new Shop();
    // Use random generation instead of fixed items
    generateRandomInventory();
}

void Game::initializeSampleItems()
{
    // Original fixed item initialization (kept for reference)
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

void Game::generateRandomInventory()
{
    // Generate between 5 and 20 random items
    std::vector<Item*> randomItems = itemGenerator->generateRandomItems(5, 20);

    // Add all generated items to inventory at random positions
    for(Item* item : randomItems) {
        // Try to add at random position
        if(!player->getInventory()->addItemAtRandomPosition(item)) {
            delete item; // Clean up if inventory is full
            break;
        }
    }

    std::cout << "Generated " << randomItems.size() << " random items!" << std::endl;
}

Game::~Game()
{
    delete player;
    delete itemGenerator;
    delete shop;
}

void Game::playShop()
{
    int userInput = 0;
    bool searchMode = false;
    std::string searchInput = "";

    std::cout << "\n=== Welcome to the Shop! ===" << std::endl;
    std::cout << "Press any key to continue..." << std::endl;
    getSingleChar();

    while(userInput != 101) { // 'e' to exit
        // Clear screen
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        if(searchMode && !searchInput.empty()) {
            shop->setSearchQuery(searchInput);
            shop->displayShopWithSearch();
        } else {
            shop->clearSearch();
            shop->displayShop();
        }

        userInput = int(getSingleChar());

        switch(userInput) {
            case 119: // 'w' - Select previous
                shop->selectPrevious();
                break;

            case 115: // 's' - Select next
                shop->selectNext();
                break;

            case 98: // 'b' - Buy item
                {
                    int selectedIdx = shop->getSelectedIndex();
                    Item* selectedItem = shop->getSelectedItem();

                    if(selectedItem != nullptr) {
                        if(shop->getPlayerGold() < selectedItem->getPrice()) {
                            std::cout << "\nNot enough gold! Need " << selectedItem->getPrice() << "g, have " << shop->getPlayerGold() << "g" << std::endl;
                            std::cout << "Press any key to continue..." << std::endl;
                            getSingleChar();
                        } else if(shop->buyItem(selectedIdx, player->getInventory())) {
                            std::cout << "\nPurchased " << selectedItem->getName() << " for " << selectedItem->getPrice() << "g!" << std::endl;
                            std::cout << "Gold remaining: " << shop->getPlayerGold() << "g" << std::endl;
                            std::cout << "Press any key to continue..." << std::endl;
                            getSingleChar();
                        } else {
                            std::cout << "\nInventory is full! Cannot buy item." << std::endl;
                            std::cout << "Press any key to continue..." << std::endl;
                            getSingleChar();
                        }
                    }
                }
                break;

            case 102: // 'f' - Search
                {
                    printf("\033[2J");
                    printf("\033[H");
                    fflush(stdout);

                    std::cout << "Enter search query (item name): ";
                    std::cin >> searchInput;
                    searchMode = true;
                }
                break;

            case 99: // 'c' - Clear search
                searchInput = "";
                searchMode = false;
                shop->clearSearch();
                break;

            case 114: // 'r' - Restock shop
                shop->restockShop();
                std::cout << "\nShop restocked with new items!" << std::endl;
                std::cout << "Press any key to continue..." << std::endl;
                getSingleChar();
                break;

            case 118: // 'v' - Sell items
                sellItemsMenu();
                break;
        }
    }
}

void Game::sellItemsMenu()
{
    int userInput = 0;

    while(userInput != 101) { // 'e' to exit
        // Clear screen
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        std::cout << "=== SELL ITEMS ===" << std::endl;
        std::cout << "Your Gold: " << shop->getPlayerGold() << "g" << std::endl;
        std::cout << std::endl;

        player->getInventory()->display();

        std::cout << "\nControls:" << std::endl;
        std::cout << "  W/A/S/D - Navigate | V - Sell selected item | E - Exit" << std::endl;

        userInput = int(getSingleChar());

        switch(userInput) {
            case 97: // a
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() - 1);
                break;
            case 100: // d
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() + 1);
                break;
            case 119: // w
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() - 1);
                break;
            case 115: // s
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() + 1);
                break;
            case 118: // 'v' - Sell item
                {
                    Item* itemToSell = player->getInventory()->getCurrentItem();
                    if(itemToSell != nullptr) {
                        int sellPrice = itemToSell->getPrice() / 2;
                        std::string itemName = itemToSell->getName();

                        // Remove from player inventory
                        int currentRow = player->getInventory()->getCurrentRow();
                        int currentCol = player->getInventory()->getCurrentCol();
                        player->getInventory()->getItems()[currentRow][currentCol] = nullptr;

                        // Sell to shop
                        shop->sellItem(itemToSell);

                        std::cout << "\nSold " << itemName << " for " << sellPrice << "g!" << std::endl;
                        std::cout << "Gold: " << shop->getPlayerGold() << "g" << std::endl;
                        std::cout << "Press any key to continue..." << std::endl;
                        getSingleChar();
                    } else {
                        std::cout << "\nNo item selected!" << std::endl;
                        std::cout << "Press any key to continue..." << std::endl;
                        getSingleChar();
                    }
                }
                break;
        }
    }
}

void Game::play()
{
    std::cout << "=== Inventory System ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  W/A/S/D - Move cursor" << std::endl;
    std::cout << "  I - Display item information" << std::endl;
    std::cout << "  1 - Filter by Common items" << std::endl;
    std::cout << "  2 - Filter by Magic items" << std::endl;
    std::cout << "  3 - Filter by Rare items" << std::endl;
    std::cout << "  0 - Clear filter (show all)" << std::endl;
    std::cout << "  P - Open Shop" << std::endl;
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

        std::cout << "Gold: " << shop->getPlayerGold() << "g" << std::endl;
        std::cout << std::endl;

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
            case 49: // '1' key - Filter by Common
                player->getInventory()->setFilter(common);
                showItemInfo = false;
                break;
            case 50: // '2' key - Filter by Magic
                player->getInventory()->setFilter(magic);
                showItemInfo = false;
                break;
            case 51: // '3' key - Filter by Rare
                player->getInventory()->setFilter(rare);
                showItemInfo = false;
                break;
            case 48: // '0' key - Clear filter
                player->getInventory()->clearFilter();
                showItemInfo = false;
                break;
            case 112: // 'p' key - Open shop
                playShop();
                showItemInfo = false;
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
    //0 : 48
    //1 : 49
    //2 : 50
    //3 : 51
    //p : 112
}