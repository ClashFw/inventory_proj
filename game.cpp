#include "game.h"
#include "singleCharacter.h"
#include <iostream>
#include <cstdio>
#include <string>

Player *Game::getPlayer() const { return player; }
void Game::setPlayer(Player *newPlayer) { player = newPlayer; }
Shop *Game::getShop() const { return shop; }

Game::Game()
{
    player = new Player();
    itemGenerator = new ItemGenerator();
    shop = new Shop();
    generateRandomInventory();
}

void Game::initializeSampleItems()
{
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
    std::vector<Item*> randomItems = itemGenerator->generateRandomItems(2, 5);
    for(Item* item : randomItems) {
        if(!player->getInventory()->addItemAtRandomPosition(item)) {
            delete item;
            break;
        }
    }
}

Game::~Game()
{
    delete player;
    delete itemGenerator;
    delete shop;
}

// ─────────────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────────────
void Game::showMainMenu()
{
    int sel = 0; // 0=Inventory 1=Shop 2=Game(future) 3=Quit
    const int OPTIONS = 4;

    while(true) {
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        std::cout << "╔══════════════════════════════╗" << std::endl;
        std::cout << "║     SERVANT BATTLE ROYALE    ║" << std::endl;
        std::cout << "╠══════════════════════════════╣" << std::endl;
        std::cout << "║                              ║" << std::endl;

        const char* labels[OPTIONS] = {
            "  Inventory",
            "  Shop",
            "  Battle Arena  ",
            "  Quit"
        };
        for(int i = 0; i < OPTIONS; i++) {
            if(i == sel)
                std::cout << "║  > " << labels[i] << std::endl;
            else
                std::cout << "║    " << labels[i] << std::endl;
        }

        std::cout << "║                              ║" << std::endl;
        std::cout << "║ Gold: " << shop->getPlayerGold() << "g";
        // pad to fixed width
        int goldLen = std::to_string(shop->getPlayerGold()).size();
        for(int p = 0; p < 22 - goldLen; p++) std::cout << " ";
        std::cout << "║" << std::endl;
        std::cout << "╚══════════════════════════════╝" << std::endl;
        std::cout << "  W/S - navigate  |  Enter - select" << std::endl;

        int key = (int)getSingleChar();
        switch(key) {
            case 119: // w
                sel = (sel - 1 + OPTIONS) % OPTIONS;
                break;
            case 115: // s
                sel = (sel + 1) % OPTIONS;
                break;
            case 10: case 13: // Enter
                if(sel == 0) { play(); }
                else if(sel == 1) { playShop(); }
                else if(sel == 2) {
                    printf("\033[2J"); printf("\033[H"); fflush(stdout);
                    std::cout << "Game mode coming soon! Press any key..." << std::endl;
                    getSingleChar();
                }
                else if(sel == 3) { return; }
                break;
            case 127: case 27: // Backspace / ESC
                return;
        }
    }
}

