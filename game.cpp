#include "game.h"
#include "singleCharacter.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cctype>

using std::cout;
using std::endl;
using std::string;
using std::vector;

// =============================================================================
// File-scope helpers for side-by-side terminal rendering
// =============================================================================

static int visibleLen(const std::string& s) {
    int len = 0; bool esc = false;
    for (char c : s) {
        if (c == '\033')        esc = true;
        else if (esc && c=='m') esc = false;
        else if (!esc)          ++len;
    }
    return len;
}

static std::string padRight(const std::string& s, int w) {
    int v = visibleLen(s);
    return v >= w ? s : s + std::string(w - v, ' ');
}

static std::vector<std::string> splitLines(const std::string& s) {
    std::vector<std::string> lines;
    std::string cur;
    for (char c : s) {
        if (c == '\n') { lines.push_back(cur); cur.clear(); }
        else cur += c;
    }
    if (!cur.empty()) lines.push_back(cur);
    return lines;
}

// =============================================================================
// Accessors
// =============================================================================

Player* Game::getPlayer() const { return player; }
void    Game::setPlayer(Player* p) { player = p; }
Shop*   Game::getShop()   const { return shop; }

// =============================================================================
// Constructor / Destructor
// =============================================================================

Game::Game() {
    player        = new Player();
    itemGenerator = new ItemGenerator();
    shop          = new Shop();
    Servant myServant = Servant::randomServant();
    player->setServant(myServant);
    generateRandomInventory();
}

Game::~Game() {
    delete player;
    delete itemGenerator;
    delete shop;
}

// =============================================================================
// Utility
// =============================================================================

void Game::initializeSampleItems() {
    player->getInventory()->addItem(new Item("Sword",    50, sword));
    player->getInventory()->addItem(new Item("Potion",   10, potion));
    player->getInventory()->addItem(new Item("Armor",    80, armor));
    player->getInventory()->addItem(new Item("Movement", 25, movement));
}

void Game::generateRandomInventory() {
    vector<Item*> randomItems = itemGenerator->generateRandomItems(2, 5);
    for (Item* item : randomItems) {
        if (!player->getInventory()->addItemAtRandomPosition(item)) {
            delete item;
            break;
        }
    }
}

// =============================================================================
// Main Menu
// =============================================================================

void Game::showMainMenu() {
    int sel = 0;
    const int OPTIONS = 4;
    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        cout << "╔═════════════════════════════╗" << endl;
        cout << "║  SERVANT BATTLE ROYALE      ║" << endl;
        cout << "╠═════════════════════════════╣" << endl;
        cout << "║                             ║" << endl;
        const char* labels[OPTIONS] = {
            " Inventory   ", " Shop        ", " Battle Arena", " Quit        "
        };
        for (int i = 0; i < OPTIONS; i++) {
            if (i == sel) cout << "║  > " << labels[i];
            else          cout << "║    " << labels[i];
            int used = 5 + (int)string(labels[i]).size();
            for (int s = used; s < 30; s++) cout << ' ';
            cout << " ║" << endl;
        }
        cout << "║                             ║" << endl;
        cout << "║  Gold: " << shop->getPlayerGold() << "g";
        int goldLen = (int)std::to_string(shop->getPlayerGold()).size();
        for (int p = 0; p < 18 - goldLen; p++) cout << ' ';
        cout << "║" << endl;
        cout << "╚═════════════════════════════╝" << endl;
        cout << "W/S - navigate | Enter - select | ESC - quit" << endl;

        int key = (int)getSingleChar();
        switch (key) {
            case 'w': case 'W': sel = (sel - 1 + OPTIONS) % OPTIONS; break;
            case 's': case 'S': sel = (sel + 1) % OPTIONS; break;
            case 10:  case 13:
                if      (sel == 0) play();
                else if (sel == 1) playShop();
                else if (sel == 2) playBattleArena();
                else               return;
                break;
            case 127: case 27: return;
            default: break;
        }
    }
}

// =============================================================================
// Shop search input
// =============================================================================

void Game::playSearchMenu() {
    string query;
    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        cout << "=== SHOP SEARCH ===" << endl;
        cout << "Type item name, Enter to confirm, Backspace to delete, ESC to cancel" << endl;
        cout << endl << "Search: " << query << "_" << endl;
        int key = (int)getSingleChar();
        if      (key == 10 || key == 13)  { shop->setSearchQuery(query); return; }
        else if (key == 27)               { return; }
        else if (key == 127 || key == 8)  { if (!query.empty()) query.pop_back(); }
        else if (key >= 32 && key <= 126) { query += (char)key; }
    }
}

