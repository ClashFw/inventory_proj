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

// ANSI colour shorthands used throughout UI
#define C_RESET     "\033[0m"
#define C_GOLD      "\033[38;5;220m"
#define C_DGOLD     "\033[38;5;178m"
#define C_LGOLD     "\033[38;5;229m"
#define C_SILVER    "\033[38;5;250m"
#define C_WHITE     "\033[97m"
#define C_DIM       "\033[38;5;240m"
#define C_CYAN      "\033[96m"
#define C_BLUE      "\033[38;5;39m"
#define C_RED       "\033[38;5;160m"
#define C_BRED      "\033[91m"
#define C_GREEN     "\033[92m"
#define C_MGENTA    "\033[38;5;141m"
#define C_PURPLE    "\033[38;5;129m"
#define C_ORANGE    "\033[38;5;214m"

// Box-drawing
#define BD_H  "\u2500"
#define BD_V  "\u2502"
#define BD_TL "\u250c"
#define BD_TR "\u2510"
#define BD_BL "\u2514"
#define BD_BR "\u2518"
#define BD_LT "\u251c"
#define BD_RT "\u2524"
#define BD_TT "\u252c"
#define BD_BT "\u2534"
#define BD_CR "\u253c"

// Double box-drawing
#define BD_DH  "\u2550"
#define BD_DV  "\u2551"
#define BD_DTL "\u2554"
#define BD_DTR "\u2557"
#define BD_DBL "\u255a"
#define BD_DBR "\u255d"
#define BD_DLT "\u2560"
#define BD_DRT "\u2563"
#define BD_DTT "\u2566"
#define BD_DBT "\u2569"

// =============================================================================
// File-scope helpers for side-by-side terminal rendering
// =============================================================================