// ─────────────────────────────────────────────
//  SHOP SEARCH MENU  (full-screen filtered list)
// ─────────────────────────────────────────────
void Game::playSearchMenu()
{
    // Build query character by character using getSingleChar
    // so we stay in raw terminal mode the whole time
    std::string query = "";

    while(true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        std::cout << "=== SHOP SEARCH ===" << std::endl;
        std::cout << "Type item name, Enter to confirm, Backspace to delete, ESC to cancel" << std::endl;
        std::cout << std::endl;
        std::cout << "Search: " << query << "_" << std::endl;

        int key = (int)getSingleChar();

        if(key == 10 || key == 13) { // Enter - confirm
            break;
        } else if(key == 27) { // ESC - cancel
            return;
        } else if(key == 127 || key == 8) { // Backspace - delete last char
            if(!query.empty()) query.pop_back();
        } else if(key >= 32 && key <= 126) { // Printable character
            query += (char)key;
        }
    }

    if(query.empty()) return;

    shop->setSearchQuery(query);
    std::vector<int> results = shop->getSearchResults();

    if(results.empty()) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        std::cout << "No items found matching \"" << query << "\"." << std::endl;
        std::cout << "Press any key to return..." << std::endl;
        getSingleChar();
        shop->clearSearch();
        return;
    }

    int localSel = 0;
    int userInput = 0;

    while(userInput != 101) { // 'e' to exit
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        std::cout << "=== SEARCH RESULTS: \"" << query << "\" ===" << std::endl;
        std::cout << "Your Gold: " << shop->getPlayerGold() << "g" << std::endl;
        std::cout << "Found " << results.size() << " item(s)" << std::endl;
        std::cout << std::endl;
        std::cout << "   | Item Name     | Price | Rarity   | Dur | Lvl" << std::endl;
        std::cout << "---+---------------+-------+----------+-----+----" << std::endl;

        for(int i = 0; i < (int)results.size(); i++) {
            int idx = results[i];
            Item* item = shop->getItemAt(idx);
            if(!item) continue;

            if(i == localSel) std::cout << ">>";
            else              std::cout << "  ";

            std::string name = item->getName();
            std::cout << " " << name;
            for(int p = name.size(); p < 13; p++) std::cout << " ";
            std::cout << " | ";

            std::cout << item->getPrice();
            int plen = std::to_string(item->getPrice()).size();
            for(int p = plen; p < 5; p++) std::cout << " ";
            std::cout << " | ";

            switch(item->getRarity()) {
                case common: std::cout << "Common  "; break;
                case magic:  std::cout << "Magic   "; break;
                case rare:   std::cout << "Rare    "; break;
            }
            std::cout << " | ";

            std::cout << item->getDurability() << " | " << item->getLevel();
            std::cout << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Controls: W/S - Navigate | B - Buy | E - Exit search" << std::endl;

        userInput = (int)getSingleChar();
        switch(userInput) {
            case 119: // w
                localSel = (localSel - 1 + (int)results.size()) % (int)results.size();
                break;
            case 115: // s
                localSel = (localSel + 1) % (int)results.size();
                break;
            case 98: { // b - buy
                int actualIdx = results[localSel];
                Item* selectedItem = shop->getItemAt(actualIdx);
                if(selectedItem) {
                    if(shop->getPlayerGold() < selectedItem->getPrice()) {
                        std::cout << "\nNot enough gold!" << std::endl;
                        std::cout << "Press any key..." << std::endl;
                        getSingleChar();
                    } else if(player->getInventory()->getEmptySlotCount() == 0) {
                        std::cout << "\nInventory full!" << std::endl;
                        std::cout << "Press any key..." << std::endl;
                        getSingleChar();
                    } else {
                        Item* bought = shop->buyItemNoPlace(actualIdx);
                        if(bought) {
                            placeItemMenu(bought);
                            results = shop->getSearchResults();
                            if(results.empty()) { userInput = 101; break; }
                            if(localSel >= (int)results.size()) localSel = (int)results.size() - 1;
                        }
                    }
                }
                break;
            }
        }
    }

    shop->clearSearch();
}