// =============================================================================
// Sell confirmation screen
// =============================================================================

void Game::showSellConfirmScreen(Item* item, int row, int col) {
    if (!item) return;
    int pct       = 80 + (std::rand() % 11);
    int sellPrice = (item->getPrice() * pct) / 100;
    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        cout << "╔══════════════════════════════════════════════╗" << endl;
        cout << "║                  SELL ITEM?                  ║" << endl;
        cout << "╠══════════════════════════════════════════════╣" << endl;
        cout << "║                                              ║" << endl;

        auto printRow = [](const string& label, const string& val) {
            string line = "║  " + label + val;
            int used = 4 + (int)label.size() + (int)val.size();
            for (int i = used; i < 46; i++) cout << ' ';
            cout << "║" << endl;
        };

        printRow("Item:   ", item->getName());
        printRow("Type:   ", Item::typeToString(item->getType()));
        printRow("Worth:  ", std::to_string(item->getPrice()) + "g");
        printRow("Sell:   ", std::to_string(sellPrice) + "g (" + std::to_string(pct) + "%)");

        cout << "║                                              ║" << endl;
        cout << "╠══════════════════════════════════════════════╣" << endl;
        cout << "║      Y - Confirm sell  |  N / ESC - Cancel   ║" << endl;
        cout << "╚══════════════════════════════════════════════╝" << endl;

        int key = (int)getSingleChar();
        if (key == 'y' || key == 'Y') {
            player->getInventory()->setItemAt(row, col, nullptr);
            shop->addGold(sellPrice);
            printf("\033[2J"); printf("\033[H"); fflush(stdout);
            cout << "Sold " << item->getName() << " for " << sellPrice
                 << "g (" << pct << "%)!" << endl;
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

// =============================================================================
// Manual item placement after purchase
// =============================================================================

void Game::placeItemMenu(Item* item) {
    Inventory* inv   = player->getInventory();
    const int  BAG_R = 2, BAG_C = 3;
    int curRow = 0, curCol = 0;
    const string& iname = item->getName();
    string itemTag;
    for (int k = 0; k < 3; k++)
        itemTag += (k < (int)iname.size()) ? (char)std::toupper((unsigned char)iname[k]) : ' ';

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        cout << "Holding: " << item->getName()
             << " [" << item->getPrice() << "g | "
             << Item::rarityToString(item->getRarity()) << " | "
             << Item::typeToString(item->getType()) << "]" << endl << endl;

        for (int i = 0; i < BAG_R; i++) {
            for (int j = 0; j < BAG_C; j++) {
                bool  isCursor = (i == curRow && j == curCol);
                Item* slot     = inv->getItemAt(i, j);
                cout << (isCursor ? "<" : "[");
                if (isCursor)   cout << itemTag;
                else if (!slot) cout << "   ";
                else            cout << inv->getItemDisplayStr(i, j);
                cout << (isCursor ? "> " : "] ");
            }
            cout << endl;
        }
        cout << endl;

        Item* curSlot = inv->getItemAt(curRow, curCol);
        if (!curSlot) cout << "[EMPTY] Enter-Place | ESC-Auto-place | Q-Cancel buy" << endl;
        else          cout << "[" << curSlot->getName() << "] Occupied! | Q-Cancel buy" << endl;

        int key = (int)getSingleChar();
        switch (key) {
            case 'a': case 'A': curCol = (curCol - 1 + BAG_C) % BAG_C; break;
            case 'd': case 'D': curCol = (curCol + 1) % BAG_C; break;
            case 'w': case 'W': curRow = (curRow - 1 + BAG_R) % BAG_R; break;
            case 's': case 'S': curRow = (curRow + 1) % BAG_R; break;
            case 'q': case 'Q':
                shop->addGold(item->getPrice());
                shop->addItemToShop(item);
                return;
            case 10: case 13:
                if (!inv->getItemAt(curRow, curCol)) {
                    inv->setItemAt(curRow, curCol, item); return;
                } else {
                    cout << "\nSlot occupied!\nPress any key..." << endl;
                    getSingleChar();
                }
                break;
            case 27:
                if (!inv->addItemAtRandomPosition(item)) {
                    shop->addGold(item->getPrice());
                    shop->addItemToShop(item);
                    cout << "\nInventory full! Purchase refunded.\nPress any key..." << endl;
                    getSingleChar();
                }
                return;
            default: break;
        }
    }
}

// =============================================================================
// Shop screen
// =============================================================================

void Game::playShop() {
    int userInput = 0;
    cout << "\n=== Welcome to the Shop! ===\nPress any key to continue..." << endl;
    getSingleChar();

    while (userInput != 'e' && userInput != 'E') {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        bool fv = !shop->getSearchQuery().empty() ||
                   shop->isRarityFilterEnabled()   ||
                   shop->isTypeFilterEnabled();
        if (fv) shop->displayShopWithSearch(); else shop->displayShop();
        cout << endl;
        cout << "1-Common 2-Magic 3-Rare  4-Potion 5-Sword 6-Armor 7-Movement 0-Clear" << endl;
        cout << "F-Search  R-Restock  V-Sell items  E-Exit" << endl;

        userInput = (int)getSingleChar();
        switch (userInput) {
            case 'w': case 'W':
                if (fv) shop->selectPreviousFiltered(shop->getSearchResults());
                else    shop->selectPrevious();
                break;
            case 's': case 'S':
                if (fv) shop->selectNextFiltered(shop->getSearchResults());
                else    shop->selectNext();
                break;
            case 'b': case 'B': {
                int   idx = shop->getSelectedIndex();
                Item* si  = shop->getSelectedItem();
                if (si) {
                    if (shop->getPlayerGold() < si->getPrice()) {
                        cout << "\nNot enough gold!\nPress any key..." << endl;
                        getSingleChar();
                    } else if (player->getInventory()->getEmptySlotCount() == 0) {
                        cout << "\nInventory is full!\nPress any key..." << endl;
                        getSingleChar();
                    } else {
                        Item* bought = shop->buyItemNoPlace(idx);
                        if (bought) placeItemMenu(bought);
                    }
                }
                break;
            }
            case 'f': case 'F': playSearchMenu(); break;
            case 'r': case 'R':
                shop->restockShop();
                cout << "\nShop restocked!\nPress any key..." << endl;
                getSingleChar();
                break;
            case 'v': case 'V': sellItemsMenu(); break;
            case '1': shop->setRarityFilter(common);   break;
            case '2': shop->setRarityFilter(magic);    break;
            case '3': shop->setRarityFilter(rare);     break;
            case '4': shop->setTypeFilter(potion);     break;
            case '5': shop->setTypeFilter(sword);      break;
            case '6': shop->setTypeFilter(armor);      break;
            case '7': shop->setTypeFilter(movement);   break;
            case '0': shop->clearAllFilters();         break;
            default: break;
        }
    }
}

// =============================================================================
// Sell items menu
// =============================================================================

void Game::sellItemsMenu() {
    int userInput = 0;
    while (userInput != 'e' && userInput != 'E') {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        cout << "=== SELL ITEMS ===" << endl;
        cout << "Your Gold: " << shop->getPlayerGold() << "g" << endl << endl;
        player->getInventory()->display();

        Item* cur = player->getInventory()->getCurrentItem();
        if (cur)
            cout << "\nSelected: " << cur->getName()
                 << " | Sell value: "
                 << (int)(cur->getPrice() * 0.80) << "-"
                 << (int)(cur->getPrice() * 0.90) << "g" << endl;
        else
            cout << "\nNo item at cursor." << endl;

        if (player->getInventory()->getIsDragging())
            cout << "\n[DRAGGING] G-Drop/Swap | K-Cancel | E-Exit" << endl;
        else
            cout << "\nW/A/S/D-Navigate | H-Sell | G-Grab | K-Cancel drag | E-Exit" << endl;

        userInput = (int)getSingleChar();
        switch (userInput) {
            case 'a': case 'A':
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() - 1); break;
            case 'd': case 'D':
                player->getInventory()->setCurrentCol(player->getInventory()->getCurrentCol() + 1); break;
            case 'w': case 'W':
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() - 1); break;
            case 's': case 'S':
                player->getInventory()->setCurrentRow(player->getInventory()->getCurrentRow() + 1); break;
            case 'g': case 'G':
                if (player->getInventory()->getIsDragging()) player->getInventory()->dropItem();
                else                                         player->getInventory()->startDrag();
                break;
            case 'k': case 'K': player->getInventory()->cancelDrag(); break;
            case 'h': case 'H': {
                if (player->getInventory()->getIsDragging()) {
                    cout << "\nCannot sell while dragging!\nPress any key..." << endl;
                    getSingleChar(); break;
                }
                Item* it = player->getInventory()->getCurrentItem();
                if (it) showSellConfirmScreen(it,
                            player->getInventory()->getCurrentRow(),
                            player->getInventory()->getCurrentCol());
                else { cout << "\nNo item to sell!\nPress any key..." << endl; getSingleChar(); }
                break;
            }
            default: break;
        }
    }
}