static int visibleLen(const std::string& s) {
    int len = 0; bool esc = false;
    for (char c : s) {
        if (c == '\033')        esc = true;
        else if (esc && c=='m') esc = false;
        else if (!esc) {
            // Count UTF-8 multibyte start bytes as 1 visible char each;
            // continuation bytes (10xxxxxx) are skipped
            if ((unsigned char)c < 0x80 || (unsigned char)c >= 0xC0) ++len;
        }
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

// Repeat a string n times
static std::string rep(const std::string& s, int n) {
    std::string out; for (int i = 0; i < n; i++) out += s; return out;
}

// FGO-style horizontal rule: ── TITLE ──────────────
static std::string fgoRule(const std::string& title, int totalW, const std::string& col) {
    int tv = visibleLen(title);
    int sides = totalW - tv - 2; // 1 space each side
    int left  = sides / 2;
    int right = sides - left;
    return col + rep(BD_H, left) + " " + C_WHITE + title + col + " " + rep(BD_H, right) + C_RESET;
}

// FGO stat rank bar (A/B/C/D/E style with filled pips)
static std::string rankBar(int val, int maxVal, int pips) {
    int filled = (maxVal > 0) ? (val * pips / maxVal) : 0;
    std::string bar = C_GOLD;
    for (int i = 0; i < pips; i++)
        bar += (i < filled) ? "\u25a0" : (C_DIM "\u25a1" C_GOLD);
    bar += C_RESET;
    return bar;
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
// Main Menu  —  FGO command-room style
// =============================================================================

void Game::showMainMenu() {
    int sel = 0;
    const int OPTIONS = 4;
    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        // Header
        cout << C_GOLD BD_DTL << rep(BD_DH, 31) << BD_DTR C_RESET << "\n";
        cout << C_GOLD BD_DV C_LGOLD "  ⚔  HOLY GRAIL WAR  •  MASTER HQ  "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DLT << rep(BD_DH, 31) << BD_DRT C_RESET << "\n";

        // Menu entries
        const char* labels[OPTIONS] = {
            "  Inventory", "  Shop     ", "  Battle   ", "  Quit     "
        };
        const char* icons[OPTIONS]  = { "\u2696", "\u2741", "\u2694", "\u00d7" };
        const char* cols[OPTIONS]   = { C_CYAN, C_GOLD, C_BRED, C_DIM };
        for (int i = 0; i < OPTIONS; i++) {
            bool active = (i == sel);
            std::string prefix = active
                ? (std::string(C_GOLD) + BD_DV + C_GOLD + "  \u25b6 ")
                : (std::string(C_GOLD) + BD_DV + C_DIM  + "    ");
            cout << prefix
                 << cols[i] << icons[i] << "  "
                 << (active ? C_WHITE : C_DIM)
                 << labels[i];
            // pad
            int used = 8 + (int)string(labels[i]).size();
            for (int s = used; s < 30; s++) cout << ' ';
            cout << C_GOLD << BD_DV << C_RESET << "\n";
        }

        // Footer
        cout << C_GOLD BD_DLT << rep(BD_DH, 31) << BD_DRT C_RESET << "\n";
        cout << C_GOLD BD_DV C_DIM " Gold: " C_GOLD
             << shop->getPlayerGold() << "g";
        int gl = (int)std::to_string(shop->getPlayerGold()).size() + 7;
        for (int i = gl; i < 30; i++) cout << ' ';
        cout << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DBL << rep(BD_DH, 31) << BD_DBR C_RESET << "\n";
        cout << C_DIM "  W/S navigate  " BD_V "  Enter select  " BD_V "  Esc quit" C_RESET << "\n";

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
        cout << C_GOLD << fgoRule("SHOP SEARCH", 44, C_DGOLD) << C_RESET << "\n\n";
        cout << C_DIM "Type item name  " BD_V "  Enter confirm  " BD_V "  Esc cancel" C_RESET "\n\n";
        cout << C_WHITE "  Search: " C_CYAN << query << C_DIM "_" C_RESET << "\n";
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

        cout << C_DGOLD BD_DTL << rep(BD_DH, 46) << BD_DTR C_RESET << "\n";
        cout << C_DGOLD BD_DV C_GOLD "            ❁  SELL ITEM?  ❁            "
             << C_DGOLD BD_DV C_RESET << "\n";
        cout << C_DGOLD BD_DLT << rep(BD_DH, 46) << BD_DRT C_RESET << "\n";

        auto printRow = [](const string& lbl, const string& val, const string& vc) {
            cout << C_DGOLD BD_DV C_DIM "  " C_SILVER << lbl
                 << vc << val;
            int used = 4 + (int)lbl.size() + (int)val.size();
            for (int i = used; i < 45; i++) cout << ' ';
            cout << C_DGOLD BD_DV C_RESET << "\n";
        };

        printRow("Item:   ", item->getName(),                        C_WHITE);
        printRow("Type:   ", Item::typeToString(item->getType()),    C_CYAN);
        printRow("Worth:  ", std::to_string(item->getPrice()) + "g", C_GOLD);
        printRow("Sell:   ", std::to_string(sellPrice) + "g (" +
                             std::to_string(pct) + "%)",             C_GREEN);

        cout << C_DGOLD BD_DLT << rep(BD_DH, 46) << BD_DRT C_RESET << "\n";
        cout << C_DGOLD BD_DV C_DIM
             "     " C_GREEN "Y" C_DIM " - Confirm sell   "
             BD_V "   " C_RED "N / Esc" C_DIM " - Cancel    "
             << C_DGOLD BD_DV C_RESET << "\n";
        cout << C_DGOLD BD_DBL << rep(BD_DH, 46) << BD_DBR C_RESET << "\n";

        int key = (int)getSingleChar();
        if (key == 'y' || key == 'Y') {
            player->getInventory()->setItemAt(row, col, nullptr);
            shop->addGold(sellPrice);
            printf("\033[2J"); printf("\033[H"); fflush(stdout);
            cout << C_GREEN "  Sold " C_WHITE << item->getName()
                 << C_GREEN " for " C_GOLD << sellPrice
                 << "g" C_DIM " (" << pct << "%)" C_RESET << "\n";
            cout << C_DIM "  Treasury: " C_GOLD << shop->getPlayerGold()
                 << "g" C_RESET << "\n\n";
            cout << C_DIM "  Press any key..." C_RESET << "\n";
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
        cout << C_GOLD "  Holding: " C_WHITE << item->getName()
             << C_DIM "  [" C_GOLD << item->getPrice() << "g"
             << C_DIM " | " C_CYAN << Item::rarityToString(item->getRarity())
             << C_DIM " | " C_SILVER << Item::typeToString(item->getType())
             << C_DIM "]" C_RESET << "\n\n";

        for (int i = 0; i < BAG_R; i++) {
            cout << "  ";
            for (int j = 0; j < BAG_C; j++) {
                bool  isCursor = (i == curRow && j == curCol);
                Item* slot     = inv->getItemAt(i, j);
                cout << (isCursor ? C_GOLD "<" : C_DIM "[");
                if (isCursor)   cout << C_WHITE << itemTag;
                else if (!slot) cout << "   ";
                else            cout << inv->getItemDisplayStr(i, j);
                cout << (isCursor ? ">" : "]") << C_RESET " ";
            }
            cout << "\n";
        }
        cout << "\n";

        Item* curSlot = inv->getItemAt(curRow, curCol);
        if (!curSlot)
            cout << C_DIM "  [EMPTY] " C_GREEN "Enter" C_DIM "-Place  "
                    C_SILVER "Esc" C_DIM "-Auto  "
                    C_RED "Q" C_DIM "-Cancel" C_RESET "\n";
        else
            cout << C_RED "  [" C_WHITE << curSlot->getName()
                 << C_RED "] Occupied  " C_RED "Q" C_DIM "-Cancel" C_RESET "\n";

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
                    cout << C_RED "\n  Slot occupied!" C_RESET "\n  Press any key...\n";
                    getSingleChar();
                }
                break;
            case 27:
                if (!inv->addItemAtRandomPosition(item)) {
                    shop->addGold(item->getPrice());
                    shop->addItemToShop(item);
                    cout << C_RED "\n  Inventory full! Purchase refunded." C_RESET "\n  Press any key...\n";
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
    cout << C_GOLD "\n  ❁  Welcome to the Shop, Master!  ❁" C_RESET "\n";
    cout << C_DIM "  Press any key to continue..." C_RESET "\n";
    getSingleChar();

    while (userInput != 'e' && userInput != 'E') {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);
        bool fv = !shop->getSearchQuery().empty() ||
                   shop->isRarityFilterEnabled()   ||
                   shop->isTypeFilterEnabled();
        if (fv) shop->displayShopWithSearch(); else shop->displayShop();
        cout << "\n";
        cout << C_DIM "  1-Common  2-Magic  3-Rare  4-Potion  5-Sword  6-Armor  7-Move  0-Clear\n";
        cout << "  F-Search  R-Restock  V-Sell  B-Buy  W/S-Navigate  E-Exit" C_RESET "\n";

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
                        cout << C_RED "\n  Not enough gold!" C_RESET "\n  Press any key...\n";
                        getSingleChar();
                    } else if (player->getInventory()->getEmptySlotCount() == 0) {
                        cout << C_RED "\n  Inventory is full!" C_RESET "\n  Press any key...\n";
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
                cout << C_GREEN "\n  Shop restocked!" C_RESET "\n  Press any key...\n";
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
        cout << C_GOLD << fgoRule("SELL ITEMS", 38, C_DGOLD) << C_RESET << "\n";
        cout << C_DIM "  Treasury: " C_GOLD << shop->getPlayerGold() << "g" C_RESET << "\n\n";
        player->getInventory()->display();

        Item* cur = player->getInventory()->getCurrentItem();
        if (cur)
            cout << C_DIM "  Selected: " C_WHITE << cur->getName()
                 << C_DIM "  Sell value: " C_GOLD
                 << (int)(cur->getPrice() * 0.80) << "-"
                 << (int)(cur->getPrice() * 0.90) << "g" C_RESET << "\n";
        else
            cout << C_DIM "  No item at cursor." C_RESET << "\n";

        if (player->getInventory()->getIsDragging())
            cout << C_GOLD "  [DRAGGING] " C_DIM "G-Drop  K-Cancel  E-Exit" C_RESET << "\n";
        else
            cout << C_DIM "  W/A/S/D-Navigate  H-Sell  G-Grab  K-Cancel  E-Exit" C_RESET << "\n";

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
                    cout << C_RED "\n  Cannot sell while dragging!" C_RESET "\n  Press any key...\n";
                    getSingleChar(); break;
                }
                Item* it = player->getInventory()->getCurrentItem();
                if (it) showSellConfirmScreen(it,
                            player->getInventory()->getCurrentRow(),
                            player->getInventory()->getCurrentCol());
                else { cout << C_RED "\n  No item to sell!" C_RESET "\n  Press any key...\n"; getSingleChar(); }
                break;
            }
            default: break;
        }
    }
}