// ─────────────────────────────────────────────
//  SELL CONFIRMATION SCREEN
// ─────────────────────────────────────────────
void Game::showSellConfirmScreen(Item* item, int row, int col)
{
    if(item == nullptr) return;

    int pct = 80 + (rand() % 11); // 80-90%
    int sellPrice = (item->getPrice() * pct) / 100;

    while(true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        std::cout << "╔═════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                   SELL ITEM?                    ║" << std::endl;
        std::cout << "╠═════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║                                                 ║" << std::endl;
        std::cout << "║  Item:   " << item->getName() << "               ";
        int namePad = 24 - (int)item->getName().size();
        for(int i = 0; i < namePad; i++) std::cout << " ";
        std::cout << "║" << std::endl;

        std::cout << "║  Worth:  " << item->getPrice() << "g (full price)           ";
        int wp = 14 - (int)std::to_string(item->getPrice()).size();
        for(int i = 0; i < wp; i++) std::cout << " ";
        std::cout << "║" << std::endl;

        std::cout << "║  Sell:   " << sellPrice << "g (" << pct << "%)                  ";
        int sp = 16 - (int)std::to_string(sellPrice).size() - (int)std::to_string(pct).size();
        for(int i = 0; i < sp; i++) std::cout << " ";
        std::cout << "║" << std::endl;


        std::cout << "║                                                 ║" << std::endl;
        std::cout << "╠═════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║  Y - Confirm sell                               ║" << std::endl;
        std::cout << "║  N - Cancel                                     ║" << std::endl;
        std::cout << "╚═════════════════════════════════════════════════╝" << std::endl;

        int key = (int)getSingleChar();
        if(key == 121 || key == 89) { // Y or y
            player->getInventory()->setItemAt(row, col, nullptr);
            shop->addGold(sellPrice);
            printf("\033[2J"); printf("\033[H"); fflush(stdout);
            std::cout << "Sold " << item->getName() << " for " << sellPrice << "g (" << pct << "%)!" << std::endl;
            std::cout << "Gold: " << shop->getPlayerGold() << "g" << std::endl;
            std::cout << "Press any key..." << std::endl;
            delete item;
            getSingleChar();
            return;
        } else if(key == 110 || key == 78 || key == 27) { // N, n, ESC
            return;
        }
    }
}