// =============================================================================
// INVENTORY SCREEN  (redesigned: side-by-side layout)
// =============================================================================

void Game::play() {
    int  userInput    = 0;
    bool showItemInfo = false;
    bool showStats    = false;
    const int LEFT_W  = 42;

    while (userInput != 127) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        Servant*   sv  = player->getServant();
        Inventory* inv = player->getInventory();

        // ── LEFT PANEL ──────────────────────────────────────────────────
        vector<string> left;

        string goldStr = " \033[93mGold:\033[0m " +
                         std::to_string(shop->getPlayerGold()) + "g";
        string locStr  = (inv->getCurrentCol() >= 3)
                         ? "  \033[93m[EQUIP]\033[0m"
                         : "  \033[97m[BAG]\033[0m";
        left.push_back(padRight(goldStr, LEFT_W / 2) + locStr);
        left.push_back("");

        auto bagLines   = inv->renderBagLines();
        auto equipLines = inv->renderEquipLines();
        int  gridRows   = (int)std::max(bagLines.size(), equipLines.size());
        for (int i = 0; i < gridRows; i++) {
            string bl = (i < (int)bagLines.size())
                        ? bagLines[i]  : string(21, ' ');
            string el = (i < (int)equipLines.size())
                        ? equipLines[i] : string(15, ' ');
            left.push_back(padRight(bl, 23) + el);
        }
        left.push_back("");

        Item* cur = inv->getCurrentItem();
        if (showItemInfo) {
            if (cur) {
                string rCol = (cur->getRarity() == rare)  ? "\033[93m"
                            : (cur->getRarity() == magic) ? "\033[96m"
                            : "\033[97m";
                left.push_back(" " + rCol + cur->getName() + "\033[0m");
                left.push_back(" Type:   " + Item::typeToString(cur->getType()));
                left.push_back(" Rarity: " + Item::rarityToString(cur->getRarity()));
                left.push_back(" Price:  " + std::to_string(cur->getPrice()) + "g");
                left.push_back(" Effect: +" + std::to_string(cur->getPercentValue()) + "%");
            } else {
                left.push_back(" \033[90m[empty slot]\033[0m");
            }
        }

        // ── RIGHT PANEL ─────────────────────────────────────────────────
        vector<string> right;

        string seriesTag = (sv->getSeries() == Series::StayNight)
                           ? "\033[34mFate/stay night\033[0m"
                           : "\033[31mFate/Zero\033[0m";
        right.push_back(" \033[93m" + sv->getName() + "\033[0m  " + seriesTag);
        right.push_back(" \033[90m────────────────────────────\033[0m");

        for (auto& al : splitLines(sv->getAsciiArt()))
            right.push_back(" " + al);

        if (showStats) {
            int bHP  = sv->getMaxHP(),      eHP  = player->getEffectiveMaxHP();
            int bSTR = sv->getStrength(),    eSTR = player->getEffectiveStrength();
            int bDUR = sv->getDurability(),  eDUR = player->getEffectiveDurability();
            int bAGI = sv->getAgility(),     eAGI = player->getEffectiveAgility();

            auto statLine = [](const string& lbl, int base, int eff) -> string {
                string s = " \033[97m" + lbl + "\033[0m " + std::to_string(base);
                if (eff != base)
                    s += " \033[92m(+" + std::to_string(eff - base) + ")\033[0m";
                return s;
            };

            right.push_back(" \033[90m────────────────────────────\033[0m");
            right.push_back(" \033[96mBase Stats\033[0m");
            right.push_back(statLine("HP: ", bHP,  eHP));
            right.push_back(statLine("STR:", bSTR, eSTR));
            right.push_back(statLine("DUR:", bDUR, eDUR));
            right.push_back(statLine("AGI:", bAGI, eAGI));

            const auto& nps = sv->getNPs();
            if (!nps.empty()) {
                right.push_back("");
                right.push_back(" \033[96mNoble Phantasms\033[0m");
                for (std::size_t ni = 0; ni < nps.size(); ++ni) {
                    string col = sv->isNPAvailable(ni) ? "\033[93m" : "\033[90m";
                    right.push_back(" " + col + nps[ni].name + "\033[0m");
                }
            }
        }

        // ── PRINT SIDE BY SIDE ───────────────────────────────────────────
        int maxRows = (int)std::max(left.size(), right.size());
        for (int i = 0; i < maxRows; i++) {
            string l = (i < (int)left.size())  ? left[i]  : "";
            string r = (i < (int)right.size()) ? right[i] : "";
            cout << padRight(l, LEFT_W) << r << "\n";
        }

        cout << "\n";
        if (inv->getIsDragging())
            cout << " \033[93m[DRAGGING]\033[0m G-Drop/Swap  K-Cancel  Backspace-Menu\n";
        else
            cout << " W/A/S/D-Move  G-Grab  T-Equip  I-Info  "
                    "F-Stats  H-Sell  P-Shop  Backspace-Menu\n";

        userInput = (int)getSingleChar();
        switch (userInput) {
            case 'a': case 'A':
                inv->setCurrentCol(inv->getCurrentCol() - 1); showItemInfo = false; break;
            case 'd': case 'D':
                inv->setCurrentCol(inv->getCurrentCol() + 1); showItemInfo = false; break;
            case 'w': case 'W':
                inv->setCurrentRow(inv->getCurrentRow() - 1); showItemInfo = false; break;
            case 's': case 'S':
                inv->setCurrentRow(inv->getCurrentRow() + 1); showItemInfo = false; break;
            case 'g': case 'G':
                if (inv->getIsDragging()) inv->dropItem();
                else                      inv->startDrag();
                showItemInfo = false; break;
            case 'k': case 'K':
                inv->cancelDrag(); showItemInfo = false; break;
            case 'h': case 'H': {
                if (inv->getIsDragging()) {
                    cout << "\nCannot sell while dragging!\nPress any key...\n";
                    getSingleChar(); break;
                }
                Item* it = inv->getCurrentItem();
                if (it) showSellConfirmScreen(it, inv->getCurrentRow(), inv->getCurrentCol());
                else { cout << "\nNo item here!\nPress any key...\n"; getSingleChar(); }
                showItemInfo = false; break;
            }
            case 't': case 'T':
                if (!inv->equipItem()) {
                    cout << "\nNo item here or equipment slots full!\nPress any key...\n";
                    getSingleChar();
                }
                showItemInfo = false; break;
            case 'i': case 'I': showItemInfo = !showItemInfo; break;
            case 'f': case 'F': showStats    = !showStats;    break;
            case 'p': case 'P': playShop();                   break;
            case 127: return;
            default: break;
        }
    }
}

