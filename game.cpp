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

static std::string rep(const std::string& s, int n) {
    std::string out; for (int i = 0; i < n; i++) out += s; return out;
}

// FGO-style horizontal rule
static std::string fgoRule(const std::string& title, int totalW, const std::string& col) {
    int tv = visibleLen(title);
    int sides = totalW - tv - 2;
    int left  = sides / 2;
    int right = sides - left;
    return col + rep(BD_H, left) + " " + C_WHITE + title + col + " " + rep(BD_H, right) + C_RESET;
}

static std::string rankBar(int val, int maxVal, int pips) {
    int filled = (maxVal > 0) ? (val * pips / maxVal) : 0;
    std::string bar = C_GOLD;
    for (int i = 0; i < pips; i++)
        bar += (i < filled) ? "\u25a0" : (C_DIM "\u25a1" C_GOLD);
    bar += C_RESET;
    return bar;
}

// Track which Holy Grail Wars have been cleared
static bool g_zeroCleared = false;
static bool g_stayCleared = false;

static const char* seriesLabel(Series s) {
    switch (s) {
        case Series::Zero:      return "Fate/Zero";
        case Series::StayNight: return "Fate/stay night";
        default:                return "Unknown War";
    }
}

// =============================================================================
// Master-name → Servant + war mapping
// =============================================================================

static Servant servantForMasterName(const std::string& nameRaw) {
    std::string name;
    name.reserve(nameRaw.size());
    for (char c : nameRaw) {
        if (!std::isspace(static_cast<unsigned char>(c)))
            name += std::tolower(static_cast<unsigned char>(c));
    }

    // Fifth Holy Grail War – Fate/stay night
    if (name == "shirou" || name == "shirouemiya" || name == "emiyashirou") {
        return Servant::selectServantForMaster("Shirou Emiya", Series::StayNight);
    }
    if (name == "rin" || name == "rintohsaka" || name == "tohsakarin") {
        return Servant::selectServantForMaster("Rin Tohsaka", Series::StayNight);
    }
    if (name == "illya" || name == "illyasviel" ||
        name == "illyasvielvoneinzbern" || name == "illyavoneinzbern") {
        return Servant::selectServantForMaster("Illyasviel von Einzbern", Series::StayNight);
    }
    if (name == "sakura" || name == "sakuramatou" || name == "matousakura") {
        return Servant::selectServantForMaster("Sakura Matou", Series::StayNight);
    }
    if (name == "shinji" || name == "shinjimatou" || name == "matoushinji") {
        return Servant::selectServantForMaster("Shinji Matou", Series::StayNight);
    }
    if (name == "kuzuki" || name == "souichiroukuzuki" || name == "soichirokuzuki") {
        return Servant::selectServantForMaster("Souichirou Kuzuki", Series::StayNight);
    }

    // Fourth Holy Grail War – Fate/Zero
    if (name == "kiritsugu" || name == "kiritsuguemiya" || name == "emiyakiritsugu") {
        return Servant::selectServantForMaster("Kiritsugu Emiya", Series::Zero);
    }
    if (name == "tokiomi" || name == "tokio mitohsaka" || name == "tokio mithosaka" ||
        name == "tokio mithosaka") {
        // handle simple "tokiomi" as well
        return Servant::selectServantForMaster("Tokiomi Tohsaka", Series::Zero);
    }
    if (name == "kayneth" || name == "kaynethelmelloiarchibald" ||
        name == "elmelloiarchibald" || name == "kaynetharchibald") {
        return Servant::selectServantForMaster("Kayneth El-Melloi Archibald", Series::Zero);
    }
    if (name == "waver" || name == "wavervelvet") {
        return Servant::selectServantForMaster("Waver Velvet", Series::Zero);
    }
    if (name == "ryuunosuke" || name == "ryunosuke" ||
        name == "ryuunosukeuryuu" || name == "ryunosukeuryuu") {
        return Servant::selectServantForMaster("Ryuunosuke Uryuu", Series::Zero);
    }
    if (name == "kariya" || name == "kariyamatou" || name == "matoukariya") {
        return Servant::selectServantForMaster("Kariya Matou", Series::Zero);
    }

    // Kirei Kotomine – random between the two wars
    if (name == "kirei" || name == "kireikotomine" || name == "kotominekirei") {
        if (std::rand() % 2 == 0) {
            return Servant::selectServantForMaster("Kirei Kotomine", Series::StayNight);
        } else {
            return Servant::selectServantForMaster("Kirei Kotomine", Series::Zero);
        }
    }

    // Fallback: completely random servant from any war
    return Servant::randomServant();
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
// Servant showcase screen
// =============================================================================

void Game::showServantProfile(const Servant& sv) {
    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << C_GOLD << fgoRule("SERVANT SUMMONED", 72, C_DGOLD) << C_RESET << "\n\n";

        bool isZero = (sv.getSeries() == Series::Zero);
        string seriesBadge = isZero
            ? (C_RED "\u2605 Fate/Zero" C_RESET)
            : (std::string(C_BLUE) + "\u2605 Fate/stay night" + C_RESET);

        cout << " " C_GOLD "\u2694  " C_WHITE << sv.getName() << C_RESET << "\n";
        cout << "    " << seriesBadge << "\n\n";

        for (auto& line : splitLines(sv.getAsciiArt()))
            cout << " " << line << "\n";

        cout << "\n" C_DIM << rep(BD_H, 72) << C_RESET << "\n";

        int bHP  = sv.getMaxHP();
        int bSTR = sv.getStrength();
        int bDUR = sv.getDurability();
        int bAGI = sv.getAgility();

        const int MAX_HP   = 220;
        const int MAX_STAT = 32;

        auto statLine = [&](const string& lbl, int val, int maxV) {
            cout << C_DIM " " << BD_V << "  " C_SILVER << lbl << C_RESET
                 << " " C_WHITE << val << C_RESET
                 << "  " << rankBar(val, maxV, 8) << "\n";
        };

        cout << C_DGOLD << fgoRule("STATUS", 32, C_DIM) << C_RESET << "\n";
        statLine("HP : ", bHP,  MAX_HP);
        statLine("STR: ", bSTR, MAX_STAT);
        statLine("DUR: ", bDUR, MAX_STAT);
        statLine("AGI: ", bAGI, MAX_STAT);

        const auto& nps = sv.getNPs();
        if (!nps.empty()) {
            cout << "\n" << C_DGOLD << fgoRule("NOBLE PHANTASMS", 40, C_DIM) << C_RESET << "\n";
            for (std::size_t i = 0; i < nps.size(); ++i) {
                const auto& np = nps[i];
                cout << C_LGOLD "  [" << (i + 1) << "] " << np.name << C_RESET << "\n";
                cout << C_DIM   "      Dmg: " C_BRED << np.baseDamage
                     << C_DIM "  Scale: " C_ORANGE << np.damageScale << "x" C_RESET << "\n";
                cout << C_SILVER "      " << np.description << C_RESET << "\n\n";
            }
        }

        cout << C_DIM << rep(BD_H, 72) << C_RESET << "\n";
        cout << C_DIM "  Press any key to continue..." C_RESET << "\n";
        getSingleChar();
        return;
    }
}