// ─────────────────────────────────────────────
//  MANUAL ITEM PLACEMENT after buying
// ─────────────────────────────────────────────
void Game::placeItemMenu(Item* item)
{
    Inventory* inv = player->getInventory();
    const int BAG_ROWS = 3;
    const int BAG_COLS = 3;
    int curRow = 0;
    int curCol = 0;

    // Build 3-char tag for the held item
    const std::string& iname = item->getName();
    std::string itemTag;
    for(int k = 0; k < 3; k++)
        itemTag += (k < (int)iname.size()) ? (char)toupper((unsigned char)iname[k]) : ' ';

    while(true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        std::cout << "Holding: " << item->getName() << "  [" << item->getPrice() << "g | ";
        switch(item->getRarity()) {
            case common: std::cout << "Common"; break;
            case magic:  std::cout << "Magic";  break;
            case rare:   std::cout << "Rare";   break;
        }
        std::cout << "]" << std::endl;
        std::cout << std::endl;

        // Draw only the 3x3 bag
        for(int i = 0; i < BAG_ROWS; i++) {
            for(int j = 0; j < BAG_COLS; j++) {
                bool isCursor = (i == curRow && j == curCol);
                Item* slot = inv->getItemAt(i, j);

                std::cout << (isCursor ? "<" : "[");
                if(isCursor)       std::cout << itemTag;
                else if(!slot)     std::cout << "   ";
                else               std::cout << inv->getItemDisplayStr(i, j);
                std::cout << (isCursor ? "> " : "] ");
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
        Item* curSlot = inv->getItemAt(curRow, curCol);
        if(!curSlot)
            std::cout << "[ EMPTY ]  Enter-Place  |  ESC-Auto-place  |  Q-Cancel buy" << std::endl;
        else
            std::cout << "[ " << curSlot->getName() << " ]  Occupied!  |  Q-Cancel buy" << std::endl;

        int key = (int)getSingleChar();
        switch(key) {
            case 97:  curCol = (curCol - 1 + BAG_COLS) % BAG_COLS; break; // a
            case 100: curCol = (curCol + 1) % BAG_COLS;             break; // d
            case 119: curRow = (curRow - 1 + BAG_ROWS) % BAG_ROWS;  break; // w
            case 115: curRow = (curRow + 1) % BAG_ROWS;             break; // s

            case 113: // 'q' - Cancel purchase, refund gold
                shop->addGold(item->getPrice());
                shop->addItemToShop(item);
                return;

            case 10: case 13: {
                if(!inv->getItemAt(curRow, curCol)) {
                    inv->setItemAt(curRow, curCol, item);
                    return;
                } else {
                    std::cout << "\nSlot occupied!" << std::endl;
                    std::cout << "Press any key..." << std::endl;
                    getSingleChar();
                }
                break;
            }

            case 27: {
                if(!inv->addItemAtRandomPosition(item)) {
                    shop->addGold(item->getPrice());
                    shop->addItemToShop(item);
                    std::cout << "\nInventory full! Purchase refunded." << std::endl;
                    std::cout << "Press any key..." << std::endl;
                    getSingleChar();
                }
                return;
            }
        }
    }
}

// ─────────────────────────────────────────────
//  SHOP
void Game::playShop()
{
    int userInput = 0;

    std::cout << "\n=== Welcome to the Shop! ===" << std::endl;
    std::cout << "Press any key to continue..." << std::endl;
    getSingleChar();

    while(userInput != 101) { // 'e' to exit
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        shop->clearSearch();
        shop->displayShop();

        userInput = (int)getSingleChar();

        switch(userInput) {
            case 119: shop->selectPrevious(); break; // w
            case 115: shop->selectNext();     break; // s

            case 98: { // 'b' - Buy
                int selectedIdx = shop->getSelectedIndex();
                Item* selectedItem = shop->getSelectedItem();
                if(selectedItem) {
                    if(shop->getPlayerGold() < selectedItem->getPrice()) {
                        std::cout << "\nNot enough gold! Need " << selectedItem->getPrice()
                                  << "g, have " << shop->getPlayerGold() << "g" << std::endl;
                        std::cout << "Press any key..." << std::endl;
                        getSingleChar();
                    } else if(player->getInventory()->getEmptySlotCount() == 0) {
                        std::cout << "\nInventory is full!" << std::endl;
                        std::cout << "Press any key..." << std::endl;
                        getSingleChar();
                    } else {
                        Item* bought = shop->buyItemNoPlace(selectedIdx);
                        if(bought) {
                            placeItemMenu(bought);
                        }
                    }
                }
                break;
            }

            case 102: // 'f' - Search (new full-screen search menu)
                playSearchMenu();
                break;

            case 114: // 'r' - Restock
                shop->restockShop();
                std::cout << "\nShop restocked!" << std::endl;
                std::cout << "Press any key..." << std::endl;
                getSingleChar();
                break;

            case 118: // 'v' - Sell menu
                sellItemsMenu();
                break;
        }
    }
}

// ─────────────────────────────────────────────
//  SELL ITEMS MENU  (H to sell at 80-90%)
// ─────────────────────────────────────────────
void Game::sellItemsMenu()
{
    int userInput = 0;

    while(userInput != 101) { // 'e' to exit
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        std::cout << "=== SELL ITEMS ===" << std::endl;
        std::cout << "Your Gold: " << shop->getPlayerGold() << "g" << std::endl;
        std::cout << std::endl;

        player->getInventory()->display();

        // Preview sell price for current item
        Item* cur = player->getInventory()->getCurrentItem();
        if(cur) {
            int lo = (int)(cur->getPrice() * 0.80);
            int hi = (int)(cur->getPrice() * 0.90);
            std::cout << "\nSelected: " << cur->getName()
                      << "  |  Sell value: " << lo << "-" << hi << "g" << std::endl;
        } else {
            std::cout << "\nNo item at cursor." << std::endl;
        }

        // Controls hint
        if(player->getInventory()->getIsDragging()) {
            std::cout << "\n[DRAGGING] G - Drop/Swap  |  K - Cancel drag" << std::endl;
        } else {
            std::cout << "\nControls: W/A/S/D - Navigate | H - Sell | G - Grab/Drop | K - Cancel drag | E - Exit" << std::endl;
        }

        userInput = (int)getSingleChar();

        switch(userInput) {
            case 97:  player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() - 1); break;
            case 100: player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() + 1); break;
            case 119: player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() - 1); break;
            case 115: player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() + 1); break;

            case 103: // 'g' - Grab or Drop
                if(player->getInventory()->getIsDragging()) {
                    player->getInventory()->dropItem();
                } else {
                    player->getInventory()->startDrag();
                }
                break;

            case 107: // 'k' - Cancel drag
                player->getInventory()->cancelDrag();
                break;

            case 104: { // 'h' - Sell (blocked while dragging)
                if(player->getInventory()->getIsDragging()) {
                    std::cout << "\nCannot sell while dragging! Press K to cancel." << std::endl;
                    std::cout << "Press any key..." << std::endl;
                    getSingleChar();
                    break;
                }
                Item* itemToSell = player->getInventory()->getCurrentItem();
                if(itemToSell) {
                    int r = player->getInventory()->getCurrentRow();
                    int c = player->getInventory()->getCurrentCol();
                    showSellConfirmScreen(itemToSell, r, c);
                } else {
                    std::cout << "\nNo item to sell!" << std::endl;
                    std::cout << "Press any key..." << std::endl;
                    getSingleChar();
                }
                break;
            }
        }
    }
}