// =============================================================================
// Battle Arena
// =============================================================================

void Game::playBattleArena() {
    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        Servant* myServant = player->getServant();

        auto fullPool = Servant::enemyPoolForSeries(myServant->getSeries());
        vector<Servant> pool;
        for (auto& s : fullPool) {
            if (s.getName() != myServant->getName() &&
                defeatedEnemies.find(s.getName()) == defeatedEnemies.end())
                pool.push_back(s);
        }

        cout << "=== BATTLE ARENA ===" << endl;
        cout << "Your Servant: " << myServant->getName() << endl;
        myServant->renderAscii();

        if (pool.empty()) {
            cout << "\n\033[93mAll servants in this Holy Grail War have been defeated!\033[0m" << endl;
            cout << "You are the last Master standing." << endl;
            cout << "\nPress any key to return..." << endl;
            getSingleChar(); return;
        }

        cout << "\nRemaining enemies: " << pool.size() << endl;
        cout << "Press B to enter battle, any other key to exit..." << endl;
        int key = (int)getSingleChar();
        if (key != 'b' && key != 'B') return;

        int   idx = std::rand() % (int)pool.size();
        Enemy enemy(pool[idx]);
        bool  won = battleOneEnemy(enemy);
        if (won) defeatedEnemies.insert(enemy.getServant().getName());
    }
}