// =============================================================================
// INVENTORY SCREEN  —  FGO / Visual Novel Style
// =============================================================================

// Helper: render one NP card in the right panel
static void pushNPCard(vector<string>& panel, std::size_t idx,
                       const NoblePhantasm& np, bool available, int panelW) {
    // Title line
    std::string avail = available ? (C_GOLD "\u25cf READY") : (C_DIM "\u25cb USED");
    panel.push_back(
        C_DIM " " + std::string(BD_TL) + rep(BD_H, 3) + C_RESET +
        " " C_LGOLD + np.name + C_RESET +
        "  " + avail + C_RESET
    );
    // Stats line: Base DMG / Scale
    panel.push_back(
        C_DIM "  " + std::string(BD_V) + "  "
        C_DIM "Dmg: " C_BRED + std::to_string(np.baseDamage) +
        C_DIM "  Scale: " C_ORANGE + std::to_string((int)(np.damageScale * 10)) + "x" +
        C_RESET
    );
    // Description — word-wrap to ~panelW - 5 chars
    int wrapW = panelW - 5;
    std::string desc = np.description;
    while ((int)desc.size() > wrapW) {
        int cut = wrapW;
        while (cut > 0 && desc[cut] != ' ') cut--;
        if (cut == 0) cut = wrapW;
        panel.push_back(C_DIM "  " + std::string(BD_V) + "  " C_SILVER + desc.substr(0, cut) + C_RESET);
        desc = desc.substr(cut + 1);
    }
    if (!desc.empty())
        panel.push_back(C_DIM "  " + std::string(BD_V) + "  " C_SILVER + desc + C_RESET);
    panel.push_back(C_DIM "  " + rep(BD_H, 28) + C_RESET);
}