// ─────────────────────────────────────────────
//  INVENTORY  (G=grab/drop, K=cancel)
// ─────────────────────────────────────────────
void Game::play()
{
    int userInput = 0;
    bool showItemInfo = false;

    while(userInput != 127) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        std::cout << "Gold: " << shop->getPlayerGold() << "g" << std::endl;
        std::cout << std::endl;

        if(showItemInfo) {
            Item* currentItem = player->getInventory()->getCurrentItem();
            if(currentItem) player->getInventory()->displayWithItemInfo(currentItem);
            else            player->getInventory()->displayWithEmptyInfo();
        } else {
            player->getInventory()->display();
        }

        // Controls hint
        if(player->getInventory()->getIsDragging()) {
            std::cout << "\n[DRAGGING] G - Drop/Swap  |  K - Cancel drag" << std::endl;
        } else {
            std::cout << "\nW/A/S/D-Move  G-Grab  T-Equip  I-Info  1/2/3-Filter  0-All  H-Sell  Backspace-Menu" << std::endl;
        }

        userInput = (int)getSingleChar();
        switch(userInput) {
            case 97:  player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() - 1); showItemInfo = false; break;
            case 100: player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() + 1); showItemInfo = false; break;
            case 119: player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() - 1); showItemInfo = false; break;
            case 115: player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() + 1); showItemInfo = false; break;

            case 103: // 'g' - Grab or Drop
                if(player->getInventory()->getIsDragging()) {
                    player->getInventory()->dropItem();
                } else {
                    player->getInventory()->startDrag();
                }
                showItemInfo = false;
                break;

            case 107: // 'k' - Cancel drag
                player->getInventory()->cancelDrag();
                showItemInfo = false;
                break;

            case 104: { // 'h' - Quick sell from inventory
                Item* itemToSell = player->getInventory()->getCurrentItem();
                if(itemToSell) {
                    int r = player->getInventory()->getCurrentRow();
                    int c = player->getInventory()->getCurrentCol();
                    showSellConfirmScreen(itemToSell, r, c);
                } else {
                    std::cout << "\nNo item here to sell!" << std::endl;
                    std::cout << "Press any key..." << std::endl;
                    getSingleChar();
                }
                showItemInfo = false;
                break;
            }

            case 116: { // 't' - Equip item to equipment slot
                if(!player->getInventory()->equipItem()) {
                    std::cout << "\nNo item here or equipment slots full!" << std::endl;
                    std::cout << "Press any key..." << std::endl;
                    getSingleChar();
                }
                showItemInfo = false;
                break;
            }

            case 105: showItemInfo = !showItemInfo; break; // 'i'
            case 49:  player->getInventory()->setFilter(common); showItemInfo = false; break;
            case 50:  player->getInventory()->setFilter(magic);  showItemInfo = false; break;
            case 51:  player->getInventory()->setFilter(rare);   showItemInfo = false; break;
            case 48:  player->getInventory()->clearFilter();     showItemInfo = false; break;
            case 127: return; // Backspace → back to main menu
        }
    }
}