#include "game.h"
#include "singleCharacter.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

// Convenience
using std::cout;
using std::endl;
using std::string;
using std::vector;

// ─────────────────────────────────────────────
// Basic getters
// ─────────────────────────────────────────────

Player* Game::getPlayer() const {
    return player;
}

void Game::setPlayer(Player* newPlayer) {
    player = newPlayer;
}

Shop* Game::getShop() const {
    return shop;
}

// ─────────────────────────────────────────────
// Construction / destruction
// ─────────────────────────────────────────────

Game::Game()
{
    player = new Player();
    itemGenerator = new ItemGenerator();
    shop = new Shop();

    // Random servant at game start
    Servant myServant = Servant::randomServant();
    player->setServant(myServant);

    generateRandomInventory();
}

Game::~Game()
{
    delete player;
    delete itemGenerator;
    delete shop;
}

// ─────────────────────────────────────────────
// Item initialization
// ─────────────────────────────────────────────

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
    vector<Item*> randomItems = itemGenerator->generateRandomItems(2, 5);
    for (Item* item : randomItems) {
        if (!player->getInventory()->addItemAtRandomPosition(item)) {
            delete item;
            break;
        }
    }
}

// ─────────────────────────────────────────────
// Main menu
// ─────────────────────────────────────────────

void Game::showMainMenu()
{
    int sel = 0;              // 0=Inventory 1=Shop 2=Battle Arena 3=Quit
    const int OPTIONS = 4;

    while (true) {
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        cout << "╔══════════════════════════════╗" << endl;
        cout << "║      SERVANT BATTLE ROYALE   ║" << endl;
        cout << "╠══════════════════════════════╣" << endl;
        cout << "║                              ║" << endl;

        const char* labels[OPTIONS] = {
            " Inventory",
            " Shop",
            " Battle Arena ",
            " Quit"
        };

        for (int i = 0; i < OPTIONS; i++) {
            if (i == sel)
                cout << "║ > " << labels[i] << endl;
            else
                cout << "║   " << labels[i] << endl;
        }

        cout << "║                              ║" << endl;
        cout << "║ Gold: " << shop->getPlayerGold() << "g";
        int goldLen = (int)std::to_string(shop->getPlayerGold()).size();
        for (int p = 0; p < 22 - goldLen; p++) cout << " ";
        cout << "║" << endl;
        cout << "╚══════════════════════════════╝" << endl;
        cout << " W/S - navigate | Enter - select" << endl;

        int key = (int)getSingleChar();
        switch (key) {
        case 'w':
        case 'W':
            sel = (sel - 1 + OPTIONS) % OPTIONS;
            break;
        case 's':
        case 'S':
            sel = (sel + 1) % OPTIONS;
            break;
        case 10:
        case 13: // Enter
            if (sel == 0) { play(); }
            else if (sel == 1) { playShop(); }
            else if (sel == 2) { playBattleArena(); }
            else if (sel == 3) { return; }
            break;
        case 127: // Backspace
        case 27:  // ESC
            return;
        default:
            break;
        }
    }
}

// ─────────────────────────────────────────────
// Shop search menu (full-screen filtered list)
// ─────────────────────────────────────────────