void Game::play() {
    int  userInput    = 0;
    bool showItemInfo = false;
    bool showStats    = false;
    const int LEFT_W  = 44;

    while (userInput != 127) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        Servant*   sv  = player->getServant();
        Inventory* inv = player->getInventory();
        bool inEquip   = (inv->getCurrentCol() >= 3);

        // ── LEFT PANEL ──────────────────────────────────────────────────
        vector<string> left;

        // Header bar
        left.push_back(
            C_GOLD " ❁ " C_WHITE "INVENTORY" C_GOLD "  │  "
            C_DIM "Gold: " C_GOLD + std::to_string(shop->getPlayerGold()) + "g"
            + C_DIM "  │  "
            + (inEquip ? C_GOLD "[EQUIP ZONE]" : C_SILVER "[BAG]") + C_RESET
        );
        left.push_back(C_DIM + rep(BD_H, LEFT_W) + C_RESET);
        left.push_back("");

        // Bag + Equip grids
        auto bagLines   = inv->renderBagLines();
        auto equipLines = inv->renderEquipLines();
        int  gridRows   = (int)std::max(bagLines.size(), equipLines.size());
        for (int i = 0; i < gridRows; i++) {
            string bl = (i < (int)bagLines.size())   ? bagLines[i]   : string(21, ' ');
            string el = (i < (int)equipLines.size()) ? equipLines[i] : string(15, ' ');
            left.push_back(padRight(bl, 25) + el);
        }
        left.push_back("");

        // Item info panel
        Item* cur = inv->getCurrentItem();
        if (showItemInfo) {
            left.push_back(C_DIM + rep(BD_H, LEFT_W) + C_RESET);
            if (cur) {
                const char* rCol = (cur->getRarity() == rare)  ? C_GOLD
                                 : (cur->getRarity() == magic) ? C_CYAN
                                 : C_WHITE;
                left.push_back(string(" ") + rCol + cur->getName() + C_RESET);
                left.push_back(C_DIM "  Type:   " C_SILVER + Item::typeToString(cur->getType()) + C_RESET);
                left.push_back(C_DIM "  Rarity: " C_CYAN   + Item::rarityToString(cur->getRarity()) + C_RESET);
                left.push_back(C_DIM "  Price:  " C_GOLD   + std::to_string(cur->getPrice()) + "g" + C_RESET);
                left.push_back(C_DIM "  Effect: " C_GREEN  + "+" + std::to_string(cur->getPercentValue()) + "%" + C_RESET);
            } else {
                left.push_back(C_DIM "  [empty slot]" C_RESET);
            }
        }

        // Controls hint
        left.push_back("");
        left.push_back(C_DIM + rep(BD_H, LEFT_W) + C_RESET);
        if (inv->getIsDragging()) {
            left.push_back(C_GOLD " [DRAGGING] "
                C_DIM "G" C_SILVER "-Drop  "
                C_DIM "K" C_SILVER "-Cancel  "
                C_DIM "Bksp" C_SILVER "-Menu" C_RESET);
        } else {
            left.push_back(
                C_DIM " WASD" C_SILVER "-Move  "
                C_DIM "G" C_SILVER "-Grab  "
                C_DIM "T" C_SILVER + std::string(inEquip ? "-Unequip  " : "-Equip  ")
                + C_DIM "I" C_SILVER "-Info" C_RESET);
            left.push_back(
                C_DIM " F" C_SILVER "-Stats  "
                C_DIM "H" C_SILVER "-Sell  "
                C_DIM "P" C_SILVER "-Shop  "
                C_DIM "Bksp" C_SILVER "-Menu" C_RESET);
        }

        // ── RIGHT PANEL ─────────────────────────────────────────────────
        vector<string> right;

        // Servant name + series badge
        bool isZero = (sv->getSeries() == Series::Zero);
        string seriesBadge = isZero
            ? (C_RED "\u2605 Fate/Zero" C_RESET)
            : (std::string(C_BLUE) + "\u2605 Fate/stay night" + C_RESET);
        right.push_back(" " C_GOLD "\u2694  " C_WHITE + sv->getName() + C_RESET);
        right.push_back("    " + seriesBadge);
        right.push_back(" " C_DIM + rep(BD_H, 30) + C_RESET);
        right.push_back("");

        // ASCII art
        for (auto& al : splitLines(sv->getAsciiArt()))
            right.push_back(" " + al);

        // Stats panel
        if (showStats) {
            int bHP  = sv->getMaxHP(),     eHP  = player->getEffectiveMaxHP();
            int bSTR = sv->getStrength(),   eSTR = player->getEffectiveStrength();
            int bDUR = sv->getDurability(), eDUR = player->getEffectiveDurability();
            int bAGI = sv->getAgility(),    eAGI = player->getEffectiveAgility();

            // Reference maxima for rank bars
            const int MAX_HP  = 220;
            const int MAX_STAT = 32;

            auto statLine = [&](const string& lbl, int base, int eff, int maxV) -> string {
                string bonus = (eff != base)
                    ? (C_GREEN "  +" + std::to_string(eff - base) + C_RESET)
                    : "";
                return C_DIM " " + string(BD_V) + "  " C_SILVER + lbl + C_RESET
                     + " " C_WHITE + std::to_string(base) + C_RESET
                     + bonus + "  " + rankBar(eff, maxV, 8);
            };

            right.push_back("");
            right.push_back(" " C_DGOLD + fgoRule("STATUS", 30, C_DIM) + C_RESET);
            right.push_back(C_DIM " " + rep(BD_H, 30) + C_RESET);
            right.push_back(statLine("HP : ", bHP,  eHP,  MAX_HP));
            right.push_back(statLine("STR: ", bSTR, eSTR, MAX_STAT));
            right.push_back(statLine("DUR: ", bDUR, eDUR, MAX_STAT));
            right.push_back(statLine("AGI: ", bAGI, eAGI, MAX_STAT));

            // Noble Phantasm cards
            const auto& nps = sv->getNPs();
            if (!nps.empty()) {
                right.push_back("");
                right.push_back(" " C_DGOLD + fgoRule("NOBLE PHANTASM", 30, C_DIM) + C_RESET);
                for (std::size_t ni = 0; ni < nps.size(); ++ni) {
                    right.push_back("");
                    pushNPCard(right, ni, nps[ni], sv->isNPAvailable(ni), 30);
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
                    cout << C_RED "\n Cannot sell while dragging!" C_RESET "\n Press any key...\n";
                    getSingleChar(); break;
                }
                Item* it = inv->getCurrentItem();
                if (it) showSellConfirmScreen(it, inv->getCurrentRow(), inv->getCurrentCol());
                else { cout << C_RED "\n No item here!" C_RESET "\n Press any key...\n"; getSingleChar(); }
                showItemInfo = false; break;
            }
            case 't': case 'T':
                if (!inv->equipItem()) {
                    bool eq = (inv->getCurrentCol() >= 3);
                    if (eq)
                        cout << C_RED "\n No item to unequip, or bag is full!" C_RESET "\n Press any key...\n";
                    else
                        cout << C_RED "\n No item here or equipment slots full!" C_RESET "\n Press any key...\n";
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
// Battle Arena  —  FGO duel style
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

        cout << C_GOLD << fgoRule("BATTLE ARENA", 44, C_DGOLD) << C_RESET << "\n";
        cout << C_DIM "  Your Servant: " C_WHITE << myServant->getName() << C_RESET << "\n";
        myServant->renderAscii();

        if (pool.empty()) {
            cout << "\n" C_GOLD "  All servants have been defeated!" C_RESET "\n";
            cout << C_DIM "  You are the last Master standing." C_RESET "\n\n";
            cout << C_DIM "  Press any key to return..." C_RESET "\n";
            getSingleChar(); return;
        }

        cout << C_DIM "\n  Remaining enemies: " C_WHITE << pool.size() << C_RESET << "\n";
        cout << C_DIM "  Press " C_GOLD "B" C_DIM " to enter battle, any other key to exit..." C_RESET "\n";
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

        // Health bars
        auto hpBar = [](int hp, int maxHp, int w) -> string {
            int filled = (maxHp > 0) ? (hp * w / maxHp) : 0;
            string bar = C_GREEN;
            for (int i = 0; i < w; i++)
                bar += (i < filled) ? "\u2588" : (C_DIM "\u2591" C_GREEN);
            bar += C_RESET;
            return bar;
        };

        // Player header
        cout << C_GOLD "\u2554" << rep(BD_DH, 42) << "\u2557" C_RESET "\n";
        cout << C_GOLD "\u2551 " C_WHITE + padRight(myServant->getName(), 20)
             << C_DIM "HP: " C_GREEN << pHP << "/" << pMaxHP;
        int pad1 = 42 - 2 - (int)myServant->getName().size() - 4 -
                   (int)std::to_string(pHP).size() - 1 -
                   (int)std::to_string(pMaxHP).size();
        if (pad1 > 0) for (int i = 0; i < pad1; i++) cout << ' ';
        cout << C_GOLD " \u2551" C_RESET "\n";
        cout << C_GOLD "\u2551 " << hpBar(pHP, pMaxHP, 38) << " \u2551" C_RESET "\n";
        cout << C_GOLD "\u2560" << rep(BD_DH, 42) << "\u2563" C_RESET "\n";

        myServant->renderAscii();

        // VS divider
        cout << C_DIM "                 VS" C_RESET "\n\n";

        // Enemy header
        cout << C_RED "\u2554" << rep(BD_DH, 42) << "\u2557" C_RESET "\n";
        cout << C_RED "\u2551 " C_WHITE + padRight(en.getName(), 20)
             << C_DIM "HP: " C_BRED << eHP << "/" << en.getMaxHP();
        int pad2 = 42 - 2 - (int)en.getName().size() - 4 -
                   (int)std::to_string(eHP).size() - 1 -
                   (int)std::to_string(en.getMaxHP()).size();
        if (pad2 > 0) for (int i = 0; i < pad2; i++) cout << ' ';
        cout << C_RED " \u2551" C_RESET "\n";
        cout << C_RED "\u2551 " << hpBar(eHP, en.getMaxHP(), 38) << " \u2551" C_RESET "\n";
        cout << C_RED "\u255a" << rep(BD_DH, 42) << "\u255d" C_RESET "\n";

        en.renderAscii();

        // Action menu
        cout << "\n" C_DIM + rep(BD_H, 44) + C_RESET "\n";
        cout << C_GOLD "  A" C_DIM " - Basic attack" C_RESET "\n";
        const auto& nps = myServant->getNPs();
        for (std::size_t i = 0; i < nps.size(); ++i) {
            bool avail = myServant->isNPAvailable(i);
            cout << (avail ? C_GOLD : C_DIM)
                 << "  " << (i + 1) << " - " C_LGOLD << nps[i].name
                 << C_DIM " [Dmg: " C_BRED << nps[i].baseDamage
                 << C_DIM " Scale: " C_ORANGE << nps[i].damageScale << "x"
                 << (avail ? "" : C_DIM "  (used)")
                 << C_RESET "\n";
        }
        cout << C_DIM "  E - Escape" C_RESET "\n";

        int  key      = (int)getSingleChar();
        bool usedTurn = false;

        if (key == 'e' || key == 'E') {
            cout << C_DIM "\n  You fled from battle." C_RESET "\n  Press any key...\n";
            getSingleChar(); return false;

        } else if (key == 'a' || key == 'A') {
            int hit = std::max(10, std::min(95, 75 + (pAgiEff - eAgiEff) * 2));
            if (std::rand() % 100 < hit) {
                int dmg = pStrEff - (eDurEff / 2);
                if (dmg < 3) dmg = 3;
                eHP -= dmg; if (eHP < 0) eHP = 0;
                cout << "\n" C_WHITE "  You strike " C_CYAN << en.getName()
                     << C_WHITE " for " C_GREEN << dmg << C_WHITE " damage!" C_RESET "\n";
            } else {
                cout << "\n" C_DIM "  Your attack is deflected!" C_RESET "\n";
            }
            usedTurn = true;

        } else if (key >= '1' && key <= '9') {
            std::size_t ni = static_cast<std::size_t>(key - '1');
            if (ni < nps.size() && myServant->isNPAvailable(ni)) {
                int hit = std::max(20, std::min(99, 85 + (pAgiEff - eAgiEff)));
                // NP activation banner
                cout << "\n" C_GOLD
                     "  \u2554" << rep(BD_DH, 38) << "\u2557" C_RESET "\n";
                cout << C_GOLD "  \u2551" C_LGOLD;
                string title = "  NOBLE PHANTASM: " + nps[ni].name + "  ";
                cout << padRight(title, 38);
                cout << C_GOLD "\u2551" C_RESET "\n";
                cout << C_GOLD "  \u2551" C_DIM;
                cout << padRight("  Dmg: " + std::to_string(nps[ni].baseDamage) +
                                 "  Scale: " + std::to_string(nps[ni].damageScale) + "x", 38);
                cout << C_GOLD "\u2551" C_RESET "\n";
                cout << C_GOLD "  \u2551" C_SILVER;
                cout << padRight("  " + nps[ni].description, 38);
                cout << C_GOLD "\u2551" C_RESET "\n";
                cout << C_GOLD "  \u255a" << rep(BD_DH, 38) << "\u255d" C_RESET "\n";

                if (std::rand() % 100 < hit) {
                    int dmg = myServant->noblePhantasmDamage(ni,
                                  pStrEff - myServant->getStrength(), eDurEff / 2);
                    dmg /= 2; if (dmg < 12) dmg = 12;
                    eHP -= dmg; if (eHP < 0) eHP = 0;
                    myServant->markNPUsed(ni);
                    cout << C_GOLD "  Hits for " C_BRED << dmg << C_GOLD " damage!" C_RESET "\n";
                } else {
                    myServant->markNPUsed(ni);
                    cout << C_DIM "  The Noble Phantasm was neutralised!" C_RESET "\n";
                }
                usedTurn = true;
            }
        }

        if (!usedTurn) cout << "\n" C_DIM "  You hesitate — turn lost." C_RESET "\n";

        if (eHP <= 0) {
            cout << "\n" C_GREEN "  " << en.getName() << " has been defeated!" C_RESET "\n";
            cout << C_DIM "  Press any key..." C_RESET "\n";
            getSingleChar(); return true;
        }

        // Enemy turn
        cout << "\n" C_RED "  " << en.getName() << "'s turn..." C_RESET "\n";
        bool enemyUsedNP = false;
        const auto& eNPs = en.getNPs();
        for (std::size_t i = 0; i < eNPs.size(); ++i) {
            if (en.isNPAvailable(i) && (std::rand() % 4 == 0)) {
                int dmg = en.noblePhantasmDamage(i, 0, pDurEff / 2);
                dmg /= 2; if (dmg < 12) dmg = 12;
                pHP -= dmg; if (pHP < 0) pHP = 0;
                en.markNPUsed(i);
                cout << C_RED "  \u2554" << rep(BD_DH, 38) << "\u2557" C_RESET "\n";
                cout << C_RED "  \u2551" C_BRED;
                string etitle = "  ENEMY NP: " + eNPs[i].name + "  ";
                cout << padRight(etitle, 38);
                cout << C_RED "\u2551" C_RESET "\n";
                cout << C_RED "  \u2551" C_SILVER;
                cout << padRight("  " + eNPs[i].description, 38);
                cout << C_RED "\u2551" C_RESET "\n";
                cout << C_RED "  \u255a" << rep(BD_DH, 38) << "\u255d" C_RESET "\n";
                cout << C_RED "  Deals " C_BRED << dmg << C_RED " damage to you!" C_RESET "\n";
                enemyUsedNP = true; break;
            }
        }
        if (!enemyUsedNP) {
            int hit = std::max(10, std::min(95, 75 + (eAgiEff - pAgiEff) * 2));
            if (std::rand() % 100 < hit) {
                int dmg = eStrEff - (pDurEff / 2);
                if (dmg < 3) dmg = 3;
                pHP -= dmg; if (pHP < 0) pHP = 0;
                cout << C_WHITE "  " << en.getName()
                     << C_RED " strikes you for " C_BRED << dmg << C_RED " damage!" C_RESET "\n";
            } else {
                cout << C_DIM "  " << en.getName() << "'s attack misses!" C_RESET "\n";
            }
        }

        if (pHP <= 0) {
            cout << "\n" C_RED "  Your servant has fallen..." C_RESET "\n";
            cout << C_DIM "  Press any key..." C_RESET "\n";
            getSingleChar(); return false;
        }

        cout << "\n" C_DIM "  Press any key for next round..." C_RESET "\n";
        getSingleChar();
    }
    return false;
}