// =============================================================================
// Naming screen (before main menu)
// =============================================================================

void Game::showNamingScreen() {
    std::string input;
    const int FRAME_W = 53;

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << C_GOLD BD_DTL << rep(BD_DH, FRAME_W) << BD_DTR C_RESET << "\n";
        cout << C_GOLD BD_DV
             << C_LGOLD "           ❁  MASTER REGISTRATION  ❁                 "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";

        cout << C_GOLD BD_DV << C_DIM
             "  Enter your name as a Master of the Holy Grail War. "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DV << C_DIM
             "  Certain names may call forth particular Servants..."
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";

        cout << C_GOLD BD_DV << C_WHITE "  Name: " << C_CYAN << input << C_DIM << "_"
             << C_RESET;
        int used = 8 + (int)input.size();
        for (int i = used; i < FRAME_W; ++i) cout << ' ';
        cout << C_GOLD BD_DV C_RESET << "\n";

        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";
        cout << C_GOLD BD_DV << C_DIM
             "  Enter - Confirm     Esc - Random name & servant    "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DBL << rep(BD_DH, FRAME_W) << BD_DBR C_RESET << "\n";

        int key = (int)getSingleChar();
        if (key == 10 || key == 13) { // Enter
            if (!input.empty()) {
                masterName = input;
                Servant chosen = servantForMasterName(input);
                player->setServant(chosen);
                showServantProfile(chosen);
                return;
            }
        } else if (key == 27) { // Esc
            masterName = "";
            Servant chosen = Servant::randomServant();
            player->setServant(chosen);
            showServantProfile(chosen);
            return;
        } else if (key == 127 || key == 8) {
            if (!input.empty()) input.pop_back();
        } else if (key >= 32 && key <= 126) {
            if ((int)input.size() < 24)
                input += static_cast<char>(key);
        }
    }
}

