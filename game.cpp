#include "game.h"
#include "singleCharacter.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cctype>

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
    player->getInventory()->addItem(new Item("Sword", 50, sword));
    player->getInventory()->addItem(new Item("Potion", 10, potion));
    player->getInventory()->addItem(new Item("Armor", 80, armor));
    player->getInventory()->addItem(new Item("Movement", 25, movement));
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
    int sel = 0;
    const int OPTIONS = 4;

    while (true) {
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        cout << "╔═════════════════════════════╗" << endl;
        cout << "║    SERVANT BATTLE ROYALE    ║" << endl;
        cout << "╠═════════════════════════════╣" << endl;
        cout << "║                             ║" << endl;

        const char* labels[OPTIONS] = {
            " Inventory ",
            " Shop ",
            " Battle Arena ",
            " Quit "
        };

        for (int i = 0; i < OPTIONS; i++) {
            if (i == sel) cout << "║ > " << labels[i];
            else          cout << "║   " << labels[i];

            int used = 5 + (int)string(labels[i]).size();
            for (int s = used; s < 30; s++) cout << " ";
            cout << " ║" << endl;
        }

        cout << "║                             ║" << endl;
        cout << "║ Gold: " << shop->getPlayerGold() << "g   ";
        int goldLen = (int)std::to_string(shop->getPlayerGold()).size();
        for (int p = 0; p < 18 - goldLen; p++) cout << " ";
        cout << "║" << endl;
        cout << "╚═════════════════════════════╝" << endl;
        cout << "W/S - navigate | Enter - select | ESC - quit" << endl;

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
            case 13:
                if (sel == 0) play();
                else if (sel == 1) playShop();
                else if (sel == 2) playBattleArena();
                else if (sel == 3) return;
                break;
            case 127:
            case 27:
                return;
            default:
                break;
        }
    }
}

// ─────────────────────────────────────────────
// Shop search input
// ─────────────────────────────────────────────

void Game::playSearchMenu()
{
    string query;

    while (true) {
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        cout << "=== SHOP SEARCH ===" << endl;
        cout << "Type item name, Enter to confirm, Backspace to delete, ESC to cancel" << endl;
        cout << endl;
        cout << "Search: " << query << "_" << endl;

        int key = (int)getSingleChar();

        if (key == 10 || key == 13) {
            shop->setSearchQuery(query);
            return;
        } else if (key == 27) {
            return;
        } else if (key == 127 || key == 8) {
            if (!query.empty()) query.pop_back();
        } else if (key >= 32 && key <= 126) {
            query += (char)key;
        }
    }
}

// ─────────────────────────────────────────────
// Sell confirmation screen
// ─────────────────────────────────────────────