bool Game::battleOneEnemy(Enemy& enemy) {
    Servant*  myServant = player->getServant();
    Servant&  en        = enemy.getServant();
    myServant->resetForNewBattle();
    en.resetForNewBattle();

    int pMaxHP  = player->getEffectiveMaxHP();
    int pHP     = pMaxHP;
    int eHP     = en.getMaxHP();
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
        cout << "\nVS\n" << endl;
        cout << en.getName() << "  HP: " << eHP << "/" << en.getMaxHP() << endl;
        en.renderAscii();

        cout << "\nChoose action:" << endl;
        cout << "  A - Basic attack" << endl;
        const auto& nps = myServant->getNPs();
        for (std::size_t i = 0; i < nps.size(); ++i) {
            cout << "  " << (i + 1) << " - NP: " << nps[i].name;
            if (!myServant->isNPAvailable(i)) cout << "  \033[90m(used)\033[0m";
            cout << endl;
        }
        cout << "  E - Escape" << endl;

        int  key      = (int)getSingleChar();
        bool usedTurn = false;

        if (key == 'e' || key == 'E') {
            cout << "\nYou fled from battle.\nPress any key..." << endl;
            getSingleChar(); return false;

        } else if (key == 'a' || key == 'A') {
            int hit = std::max(10, std::min(95, 75 + (pAgiEff - eAgiEff) * 2));
            if (std::rand() % 100 < hit) {
                int dmg = pStrEff - (eDurEff / 2);
                if (dmg < 3) dmg = 3;
                eHP -= dmg; if (eHP < 0) eHP = 0;
                cout << "\nYou strike " << en.getName()
                     << " for \033[97m" << dmg << "\033[0m damage!" << endl;
            } else {
                cout << "\nYour attack is deflected!" << endl;
            }
            usedTurn = true;

        } else if (key >= '1' && key <= '9') {
            std::size_t ni = static_cast<std::size_t>(key - '1');
            if (ni < nps.size() && myServant->isNPAvailable(ni)) {
                int hit = std::max(20, std::min(99, 85 + (pAgiEff - eAgiEff)));
                cout << "\n\033[93m=== NOBLE PHANTASM ===\033[0m" << endl;
                cout << "\033[97m" << nps[ni].name << "\033[0m" << endl;
                cout << nps[ni].description << endl;
                if (std::rand() % 100 < hit) {
                    int dmg = myServant->noblePhantasmDamage(ni,
                                  pStrEff - myServant->getStrength(), eDurEff / 2);
                    dmg /= 2; if (dmg < 12) dmg = 12;
                    eHP -= dmg; if (eHP < 0) eHP = 0;
                    myServant->markNPUsed(ni);
                    cout << "Hits for \033[93m" << dmg << "\033[0m damage!" << endl;
                } else {
                    myServant->markNPUsed(ni);
                    cout << "The Noble Phantasm was neutralised!" << endl;
                }
                usedTurn = true;
            }
        }

        if (!usedTurn) cout << "\nYou hesitate — your turn is lost." << endl;

        if (eHP <= 0) {
            cout << "\n\033[92m" << en.getName() << " has been defeated!\033[0m" << endl;
            cout << "Press any key..." << endl;
            getSingleChar(); return true;
        }

        cout << "\n" << en.getName() << "'s turn..." << endl;
        bool enemyUsedNP = false;
        const auto& eNPs = en.getNPs();
        for (std::size_t i = 0; i < eNPs.size(); ++i) {
            if (en.isNPAvailable(i) && (std::rand() % 4 == 0)) {
                int dmg = en.noblePhantasmDamage(i, 0, pDurEff / 2);
                dmg /= 2; if (dmg < 12) dmg = 12;
                pHP -= dmg; if (pHP < 0) pHP = 0;
                en.markNPUsed(i);
                cout << "\033[91m=== ENEMY NOBLE PHANTASM ===\033[0m" << endl;
                cout << "\033[97m" << eNPs[i].name << "\033[0m" << endl;
                cout << eNPs[i].description << endl;
                cout << "Deals \033[91m" << dmg << "\033[0m damage to you!" << endl;
                enemyUsedNP = true; break;
            }
        }
        if (!enemyUsedNP) {
            int hit = std::max(10, std::min(95, 75 + (eAgiEff - pAgiEff) * 2));
            if (std::rand() % 100 < hit) {
                int dmg = eStrEff - (pDurEff / 2);
                if (dmg < 3) dmg = 3;
                pHP -= dmg; if (pHP < 0) pHP = 0;
                cout << en.getName() << " strikes you for \033[91m" << dmg << "\033[0m damage!" << endl;
            } else {
                cout << en.getName() << "'s attack misses!" << endl;
            }
        }

        if (pHP <= 0) {
            cout << "\n\033[91mYour servant has fallen...\033[0m" << endl;
            cout << "Press any key..." << endl;
            getSingleChar(); return false;
        }

        cout << "\nPress any key for next round..." << endl;
        getSingleChar();
    }
    return false;
}