// =============================================================================
// Renaming screen for second Holy Grail War
// =============================================================================

void Game::showRenamingScreen() {
    std::string input;
    const int FRAME_W = 53;

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << C_GOLD BD_DTL << rep(BD_DH, FRAME_W) << BD_DTR C_RESET << "\n";
        cout << C_GOLD BD_DV
             << C_LGOLD "      ❁  NEW MASTER REGISTRATION  ❁            "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";

        cout << C_GOLD BD_DV << C_DIM
             "  You are summoned to a new Holy Grail War.        "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DV << C_DIM
             "  Enter your name as this war's new Master.       "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";

        cout << C_GOLD BD_DV << C_WHITE "  Name: " << C_CYAN << input << C_DIM << "_"
             << C_RESET;
        int used = 8 + (int)input.size();
        for (int i = used; i < FRAME_W; ++i) cout << ' ';
        cout << C_GOLD BD_DV C_RESET << "\n";

        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";
        cout << C_GOLD BD_DV << C_DIM
             "  Enter - Confirm     Esc - Keep name             "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DBL << rep(BD_DH, FRAME_W) << BD_DBR C_RESET << "\n";

        int key = (int)getSingleChar();
        if (key == 10 || key == 13) { // Enter
            if (!input.empty()) masterName = input;
            showServantProfile(*player->getServant());
            return;
        } else if (key == 27) {
            showServantProfile(*player->getServant());
            return;
        } else if (key == 127 || key == 8) {
            if (!input.empty()) input.pop_back();
        } else if (key >= 32 && key <= 126) {
            if ((int)input.size() < 24)
                input += static_cast<char>(key);
        }
    }
}

// =============================================================================
// Enemy gallery – FSN on left, Zero on right, defeated in red
// =============================================================================

void Game::showEnemyGallery() {
    printf("\033[2J"); printf("\033[H"); fflush(stdout);

    const int LEFT_W  = 34;
    const int RIGHT_W = 34;

    cout << C_GOLD << fgoRule("HOLY GRAIL WAR ROSTER", LEFT_W + RIGHT_W + 4, C_DGOLD)
         << C_RESET << "\n\n";

    auto stay = Servant::enemyPoolForSeries(Series::StayNight);
    auto zero = Servant::enemyPoolForSeries(Series::Zero);

    string leftHeader  = string(" ") + C_LGOLD "Fate/stay night" C_RESET;
    string rightHeader = string(" ") + C_LGOLD "Fate/Zero" C_RESET;

    cout << padRight(leftHeader,  LEFT_W)
         << "  "
         << padRight(rightHeader, RIGHT_W) << "\n";

    cout << padRight(string(" ") + C_DIM + rep(BD_H, LEFT_W - 2)  + C_RESET, LEFT_W)
         << "  "
         << padRight(string(" ") + C_DIM + rep(BD_H, RIGHT_W - 2) + C_RESET, RIGHT_W)
         << "\n\n";

    int rows = (int)std::max(stay.size(), zero.size());

    for (int i = 0; i < rows; ++i) {
        std::string leftName, leftMaster;
        std::string rightName, rightMaster;

        if (i < (int)stay.size()) {
            const Servant& sv = stay[i];
            bool defeated = (defeatedEnemies.find(sv.getName()) != defeatedEnemies.end());

            if (defeated)
                leftName = std::string(" ") + C_RED + sv.getName() + " [DEFEATED]" + C_RESET;
            else
                leftName = std::string(" ") + C_GOLD + sv.getName() + C_RESET;

            leftMaster = std::string(" ")
                       + C_DIM "M: " C_CYAN + sv.getMasterName() + C_RESET;
        }

        if (i < (int)zero.size()) {
            const Servant& sv = zero[i];
            bool defeated = (defeatedEnemies.find(sv.getName()) != defeatedEnemies.end());

            if (defeated)
                rightName = std::string(" ") + C_RED + sv.getName() + " [DEFEATED]" + C_RESET;
            else
                rightName = std::string(" ") + C_GOLD + sv.getName() + C_RESET;

            rightMaster = std::string(" ")
                        + C_DIM "M: " C_CYAN + sv.getMasterName() + C_RESET;
        }

        cout << padRight(leftName,  LEFT_W)
             << "  "
             << padRight(rightName, RIGHT_W) << "\n";

        cout << padRight(leftMaster,  LEFT_W)
             << "  "
             << padRight(rightMaster, RIGHT_W) << "\n\n";
    }

    cout << C_DIM "  Press any key to return..." C_RESET << "\n";
    getSingleChar();
}