void Game::showSellConfirmScreen(Item* item, int row, int col)
{
    if (item == nullptr) return;

    int pct = 80 + (std::rand() % 11);
    int sellPrice = (item->getPrice() * pct) / 100;

    while (true) {
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        cout << "╔══════════════════════════════════════════════╗" << endl;
        cout << "║                 SELL ITEM?                  ║" << endl;
        cout << "╠══════════════════════════════════════════════╣" << endl;
        cout << "║                                              ║" << endl;

        cout << "║ Item: " << item->getName();
        int used1 = 7 + (int)item->getName().size();
        for (int i = used1; i < 45; i++) cout << " ";
        cout << "║" << endl;

        cout << "║ Type: " << Item::typeToString(item->getType());
        int used2 = 7 + (int)Item::typeToString(item->getType()).size();
        for (int i = used2; i < 45; i++) cout << " ";
        cout << "║" << endl;

        cout << "║ Worth: " << item->getPrice() << "g";
        int used3 = 9 + (int)std::to_string(item->getPrice()).size();
        for (int i = used3; i < 45; i++) cout << " ";
        cout << "║" << endl;

        cout << "║ Sell: " << sellPrice << "g (" << pct << "%)";
        int used4 = 8 + (int)std::to_string(sellPrice).size() + 3 + (int)std::to_string(pct).size();
        for (int i = used4; i < 45; i++) cout << " ";
        cout << "║" << endl;

        cout << "║                                              ║" << endl;
        cout << "╠══════════════════════════════════════════════╣" << endl;
        cout << "║ Y - Confirm sell | N / ESC - Cancel         ║" << endl;
        cout << "╚══════════════════════════════════════════════╝" << endl;

        int key = (int)getSingleChar();
        if (key == 'y' || key == 'Y') {
            player->getInventory()->setItemAt(row, col, nullptr);
            shop->addGold(sellPrice);

            printf("\033[2J");
            printf("\033[H");
            fflush(stdout);

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

    const string& iname = item->getName();
    string itemTag;
    for (int k = 0; k < 3; k++) {
        itemTag += (k < (int)iname.size())
            ? (char)std::toupper((unsigned char)iname[k])
            : ' ';
    }

    while (true) {
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        cout << "Holding: " << item->getName()
             << " [" << item->getPrice() << "g | "
             << Item::rarityToString(item->getRarity()) << " | "
             << Item::typeToString(item->getType()) << "]" << endl;
        cout << endl;

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
            cout << "[EMPTY] Enter-Place | ESC-Auto-place | Q-Cancel buy" << endl;
        else
            cout << "[" << curSlot->getName() << "] Occupied! | Q-Cancel buy" << endl;

        int key = (int)getSingleChar();
        switch (key) {
            case 'a': case 'A':
                curCol = (curCol - 1 + BAG_COLS) % BAG_COLS;
                break;
            case 'd': case 'D':
                curCol = (curCol + 1) % BAG_COLS;
                break;
            case 'w': case 'W':
                curRow = (curRow - 1 + BAG_ROWS) % BAG_ROWS;
                break;
            case 's': case 'S':
                curRow = (curRow + 1) % BAG_ROWS;
                break;

            case 'q': case 'Q':
                shop->addGold(item->getPrice());
                shop->addItemToShop(item);
                return;

            case 10:
            case 13:
                if (!inv->getItemAt(curRow, curCol)) {
                    inv->setItemAt(curRow, curCol, item);
                    return;
                } else {
                    cout << "\nSlot occupied!" << endl;
                    cout << "Press any key..." << endl;
                    getSingleChar();
                }
                break;

            case 27:
                if (!inv->addItemAtRandomPosition(item)) {
                    shop->addGold(item->getPrice());
                    shop->addItemToShop(item);
                    cout << "\nInventory full! Purchase refunded." << endl;
                    cout << "Press any key..." << endl;
                    getSingleChar();
                }
                return;

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
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        bool filteredView =
            !shop->getSearchQuery().empty() ||
            shop->isRarityFilterEnabled() ||
            shop->isTypeFilterEnabled();

        if (filteredView) {
            shop->displayShopWithSearch();
        } else {
            shop->displayShop();
        }

        cout << endl;
        cout << "1-Common 2-Magic 3-Rare 4-Potion 5-Sword 6-Armor 7-Movement 0-Clear" << endl;
        cout << "F-Search  R-Restock  V-Sell items  E-Exit" << endl;

        userInput = (int)getSingleChar();

        switch (userInput) {
            case 'w': case 'W':
                if (filteredView) shop->selectPreviousFiltered(shop->getSearchResults());
                else shop->selectPrevious();
                break;

            case 's': case 'S':
                if (filteredView) shop->selectNextFiltered(shop->getSearchResults());
                else shop->selectNext();
                break;

            case 'b': case 'B': {
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

            case 'f': case 'F':
                playSearchMenu();
                break;

            case 'r': case 'R':
                shop->restockShop();
                cout << "\nShop restocked!" << endl;
                cout << "Press any key..." << endl;
                getSingleChar();
                break;

            case 'v': case 'V':
                sellItemsMenu();
                break;

            case '1':
                shop->setRarityFilter(common);
                break;
            case '2':
                shop->setRarityFilter(magic);
                break;
            case '3':
                shop->setRarityFilter(rare);
                break;
            case '4':
                shop->setTypeFilter(potion);
                break;
            case '5':
                shop->setTypeFilter(sword);
                break;
            case '6':
                shop->setTypeFilter(armor);
                break;
            case '7':
                shop->setTypeFilter(movement);
                break;
            case '0':
                shop->clearAllFilters();
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
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

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

        if (player->getInventory()->getIsDragging()) {
            cout << "\n[DRAGGING] G - Drop/Swap | K - Cancel drag | E - Exit" << endl;
        } else {
            cout << "\nControls: W/A/S/D - Navigate | H - Sell | G - Grab/Drop | K - Cancel drag | E - Exit" << endl;
        }

        userInput = (int)getSingleChar();

        switch (userInput) {
            case 'a': case 'A':
                player->getInventory()->setCurrentCol(
                    player->getInventory()->getCurrentCol() - 1);
                break;
            case 'd': case 'D':
                player->getInventory()->setCurrentCol(
                    player->getInventory()->getCurrentCol() + 1);
                break;
            case 'w': case 'W':
                player->getInventory()->setCurrentRow(
                    player->getInventory()->getCurrentRow() - 1);
                break;
            case 's': case 'S':
                player->getInventory()->setCurrentRow(
                    player->getInventory()->getCurrentRow() + 1);
                break;

            case 'g': case 'G':
                if (player->getInventory()->getIsDragging()) {
                    player->getInventory()->dropItem();
                } else {
                    player->getInventory()->startDrag();
                }
                break;

            case 'k': case 'K':
                player->getInventory()->cancelDrag();
                break;

            case 'h': case 'H': {
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
// Inventory
// ─────────────────────────────────────────────

void Game::play()
{
    int userInput = 0;
    bool showItemInfo = false;

    while (userInput != 127) { // Backspace to exit
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        cout << "Gold: " << shop->getPlayerGold() << "g" << endl;
        cout << endl;

        if (showItemInfo) {
            Item* currentItem = player->getInventory()->getCurrentItem();
            if (currentItem)
                player->getInventory()->displayWithItemInfo(currentItem);
            else
                player->getInventory()->displayWithEmptyInfo();
        } else {
            player->getInventory()->display();
        }

        cout << "\nYour Servant: " << player->getServant()->getName() << endl;
        player->getServant()->renderAscii();

        if (player->getInventory()->getIsDragging()) {
            cout << "\n[DRAGGING] G - Drop/Swap | K - Cancel drag | Backspace - Menu" << endl;
        } else {
            cout << "\nW/A/S/D - Move | G - Grab | T - Equip | I - Info | H - Sell | Backspace - Menu" << endl;
        }

        userInput = (int)getSingleChar();

        switch (userInput) {
            case 'a':
            case 'A':
                player->getInventory()->setCurrentCol(
                    player->getInventory()->getCurrentCol() - 1
                );
                showItemInfo = false;
                break;

            case 'd':
            case 'D':
                player->getInventory()->setCurrentCol(
                    player->getInventory()->getCurrentCol() + 1
                );
                showItemInfo = false;
                break;

            case 'w':
            case 'W':
                player->getInventory()->setCurrentRow(
                    player->getInventory()->getCurrentRow() - 1
                );
                showItemInfo = false;
                break;

            case 's':
            case 'S':
                player->getInventory()->setCurrentRow(
                    player->getInventory()->getCurrentRow() + 1
                );
                showItemInfo = false;
                break;

            case 'g':
            case 'G':
                if (player->getInventory()->getIsDragging()) {
                    player->getInventory()->dropItem();
                } else {
                    player->getInventory()->startDrag();
                }
                showItemInfo = false;
                break;

            case 'k':
            case 'K':
                player->getInventory()->cancelDrag();
                showItemInfo = false;
                break;

            case 'h':
            case 'H': {
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

            case 't':
            case 'T':
                if (!player->getInventory()->equipItem()) {
                    cout << "\nNo item here or equipment slots full!" << endl;
                    cout << "Press any key..." << endl;
                    getSingleChar();
                }
                showItemInfo = false;
                break;

            case 'i':
            case 'I':
                showItemInfo = !showItemInfo;
                break;

            case 127: // Backspace
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
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

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
        printf("\033[2J");
        printf("\033[H");
        fflush(stdout);

        cout << "=== DUEL ===" << endl;
        cout << myServant->getName() << " HP: " << pHP << "/" << pMaxHP << endl;
        myServant->renderAscii();
        cout << "\nVS\n\n";
        cout << en.getName() << " HP: " << eHP << "/" << en.getMaxHP() << endl;
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
                int dmg = pStrEff - eDurEff;
                if (dmg < 1) dmg = 1;
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

        cout << "\nEnemy's turn..." << endl;

        int hitChanceE = 75 + (eAgiEff - pAgiEff) * 2;
        if (hitChanceE < 10) hitChanceE = 10;
        if (hitChanceE > 95) hitChanceE = 95;

        int rollE = std::rand() % 100;
        if (rollE < hitChanceE) {
            int dmg = eStrEff - pDurEff;
            if (dmg < 1) dmg = 1;
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