void Game::playSearchMenu()
{
    string query;

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << "=== SHOP SEARCH ===" << endl;
        cout << "Type item name, Enter to confirm, Backspace to delete, ESC to cancel" << endl;
        cout << endl;
        cout << "Search: " << query << "_" << endl;

        int key = (int)getSingleChar();

        if (key == 10 || key == 13) {           // Enter - confirm
            break;
        } else if (key == 27) {                 // ESC - cancel
            return;
        } else if (key == 127 || key == 8) {    // Backspace
            if (!query.empty()) query.pop_back();
        } else if (key >= 32 && key <= 126) {   // Printable
            query += (char)key;
        }
    }

    if (query.empty()) return;

    shop->setSearchQuery(query);
    vector<int> results = shop->getSearchResults();

    if (results.empty()) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        cout << "No items found matching \"" << query << "\"." << endl;
        cout << "Press any key to return..." << endl;
        getSingleChar();
        shop->clearSearch();
        return;
    }

    int localSel = 0;
    int userInput = 0;

    while (userInput != 'e' && userInput != 'E') {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << "=== SEARCH RESULTS: \"" << query << "\" ===" << endl;
        cout << "Your Gold: " << shop->getPlayerGold() << "g" << endl;
        cout << "Found " << results.size() << " item(s)" << endl;
        cout << endl;
        cout << "   | Item Name     | Price | Rarity   | Dur | Lvl" << endl;
        cout << "---+---------------+-------+----------+-----+----" << endl;

        for (int i = 0; i < (int)results.size(); i++) {
            int idx = results[i];
            Item* item = shop->getItemAt(idx);
            if (!item) continue;

            if (i == localSel) cout << ">>";
            else cout << "  ";

            string name = item->getName();
            cout << " " << name;
            for (int p = (int)name.size(); p < 13; p++) cout << " ";
            cout << " | ";

            cout << item->getPrice();
            int plen = (int)std::to_string(item->getPrice()).size();
            for (int p = plen; p < 5; p++) cout << " ";
            cout << " | ";

            switch (item->getRarity()) {
            case common: cout << "Common "; break;
            case magic:  cout << "Magic  "; break;
            case rare:   cout << "Rare   "; break;
            }
            cout << " | ";

            cout << item->getDurability() << " | " << item->getLevel();
            cout << endl;
        }

        cout << endl;
        cout << "Controls: W/S - Navigate | B - Buy | E - Exit search" << endl;

        userInput = (int)getSingleChar();
        switch (userInput) {
        case 'w': case 'W':
            localSel = (localSel - 1 + (int)results.size()) % (int)results.size();
            break;
        case 's': case 'S':
            localSel = (localSel + 1) % (int)results.size();
            break;
        case 'b': case 'B': {
            int actualIdx = results[localSel];
            Item* selectedItem = shop->getItemAt(actualIdx);
            if (selectedItem) {
                if (shop->getPlayerGold() < selectedItem->getPrice()) {
                    cout << "\nNot enough gold!" << endl;
                    cout << "Press any key..." << endl;
                    getSingleChar();
                } else if (player->getInventory()->getEmptySlotCount() == 0) {
                    cout << "\nInventory full!" << endl;
                    cout << "Press any key..." << endl;
                    getSingleChar();
                } else {
                    Item* bought = shop->buyItemNoPlace(actualIdx);
                    if (bought) {
                        placeItemMenu(bought);
                        results = shop->getSearchResults();
                        if (results.empty()) { userInput = 'e'; break; }
                        if (localSel >= (int)results.size())
                            localSel = (int)results.size() - 1;
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    }

    shop->clearSearch();
}

// ─────────────────────────────────────────────
// Sell confirmation screen
// ─────────────────────────────────────────────

void Game::showSellConfirmScreen(Item* item, int row, int col)
{
    if (item == nullptr) return;

    int pct = 80 + (std::rand() % 11); // 80-90%
    int sellPrice = (item->getPrice() * pct) / 100;

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << "╔═════════════════════════════════════════════════╗" << endl;
        cout << "║                 SELL ITEM?                      ║" << endl;
        cout << "╠═════════════════════════════════════════════════╣" << endl;
        cout << "║                                                 ║" << endl;
        cout << "║ Item: " << item->getName() << " ";
        int namePad = 24 - (int)item->getName().size();
        for (int i = 0; i < namePad; i++) cout << " ";
        cout << "║" << endl;

        cout << "║ Worth: " << item->getPrice() << "g (full price) ";
        int wp = 14 - (int)std::to_string(item->getPrice()).size();
        for (int i = 0; i < wp; i++) cout << " ";
        cout << "║" << endl;

        cout << "║ Sell: " << sellPrice << "g (" << pct << "%) ";
        int sp = 16 - (int)std::to_string(sellPrice).size()
                 - (int)std::to_string(pct).size();
        for (int i = 0; i < sp; i++) cout << " ";
        cout << "║" << endl;

        cout << "║                                                 ║" << endl;
        cout << "╠═════════════════════════════════════════════════╣" << endl;
        cout << "║      Y - Confirm sell    |    N/ESC - Cancel    ║" << endl;
        cout << "╚═════════════════════════════════════════════════╝" << endl;

        int key = (int)getSingleChar();
        if (key == 'y' || key == 'Y') {
            player->getInventory()->setItemAt(row, col, nullptr);
            shop->addGold(sellPrice);
            printf("\033[2J"); printf("\033[H"); fflush(stdout);
            cout << "Sold " << item->getName() << " for "
                 << sellPrice << "g (" << pct << "%)!" << endl;
            cout << "Gold: " << shop->getPlayerGold() << "g" << endl;
            cout << "Press any key..." << endl;
            delete item;
            getSingleChar();
            return;
        } else if (key == 'n' || key == 'N' || key == 27) {
            return;
        }
    }
}

// ─────────────────────────────────────────────
// Manual item placement after buying
// ─────────────────────────────────────────────

void Game::placeItemMenu(Item* item)
{
    Inventory* inv = player->getInventory();
    const int BAG_ROWS = 2;
    const int BAG_COLS = 3;
    int curRow = 0;
    int curCol = 0;

    // Build 3-char tag for the held item
    const string& iname = item->getName();
    string itemTag;
    for (int k = 0; k < 3; k++)
        itemTag += (k < (int)iname.size())
                   ? (char)std::toupper((unsigned char)iname[k]) : ' ';

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << "Holding: " << item->getName()
             << " [" << item->getPrice() << "g | ";
        switch (item->getRarity()) {
        case common: cout << "Common"; break;
        case magic:  cout << "Magic";  break;
        case rare:   cout << "Rare";   break;
        }
        cout << "]" << endl;
        cout << endl;

        // Draw only the 2x3 bag (rows 0–1, cols 0–2)
        for (int i = 0; i < BAG_ROWS; i++) {
            for (int j = 0; j < BAG_COLS; j++) {
                bool isCursor = (i == curRow && j == curCol);
                Item* slot = inv->getItemAt(i, j);

                cout << (isCursor ? "<" : "[");
                if (isCursor) cout << itemTag;
                else if (!slot) cout << "   ";
                else cout << inv->getItemDisplayStr(i, j);
                cout << (isCursor ? "> " : "] ");
            }
            cout << endl;
        }

        cout << endl;
        Item* curSlot = inv->getItemAt(curRow, curCol);
        if (!curSlot)
            cout << "[ EMPTY ] Enter-Place | ESC-Auto-place | Q-Cancel buy" << endl;
        else
            cout << "[ " << curSlot->getName()
                 << " ] Occupied! | Q-Cancel buy" << endl;

        int key = (int)getSingleChar();
        switch (key) {
        case 'a': case 'A':
            curCol = (curCol - 1 + BAG_COLS) % BAG_COLS; break;
        case 'd': case 'D':
            curCol = (curCol + 1) % BAG_COLS; break;
        case 'w': case 'W':
            curRow = (curRow - 1 + BAG_ROWS) % BAG_ROWS; break;
        case 's': case 'S':
            curRow = (curRow + 1) % BAG_ROWS; break;

        case 'q': case 'Q': {    // Cancel purchase, refund gold
            shop->addGold(item->getPrice());
            shop->addItemToShop(item);
            return;
        }

        case 10:
        case 13: {  // Enter - place if empty
            if (!inv->getItemAt(curRow, curCol)) {
                inv->setItemAt(curRow, curCol, item);
                return;
            } else {
                cout << "\nSlot occupied!" << endl;
                cout << "Press any key..." << endl;
                getSingleChar();
            }
            break;
        }

        case 27: { // ESC → try auto-place
            if (!inv->addItemAtRandomPosition(item)) {
                shop->addGold(item->getPrice());
                shop->addItemToShop(item);
                cout << "\nInventory full! Purchase refunded." << endl;
                cout << "Press any key..." << endl;
                getSingleChar();
            }
            return;
        }

        default:
            break;
        }
    }
}

// ─────────────────────────────────────────────
// Shop
// ─────────────────────────────────────────────

void Game::playShop()
{
    int userInput = 0;

    cout << "\n=== Welcome to the Shop! ===" << endl;
    cout << "Press any key to continue..." << endl;
    getSingleChar();

    while (userInput != 'e' && userInput != 'E') {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        shop->clearSearch();
        shop->displayShop();

        userInput = (int)getSingleChar();

        switch (userInput) {
        case 'w': case 'W':
            shop->selectPrevious();
            break;
        case 's': case 'S':
            shop->selectNext();
            break;

        case 'b': case 'B': { // Buy
            int selectedIdx = shop->getSelectedIndex();
            Item* selectedItem = shop->getSelectedItem();
            if (selectedItem) {
                if (shop->getPlayerGold() < selectedItem->getPrice()) {
                    cout << "\nNot enough gold! Need " << selectedItem->getPrice()
                         << "g, have " << shop->getPlayerGold() << "g" << endl;
                    cout << "Press any key..." << endl;
                    getSingleChar();
                } else if (player->getInventory()->getEmptySlotCount() == 0) {
                    cout << "\nInventory is full!" << endl;
                    cout << "Press any key..." << endl;
                    getSingleChar();
                } else {
                    Item* bought = shop->buyItemNoPlace(selectedIdx);
                    if (bought) {
                        placeItemMenu(bought);
                    }
                }
            }
            break;
        }

        case 'f': case 'F':   // Search
            playSearchMenu();
            break;

        case 'r': case 'R':   // Restock
            shop->restockShop();
            cout << "\nShop restocked!" << endl;
            cout << "Press any key..." << endl;
            getSingleChar();
            break;

        case 'v': case 'V':   // Sell menu
            sellItemsMenu();
            break;

        default:
            break;
        }
    }
}

// ─────────────────────────────────────────────
// Sell items menu
// ─────────────────────────────────────────────

void Game::sellItemsMenu()
{
    int userInput = 0;

    while (userInput != 'e' && userInput != 'E') {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << "=== SELL ITEMS ===" << endl;
        cout << "Your Gold: " << shop->getPlayerGold() << "g" << endl;
        cout << endl;

        player->getInventory()->display();

        Item* cur = player->getInventory()->getCurrentItem();
        if (cur) {
            int lo = (int)(cur->getPrice() * 0.80);
            int hi = (int)(cur->getPrice() * 0.90);
            cout << "\nSelected: " << cur->getName()
                 << " | Sell value: " << lo << "-" << hi << "g" << endl;
        } else {
            cout << "\nNo item at cursor." << endl;
        }

        // Controls hint
        if (player->getInventory()->getIsDragging()) {
            cout << "\n[DRAGGING] G - Drop/Swap | K - Cancel drag" << endl;
        } else {
            cout << "\nControls: W/A/S/D - Navigate | H - Sell | "
                 << "G - Grab/Drop | K - Cancel drag | E - Exit" << endl;
        }

        userInput = (int)getSingleChar();

        switch (userInput) {
        case 'a': case 'A':
            player->getInventory()->setCurrentCol(
                player->getInventory()->getCurrentCol() - 1); break;
        case 'd': case 'D':
            player->getInventory()->setCurrentCol(
                player->getInventory()->getCurrentCol() + 1); break;
        case 'w': case 'W':
            player->getInventory()->setCurrentRow(
                player->getInventory()->getCurrentRow() - 1); break;
        case 's': case 'S':
            player->getInventory()->setCurrentRow(
                player->getInventory()->getCurrentRow() + 1); break;

        case 'g': case 'G':   // Grab or Drop
            if (player->getInventory()->getIsDragging()) {
                player->getInventory()->dropItem();
            } else {
                player->getInventory()->startDrag();
            }
            break;

        case 'k': case 'K':   // Cancel drag
            player->getInventory()->cancelDrag();
            break;

        case 'h': case 'H': { // Sell (blocked while dragging)
            if (player->getInventory()->getIsDragging()) {
                cout << "\nCannot sell while dragging! Press K to cancel." << endl;
                cout << "Press any key..." << endl;
                getSingleChar();
                break;
            }

            Item* itemToSell = player->getInventory()->getCurrentItem();
            if (itemToSell) {
                int r = player->getInventory()->getCurrentRow();
                int c = player->getInventory()->getCurrentCol();
                showSellConfirmScreen(itemToSell, r, c);
            } else {
                cout << "\nNo item to sell!" << endl;
                cout << "Press any key..." << endl;
                getSingleChar();
            }
            break;
        }

        default:
            break;
        }
    }
}

// ─────────────────────────────────────────────
// Inventory (G=grab/drop, K=cancel)
// ─────────────────────────────────────────────

void Game::play()
{
    int userInput = 0;
    bool showItemInfo = false;

    while (userInput != 127) { // Backspace to exit
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << "Gold: " << shop->getPlayerGold() << "g" << endl;
        cout << endl;

        if (showItemInfo) {
            Item* currentItem = player->getInventory()->getCurrentItem();
            if (currentItem) player->getInventory()->displayWithItemInfo(currentItem);
            else player->getInventory()->displayWithEmptyInfo();
        } else {
            player->getInventory()->display();
        }

        // Show the player's servant under the inventory
        cout << "\nYour Servant: " << player->getServant()->getName() << endl;
        player->getServant()->renderAscii();

        // Controls hint
        if (player->getInventory()->getIsDragging()) {
            cout << "\n[DRAGGING] G - Drop/Swap | K - Cancel drag" << endl;
        } else {
            cout << "\nW/A/S/D-Move  G-Grab  T-Equip  I-Info  "
                 << "1/2/3-Filter 0-All  H-Sell  Backspace-Menu" << endl;
        }

        userInput = (int)getSingleChar();
        switch (userInput) {
        case 'a': case 'A':
            player->getInventory()->setCurrentCol(
                player->getInventory()->getCurrentCol() - 1);
            showItemInfo = false;
            break;
        case 'd': case 'D':
            player->getInventory()->setCurrentCol(
                player->getInventory()->getCurrentCol() + 1);
            showItemInfo = false;
            break;
        case 'w': case 'W':
            player->getInventory()->setCurrentRow(
                player->getInventory()->getCurrentRow() - 1);
            showItemInfo = false;
            break;
        case 's': case 'S':
            player->getInventory()->setCurrentRow(
                player->getInventory()->getCurrentRow() + 1);
            showItemInfo = false;
            break;

        case 'g': case 'G': {
            if (player->getInventory()->getIsDragging()) {
                player->getInventory()->dropItem();
            } else {
                player->getInventory()->startDrag();
            }
            showItemInfo = false;
            break;
        }

        case 'k': case 'K':
            player->getInventory()->cancelDrag();
            showItemInfo = false;
            break;

        case 'h': case 'H': { // quick sell from inventory
            Item* itemToSell = player->getInventory()->getCurrentItem();
            if (itemToSell) {
                int r = player->getInventory()->getCurrentRow();
                int c = player->getInventory()->getCurrentCol();
                showSellConfirmScreen(itemToSell, r, c);
            } else {
                cout << "\nNo item here to sell!" << endl;
                cout << "Press any key..." << endl;
                getSingleChar();
            }
            showItemInfo = false;
            break;
        }

        case 't': case 'T': { // Equip
            if (!player->getInventory()->equipItem()) {
                cout << "\nNo item here or equipment slots full!" << endl;
                cout << "Press any key..." << endl;
                getSingleChar();
            }
            showItemInfo = false;
            break;
        }

        case 'i': case 'I':
            showItemInfo = !showItemInfo;
            break;

        case '1':
            player->getInventory()->setFilter(common);
            showItemInfo = false;
            break;
        case '2':
            player->getInventory()->setFilter(magic);
            showItemInfo = false;
            break;
        case '3':
            player->getInventory()->setFilter(rare);
            showItemInfo = false;
            break;
        case '0':
            player->getInventory()->clearFilter();
            showItemInfo = false;
            break;

        case 127: // Backspace -> back to main menu
            return;

        default:
            break;
        }
    }
}

// ─────────────────────────────────────────────
// Battle Arena
// ─────────────────────────────────────────────

void Game::playBattleArena()
{
    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        Servant* myServant = player->getServant();
        cout << "=== BATTLE ARENA ===" << endl;
        cout << "Your Servant: " << myServant->getName() << endl;
        myServant->renderAscii();

        auto pool = Servant::enemyPoolForSeries(myServant->getSeries());
        if (pool.empty()) {
            cout << "\nNo enemies defined yet for this series." << endl;
            cout << "Press any key to return..." << endl;
            getSingleChar();
            return;
        }

        int idx = std::rand() % pool.size();
        Enemy enemy(pool[idx]);

        battleOneEnemy(enemy);

        cout << "\nBattle finished. Press B to battle again, any other key to exit..." << endl;
        int key = (int)getSingleChar();
        if (key != 'b' && key != 'B') return;
    }
}

void Game::battleOneEnemy(Enemy& enemy)
{
    Servant* myServant = player->getServant();
    Servant& en = enemy.getServant();

    myServant->resetForNewBattle();
    en.resetForNewBattle();

    int pMaxHP = player->getEffectiveMaxHP();
    int pHP = pMaxHP;
    int eHP = en.getMaxHP();

    int pStrEff = player->getEffectiveStrength();
    int pDurEff = player->getEffectiveDurability();
    int pAgiEff = player->getEffectiveAgility();

    int eStrEff = en.getStrength();
    int eDurEff = en.getDurability();
    int eAgiEff = en.getAgility();

    while (pHP > 0 && eHP > 0) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << "=== DUEL ===" << endl;
        cout << myServant->getName() << "  HP: " << pHP << "/" << pMaxHP << endl;
        myServant->renderAscii();
        cout << "\nVS\n\n";
        cout << en.getName() << "  HP: " << eHP << "/" << en.getMaxHP() << endl;
        en.renderAscii();

        cout << "\nChoose action:" << endl;
        cout << "A - Basic attack" << endl;

        const auto& nps = myServant->getNPs();
        for (size_t i = 0; i < nps.size(); ++i) {
            cout << (i + 1) << " - NP: " << nps[i].name;
            if (!myServant->isNPAvailable(i)) cout << " (used)";
            cout << endl;
        }
        cout << "E - Escape battle" << endl;

        int key = (int)getSingleChar();

        // Player turn
        bool usedTurn = false;
        if (key == 'e' || key == 'E') {
            cout << "\nYou escaped!" << endl;
            cout << "Press any key..." << endl;
            getSingleChar();
            return;
        } else if (key == 'a' || key == 'A') {
            int hitChance = 75 + (pAgiEff - eAgiEff) * 2;
            if (hitChance < 10) hitChance = 10;
            if (hitChance > 95) hitChance = 95;
            int roll = std::rand() % 100;
            if (roll < hitChance) {
                int dmg = myServant->basicAttackDamage(
                    pStrEff - myServant->getStrength(),
                    eDurEff
                );
                eHP -= dmg;
                if (eHP < 0) eHP = 0;
                cout << "\nYou hit the enemy for " << dmg << " damage!" << endl;
            } else {
                cout << "\nYour attack missed!" << endl;
            }
            usedTurn = true;
        } else if (key >= '1' && key <= '9') {
            size_t idx = static_cast<size_t>(key - '1');
            if (idx < nps.size() && myServant->isNPAvailable(idx)) {
                int hitChance = 75 + (pAgiEff - eAgiEff) * 2;
                if (hitChance < 10) hitChance = 10;
                if (hitChance > 95) hitChance = 95;
                int roll = std::rand() % 100;
                if (roll < hitChance) {
                    int dmg = myServant->noblePhantasmDamage(
                        idx,
                        pStrEff - myServant->getStrength(),
                        eDurEff
                    );
                    eHP -= dmg;
                    if (eHP < 0) eHP = 0;
                    myServant->markNPUsed(idx);
                    cout << "\nYou unleashed " << nps[idx].name
                         << " for " << dmg << " damage!" << endl;
                } else {
                    cout << "\nYour Noble Phantasm missed!" << endl;
                    myServant->markNPUsed(idx);
                }
                usedTurn = true;
            }
        }

        if (!usedTurn) {
            cout << "\nInvalid choice, you hesitate and lose your turn..." << endl;
        }

        if (eHP <= 0) {
            cout << "\nEnemy defeated!" << endl;
            cout << "Press any key..." << endl;
            getSingleChar();
            break;
        }

        // Enemy turn – simple AI: basic attack only
        cout << "\nEnemy's turn..." << endl;

        int hitChanceE = 75 + (eAgiEff - pAgiEff) * 2;
        if (hitChanceE < 10) hitChanceE = 10;
        if (hitChanceE > 95) hitChanceE = 95;
        int rollE = std::rand() % 100;

        if (rollE < hitChanceE) {
            int dmg = en.basicAttackDamage(
                eStrEff - en.getStrength(),
                pDurEff
            );
            pHP -= dmg;
            if (pHP < 0) pHP = 0;
            cout << "Enemy hits you for " << dmg << " damage!" << endl;
        } else {
            cout << "Enemy attack missed!" << endl;
        }

        if (pHP <= 0) {
            cout << "\nYou were defeated..." << endl;
            cout << "Press any key..." << endl;
            getSingleChar();
            break;
        }

        cout << "\nPress any key for next round..." << endl;
        getSingleChar();
    }
}