// =============================================================================
// Main Menu
// =============================================================================

void Game::showMainMenu() {
    int sel = 0;
    const int OPTIONS = 5;
    const int FRAME_W = 35;

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << C_GOLD BD_DTL << rep(BD_DH, FRAME_W) << BD_DTR C_RESET << "\n";
        cout << C_GOLD BD_DV
             << C_LGOLD "  ⚔  HOLY GRAIL WAR  •  MASTER HQ  "
             << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";

        if (!masterName.empty()) {
            cout << C_GOLD BD_DV << C_DIM "  Master: " << C_WHITE << masterName;
            int used = 11 + (int)masterName.size();
            for (int i = used; i < FRAME_W; ++i) cout << ' ';
            cout << C_GOLD BD_DV C_RESET << "\n";
            cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";
        }

        const char* labels[OPTIONS] = {
            "  Inventory  ", "  Shop     ", "  Battle   ", "  Gallery  ", "  Quit     "
        };
        const char* icons[OPTIONS]  = { "\u2696", "\u2741", "\u2694", "\u24c8", "\u00d7" };
        const char* cols[OPTIONS]   = { C_CYAN, C_GOLD, C_BRED, C_MGENTA, C_DIM };

        for (int i = 0; i < OPTIONS; i++) {
            bool active = (i == sel);
            std::string prefix = active
                ? (std::string(C_GOLD) + BD_DV + C_GOLD + "  \u25b6 ")
                : (std::string(C_GOLD) + BD_DV + C_DIM  + "    ");
            cout << prefix
                 << cols[i] << icons[i] << "  "
                 << (active ? C_WHITE : C_DIM)
                 << labels[i];

            int used = 8 + (int)string(labels[i]).size();
            for (int s = used; s < FRAME_W; ++s) cout << ' ';

            cout << C_GOLD << BD_DV << C_RESET << "\n";
        }

        cout << C_GOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";
        cout << C_GOLD BD_DV << C_DIM " Gold: " << C_GOLD
             << shop->getPlayerGold() << "g";
        int gl = (int)std::to_string(shop->getPlayerGold()).size() + 8;
        for (int i = gl; i < FRAME_W; ++i) cout << ' ';
        cout << C_GOLD BD_DV C_RESET << "\n";
        cout << C_GOLD BD_DBL << rep(BD_DH, FRAME_W) << BD_DBR C_RESET << "\n";
        cout << C_DIM "  W/S navigate  " BD_V "  Enter select  " BD_V "  Esc quit" C_RESET << "\n";

        int key = (int)getSingleChar();
        switch (key) {
            case 'w': case 'W': sel = (sel - 1 + OPTIONS) % OPTIONS; break;
            case 's': case 'S': sel = (sel + 1) % OPTIONS; break;
            case 10:  case 13:
                if      (sel == 0) play();
                else if (sel == 1) playShop();
                else if (sel == 2) playBattleArena();
                else if (sel == 3) showEnemyGallery();
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
    const int FRAME_W = 46;

    while (true) {
        printf("\033[2J"); printf("\033[H"); fflush(stdout);

        cout << C_DGOLD BD_DTL << rep(BD_DH, FRAME_W) << BD_DTR C_RESET << "\n";
        cout << C_DGOLD BD_DV C_GOLD "            ❁  SELL ITEM?  ❁            "
             << C_DGOLD BD_DV C_RESET << "\n";
        cout << C_DGOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";

        auto printRow = [&](const string& lbl, const string& val, const string& vc) {
            string content = string("  ") + C_SILVER + lbl + vc + val + C_RESET;
            cout << C_DGOLD BD_DV << padRight(content, FRAME_W - 1)
                 << C_DGOLD BD_DV C_RESET << "\n";
        };

        printRow("Item:   ", item->getName(),                        C_WHITE);
        printRow("Type:   ", Item::typeToString(item->getType()),    C_CYAN);
        printRow("Worth:  ", std::to_string(item->getPrice()) + "g", C_GOLD);
        printRow("Sell:   ", std::to_string(sellPrice) + "g (" +
                               std::to_string(pct) + "%)",           C_GREEN);

        cout << C_DGOLD BD_DLT << rep(BD_DH, FRAME_W) << BD_DRT C_RESET << "\n";
        cout << C_DGOLD BD_DV C_DIM
             "     " C_GREEN "Y" C_DIM " - Confirm sell   "
             BD_V "   " C_RED "N / Esc" C_DIM " - Cancel    "
             << C_DGOLD BD_DV C_RESET << "\n";
        cout << C_DGOLD BD_DBL << rep(BD_DH, FRAME_W) << BD_DBR C_RESET << "\n";

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
// INVENTORY SCREEN
// =============================================================================

static void pushNPCard(vector<string>& panel, std::size_t idx,
                       const NoblePhantasm& np, bool available, int panelW) {
    std::string avail = available ? (C_GOLD "\u25cf READY") : (C_DIM "\u25cb USED");
    panel.push_back(
        C_DIM " " + std::string(BD_TL) + rep(BD_H, 3) + C_RESET +
        " " C_LGOLD + np.name + C_RESET +
        "  " + avail + C_RESET
    );
    panel.push_back(
        C_DIM "  " + std::string(BD_V) + "  "
        C_DIM "Dmg: " C_BRED + std::to_string(np.baseDamage) +
        C_DIM "  Scale: " C_ORANGE + std::to_string((int)(np.damageScale * 10)) + "x" +
        C_RESET
    );
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

        vector<string> left;

        left.push_back(
            C_GOLD " ❁ " C_WHITE "INVENTORY" C_GOLD "  │  "
            C_DIM "Gold: " C_GOLD + std::to_string(shop->getPlayerGold()) + "g"
            + C_DIM "  │  "
            + (inEquip ? C_GOLD "[EQUIP ZONE]" : C_SILVER "[BAG]") + C_RESET
        );
        left.push_back(C_DIM + rep(BD_H, LEFT_W) + C_RESET);
        left.push_back("");

        auto bagLines   = inv->renderBagLines();
        auto equipLines = inv->renderEquipLines();
        int  gridRows   = (int)std::max(bagLines.size(), equipLines.size());
        for (int i = 0; i < gridRows; i++) {
            string bl = (i < (int)bagLines.size())   ? bagLines[i]   : string(21, ' ');
            string el = (i < (int)equipLines.size()) ? equipLines[i] : string(15, ' ');
            left.push_back(padRight(bl, 25) + el);
        }
        left.push_back("");

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
                left.push_back(C_DIM "  Effect: " C_GREEN  "+" + std::to_string(cur->getPercentValue()) + "%" + C_RESET);
            } else {
                left.push_back(C_DIM "  [empty slot]" C_RESET);
            }
        }

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

        vector<string> right;

        bool isZero = (sv->getSeries() == Series::Zero);
        string seriesBadge = isZero
            ? (C_RED "\u2605 Fate/Zero" C_RESET)
            : (std::string(C_BLUE) + "\u2605 Fate/stay night" + C_RESET);
        right.push_back(" " C_GOLD "\u2694  " C_WHITE + sv->getName() + C_RESET);
        right.push_back("    " + seriesBadge);
        right.push_back(" " C_DIM + rep(BD_H, 30) + C_RESET);
        right.push_back("");

        for (auto& al : splitLines(sv->getAsciiArt()))
            right.push_back(" " + al);

        if (showStats) {
            int bHP  = sv->getMaxHP(),     eHP  = player->getEffectiveMaxHP();
            int bSTR = sv->getStrength(),   eSTR = player->getEffectiveStrength();
            int bDUR = sv->getDurability(), eDUR = player->getEffectiveDurability();
            int bAGI = sv->getAgility(),    eAGI = player->getEffectiveAgility();

            const int MAX_HP  = 220;
            const int MAX_STAT = 32;

            auto statLine = [&](const string& lbl, int base, int eff, int maxV) -> string {
                string bonus = (eff != base)
                    ? (C_GREEN "  +" + std::to_string(eff - base) + C_RESET)
                    : "";
                return C_DIM " " + string(BD_V) + "  " C_SILVER + lbl + C_RESET
                     + " " C_WHITE + std::to_string(base) + C_RESET
                     + "  " + rankBar(eff, maxV, 8)
                     + bonus;
            };

            right.push_back("");
            right.push_back(" " C_DGOLD + fgoRule("STATUS", 30, C_DIM) + C_RESET);
            right.push_back(C_DIM " " + rep(BD_H, 30) + C_RESET);
            right.push_back(statLine("HP : ", bHP,  eHP,  MAX_HP));
            right.push_back(statLine("STR: ", bSTR, eSTR, MAX_STAT));
            right.push_back(statLine("DUR: ", bDUR, eDUR, MAX_STAT));
            right.push_back(statLine("AGI: ", bAGI, eAGI, MAX_STAT));

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
// Battle Arena – dual-war flow, bigger arena
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

        cout << C_GOLD << fgoRule("BATTLE ARENA", 72, C_DGOLD) << C_RESET << "\n";
        cout << C_DIM "  Master: " C_WHITE << (masterName.empty() ? std::string("Unknown") : masterName)
             << C_DIM "   Servant: " C_WHITE << myServant->getName() << C_RESET << "\n";
        cout << C_DIM "  A grand arena manifests between two opposing heroes..." C_RESET << "\n\n";

        if (pool.empty()) {
            Series curSeries = myServant->getSeries();
            if (curSeries == Series::Zero)      g_zeroCleared = true;
            else if (curSeries == Series::StayNight) g_stayCleared = true;

            cout << C_GOLD "  All servants of this Holy Grail War have been defeated!"
                 << C_RESET << "\n";
            cout << C_DIM "  You stand as the final Master of " C_LGOLD
                 << seriesLabel(curSeries) << C_DIM "." C_RESET << "\n\n";

            if (g_zeroCleared && g_stayCleared) {
                cout << C_GOLD "  You have conquered both Holy Grail Wars." C_RESET << "\n";
                cout << C_DIM  "  No more Masters remain to challenge you." C_RESET << "\n\n";
                cout << C_DIM  "  Press any key to return to HQ..." C_RESET << "\n";
                getSingleChar();
                return;
            }

            Series nextSeries =
                (curSeries == Series::Zero) ? Series::StayNight : Series::Zero;

            cout << C_LGOLD "  A new Holy Grail War stirs..." C_RESET << "\n";
            cout << C_DIM   "  You will enter " C_LGOLD << seriesLabel(nextSeries)
                 << C_DIM " as a new Master with a new Servant." C_RESET << "\n\n";
            cout << C_DIM   "  Press any key to accept this calling..." C_RESET << "\n";
            getSingleChar();

            auto nextPool = Servant::enemyPoolForSeries(nextSeries);
            if (!nextPool.empty()) {
                Servant nextServant = nextPool[std::rand() % (int)nextPool.size()];
                player->setServant(nextServant);
            }

            defeatedEnemies.clear();
            showRenamingScreen();
            return;
        }

        cout << C_DIM "  Remaining enemies: " C_WHITE << pool.size() << C_RESET << "\n";
        cout << C_DIM "  Press " C_GOLD "B" C_DIM " to enter battle, any other key to exit..." C_RESET << "\n";
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

        auto hpBar = [](int hp, int maxHp, int w,
                        const std::string& colMain,
                        const std::string& colDim) -> std::string {
            int filled = (maxHp > 0) ? (hp * w / maxHp) : 0;
            std::string bar = colMain;
            for (int i = 0; i < w; i++)
                bar += (i < filled) ? "\u2588" : (colDim + "\u2591" + colMain);
            bar += C_RESET;
            return bar;
        };

        const int PANEL_W = 36;
        const int ARENA_W = PANEL_W * 2 + 8;

        string pHeaderTop =
            string(C_BLUE) + "\u2554" + rep(BD_DH, PANEL_W) + "\u2557" + C_RESET;
        string pHeaderName =
            string(C_BLUE "\u2551 ") + C_WHITE + padRight(myServant->getName(), 18)
            + C_DIM "HP: " C_GREEN + std::to_string(pHP) + "/" + std::to_string(pMaxHP);
        int pPad = PANEL_W - 2 - 18 - 4
                 - (int)std::to_string(pHP).size() - 1
                 - (int)std::to_string(pMaxHP).size();
        if (pPad < 0) pPad = 0;
        pHeaderName += string(pPad, ' ') + C_BLUE " \u2551" C_RESET;
        string pHeaderHpBar =
            string(C_BLUE "\u2551 ") + hpBar(pHP, pMaxHP, PANEL_W - 4, C_GREEN, C_DIM)
            + C_BLUE " \u2551" C_RESET;
        string pHeaderBottom =
            string(C_BLUE "\u255a") + rep(BD_DH, PANEL_W) + "\u255d" C_RESET;

        string eHeaderTop =
            string(C_RED) + "\u2554" + rep(BD_DH, PANEL_W) + "\u2557" + C_RESET;
        string eHeaderName =
            string(C_RED "\u2551 ") + C_WHITE + padRight(en.getName(), 18)
            + C_DIM "HP: " C_BRED + std::to_string(eHP) + "/" + std::to_string(en.getMaxHP());
        int ePad = PANEL_W - 2 - 18 - 4
                 - (int)std::to_string(eHP).size() - 1
                 - (int)std::to_string(en.getMaxHP()).size();
        if (ePad < 0) ePad = 0;
        eHeaderName += string(ePad, ' ') + C_RED " \u2551" C_RESET;
        string eHeaderHpBar =
            string(C_RED "\u2551 ") + hpBar(eHP, en.getMaxHP(), PANEL_W - 4, C_ORANGE, C_DIM)
            + C_RED " \u2551" C_RESET;
        string eHeaderBottom =
            string(C_RED "\u255a") + rep(BD_DH, PANEL_W) + "\u255d" C_RESET;

        vector<string> pBody, eBody;
        for (auto& al : splitLines(myServant->getAsciiArt()))
            pBody.push_back(al);
        for (auto& al : splitLines(en.getAsciiArt()))
            eBody.push_back(al);
        int maxBody = (int)std::max(pBody.size(), eBody.size());

        cout << padRight(pHeaderTop,   PANEL_W + 4) << "  " << eHeaderTop    << "\n";
        cout << padRight(pHeaderName,  PANEL_W + 4) << "  " << eHeaderName   << "\n";
        cout << padRight(pHeaderHpBar, PANEL_W + 4) << "  " << eHeaderHpBar  << "\n";
        cout << padRight(pHeaderBottom,PANEL_W + 4) << "  " << eHeaderBottom << "\n";

        cout << C_DIM << rep(BD_H, PANEL_W / 2)
             << C_ORANGE "  VS  "
             << C_DIM << rep(BD_H, PANEL_W / 2)
             << C_RESET << "\n";

        for (int i = 0; i < maxBody; ++i) {
            string pl = (i < (int)pBody.size()) ? pBody[i] : "";
            string el = (i < (int)eBody.size()) ? eBody[i] : "";
            cout << padRight(" " + pl, PANEL_W + 4) << "  " << " " << el << "\n";
        }

        cout << "\n" << C_DGOLD << rep(BD_H, ARENA_W) << C_RESET << "\n";
        cout << C_GOLD "  A" C_DIM " - Basic attack" C_RESET << "\n";

        const auto& nps = myServant->getNPs();
        for (std::size_t i = 0; i < nps.size(); ++i) {
            bool avail = myServant->isNPAvailable(i);
            cout << (avail ? C_GOLD : C_DIM)
                 << "  " << (i + 1) << " - " C_LGOLD << nps[i].name
                 << C_DIM " [Dmg: " C_BRED << nps[i].baseDamage
                 << C_DIM " Scale: " C_ORANGE << nps[i].damageScale << "x"
                 << (avail ? "" : C_DIM "  (used)")
                 << C_RESET << "\n";
        }
        cout << C_DIM "  E - Escape" C_RESET << "\n";

        int  key      = (int)getSingleChar();
        bool usedTurn = false;

        if (key == 'e' || key == 'E') {
            cout << C_DIM "\n  You fled from battle." C_RESET "\n"
                 << C_DIM "  Press any key..." C_RESET "\n";
            getSingleChar();
            return false;
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

                cout << "\n"
                     << C_GOLD << "  \u2554" << rep(BD_DH, ARENA_W) << "\u2557"
                     << C_RESET << "\n";
                cout << C_GOLD << "  \u2551" << C_LGOLD;
                string title = "  NOBLE PHANTASM: " + nps[ni].name + "  ";
                cout << padRight(title, ARENA_W);
                cout << C_GOLD << "\u2551" << C_RESET << "\n";

                cout << C_GOLD << "  \u2551" << C_DIM;
                cout << padRight("  Dmg: " + std::to_string(nps[ni].baseDamage) +
                                 "  Scale: " + std::to_string(nps[ni].damageScale) + "x",
                                 ARENA_W);
                cout << C_GOLD << "\u2551" << C_RESET << "\n";

                cout << C_GOLD << "  \u2551" << C_SILVER;
                cout << padRight("  " + nps[ni].description, ARENA_W);
                cout << C_GOLD << "\u2551" << C_RESET << "\n";

                cout << C_GOLD << "  \u255a" << rep(BD_DH, ARENA_W) << "\u255d"
                     << C_RESET << "\n";

                if (std::rand() % 100 < hit) {
                    int dmg = myServant->noblePhantasmDamage(
                        ni,
                        pStrEff - myServant->getStrength(),
                        eDurEff / 2
                    );
                    dmg /= 2; if (dmg < 12) dmg = 12;
                    eHP -= dmg; if (eHP < 0) eHP = 0;
                    myServant->markNPUsed(ni);
                    cout << C_GOLD "  Hits for " C_BRED << dmg
                         << C_GOLD " damage!" C_RESET "\n";
                } else {
                    myServant->markNPUsed(ni);
                    cout << C_DIM "  The Noble Phantasm was neutralised!" C_RESET "\n";
                }
                usedTurn = true;
            }
        }

        if (!usedTurn) {
            cout << "\n" C_DIM "  You hesitate — turn lost." C_RESET "\n";
        }

        if (eHP <= 0) {
            cout << "\n" C_GREEN "  " << en.getName() << " has been defeated!" C_RESET "\n";
            cout << C_DIM "  Press any key..." C_RESET "\n";
            getSingleChar();
            return true;
        }

        cout << "\n" C_RED "  " << en.getName() << "'s turn..." C_RESET "\n";
        bool        enemyUsedNP = false;
        const auto& eNPs        = en.getNPs();
        for (std::size_t i = 0; i < eNPs.size(); ++i) {
            if (en.isNPAvailable(i) && (std::rand() % 4 == 0)) {
                int dmg = en.noblePhantasmDamage(i, 0, pDurEff / 2);
                dmg /= 2; if (dmg < 12) dmg = 12;
                pHP -= dmg; if (pHP < 0) pHP = 0;
                en.markNPUsed(i);

                cout << C_RED << "  \u2554" << rep(BD_DH, ARENA_W) << "\u2557"
                     << C_RESET << "\n";
                cout << C_RED << "  \u2551" << C_BRED;
                string etitle = "  ENEMY NP: " + eNPs[i].name + "  ";
                cout << padRight(etitle, ARENA_W);
                cout << C_RED << "\u2551" << C_RESET << "\n";

                cout << C_RED << "  \u2551" << C_SILVER;
                cout << padRight("  " + eNPs[i].description, ARENA_W);
                cout << C_RED << "\u2551" << C_RESET << "\n";

                cout << C_RED << "  \u255a" << rep(BD_DH, ARENA_W) << "\u255d"
                     << C_RESET << "\n";
                cout << C_RED "  Deals " C_BRED << dmg << C_RED " damage to you!"
                     << C_RESET "\n";

                enemyUsedNP = true;
                break;
            }
        }
        if (!enemyUsedNP) {
            int hit = std::max(10, std::min(95, 75 + (eAgiEff - pAgiEff) * 2));
            if (std::rand() % 100 < hit) {
                int dmg = eStrEff - (pDurEff / 2);
                if (dmg < 3) dmg = 3;
                pHP -= dmg; if (pHP < 0) pHP = 0;
                cout << C_WHITE "  " << en.getName()
                     << C_RED " strikes you for " C_BRED << dmg << C_RED " damage!"
                     << C_RESET "\n";
            } else {
                cout << C_DIM "  " << en.getName() << "'s attack misses!" C_RESET "\n";
            }
        }

        if (pHP <= 0) {
            cout << "\n" C_RED "  Your servant has fallen..." C_RESET "\n";
            cout << C_DIM "  Press any key..." C_RESET "\n";
            getSingleChar();
            return false;
        }

        cout << "\n" C_DIM "  Press any key for next round..." C_RESET "\n";
        getSingleChar();
    }
    return false;
}