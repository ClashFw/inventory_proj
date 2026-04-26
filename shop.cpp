#include "shop.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>

// ── ANSI palette ──────────────────────────────────────────────────────────
#define RST   "\033[0m"
#define GOLD  "\033[38;5;220m"
#define DGOLD "\033[38;5;178m"
#define LGOLD "\033[38;5;229m"
#define DIM   "\033[38;5;240m"
#define WHITE "\033[97m"
#define SILV  "\033[38;5;250m"
#define CYAN  "\033[96m"
#define BLUE  "\033[38;5;39m"
#define RED   "\033[38;5;196m"
#define BRED  "\033[91m"
#define GRN   "\033[92m"
#define ORG   "\033[38;5;214m"
#define MAG   "\033[38;5;201m"
// rarity-specific
#define COL_COM  "\033[37m"
#define COL_MAG  "\033[38;5;39m"
#define COL_RARE "\033[38;5;220m"
#define COL_LEG  "\033[38;5;202m"
#define COL_DIV  "\033[38;5;201m"

// box glyphs
#define DH  "\u2550"
#define DV  "\u2551"
#define DTL "\u2554"
#define DTR "\u2557"
#define DBL "\u255a"
#define DBR "\u255d"
#define DLT "\u2560"
#define DRT "\u2563"
#define SH  "\u2500"
#define SV  "\u2502"

static std::string rep(const std::string& s, int n) {
    std::string o; for (int i = 0; i < n; i++) o += s; return o;
}

static int visLen(const std::string& s) {
    int l = 0; bool e = false;
    for (char c : s) {
        if (c == '\033') e = true;
        else if (e && c == 'm') e = false;
        else if (!e) {
            if ((unsigned char)c < 0x80 || (unsigned char)c >= 0xC0) ++l;
        }
    }
    return l;
}

static std::string pad(const std::string& s, int w) {
    int v = visLen(s);
    return v >= w ? s : s + std::string(w - v, ' ');
}

static const char* rarityCol(Rarity r) {
    switch (r) {
        case common:    return COL_COM;
        case magic:     return COL_MAG;
        case rare:      return COL_RARE;
        case legendary: return COL_LEG;
        case divine:    return COL_DIV;
    }
    return RST;
}

static const char* typeCol(ItemType t) {
    switch (t) {
        case potion:   return GRN;
        case sword:    return RED;
        case armor:    return BLUE;
        case movement: return ORG;
    }
    return RST;
}

static bool shopItemMatches(Item* item,
                            const std::string& searchQuery,
                            bool rarityFilterEnabled, Rarity rarityFilter,
                            bool typeFilterEnabled,   ItemType typeFilter)
{
    if (!item) return false;
    if (rarityFilterEnabled && item->getRarity() != rarityFilter) return false;
    if (typeFilterEnabled   && item->getType()   != typeFilter)   return false;
    if (searchQuery.empty()) return true;
    std::string lq = searchQuery;
    std::transform(lq.begin(), lq.end(), lq.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    std::string n = item->getName();
    std::transform(n.begin(), n.end(), n.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    std::string tp = Item::typeToString(item->getType());
    std::transform(tp.begin(), tp.end(), tp.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return n.find(lq) != std::string::npos || tp.find(lq) != std::string::npos;
}

// ── Constructor / Destructor ──────────────────────────────────────────────
Shop::Shop() {
    itemGenerator = new ItemGenerator();
    int roll = rand() % 100;
    if (roll < 70)      playerGold = 200 + (rand() % 201);
    else if (roll < 90) playerGold = 400 + (rand() % 101);
    else                playerGold = 500 + (rand() % 301);
    selectedIndex       = 0;
    searchQuery         = "";
    rarityFilterEnabled = false;
    rarityFilter        = common;
    typeFilterEnabled   = false;
    typeFilter          = potion;
    generateShopInventory(10);
}

Shop::~Shop() {
    for (Item* i : shopInventory) delete i;
    shopInventory.clear();
    delete itemGenerator;
}

void Shop::generateShopInventory(int count) {
    for (Item* i : shopInventory) delete i;
    shopInventory.clear();
    // Use shop-specific generator so legendary/divine can appear
    for (int i = 0; i < count; i++)
        shopInventory.push_back(itemGenerator->generateShopItem());
    selectedIndex = 0;
}

// ── Shared row renderer ────────────────────────────────────────────────────
void Shop::printShopRow(int actualIdx, bool selected) const {
    Item* item = shopInventory[actualIdx];
    Rarity  r  = item->getRarity();
    ItemType t = item->getType();
    const char* rc = rarityCol(r);
    const char* tc = typeCol(t);

    // selector arrow
    std::cout << (selected ? GOLD " \u25b6 " : DIM "   ");

    // index number
    std::string idx = std::to_string(actualIdx + 1);
    std::cout << DIM << pad(idx, 2) << "  " RST;

    // name (rarity-coloured, padded to 22)
    std::string name = item->getName();
    std::cout << rc << pad(name, 22) << RST "  ";

    // type badge
    std::string typStr = Item::typeToString(t);
    std::cout << tc << pad(typStr, 8) << RST "  ";

    // rarity badge (padded to 10)
    std::string rarStr = Item::rarityToString(r);
    std::cout << rc << pad(rarStr, 10) << RST "  ";

    // price
    std::string priceStr = std::to_string(item->getPrice()) + "g";
    std::cout << GOLD << pad(priceStr, 6) << RST "  ";

    // effect
    std::string fxStr = "+" + std::to_string(item->getPercentValue()) + "%";
    std::cout << GRN << fxStr << RST "\n";
}

// ── displayShop ───────────────────────────────────────────────────────────
void Shop::displayShop() {
    // Header
    std::cout << GOLD DTL << rep(DH, 70) << DTR RST "\n";
    std::cout << GOLD DV LGOLD "  \u2741  SHOP OF CHALDEA  \u2741  "
              << DIM "Treasury: " GOLD << playerGold << "g";
    int gl = 38 + (int)std::to_string(playerGold).size();
    for (int i = gl; i < 69; i++) std::cout << ' ';
    std::cout << GOLD DV RST "\n";
    std::cout << GOLD DLT << rep(DH, 70) << DRT RST "\n";

    // Column headers
    std::cout << DIM "    #   "
              << pad("NAME", 22) << "  "
              << pad("TYPE", 8)  << "  "
              << pad("RARITY", 10) << "  "
              << pad("PRICE", 6)  << "  "
              << "EFFECT" RST "\n";
    std::cout << DIM << rep(SH, 70) << RST "\n";

    for (int i = 0; i < (int)shopInventory.size(); i++)
        printShopRow(i, i == selectedIndex);

    std::cout << GOLD << rep(SH, 70) << RST "\n";
}

// ── displayShopWithSearch ─────────────────────────────────────────────────
void Shop::displayShopWithSearch() {
    std::vector<int> results = getSearchResults();

    std::cout << GOLD DTL << rep(DH, 70) << DTR RST "\n";
    std::cout << GOLD DV LGOLD "  \u2741  SHOP  \u2741  "
              << DIM "Filter: \"" CYAN << searchQuery << DIM "\""
              << DIM "  Results: " WHITE << results.size();
    int gl2 = 28 + (int)searchQuery.size() + (int)std::to_string(results.size()).size();
    for (int i = gl2; i < 69; i++) std::cout << ' ';
    std::cout << GOLD DV RST "\n";
    std::cout << GOLD DLT << rep(DH, 70) << DRT RST "\n";

    if (results.empty()) {
        std::cout << DIM "  No items matched the search/filter." RST "\n";
        std::cout << GOLD << rep(SH, 70) << RST "\n";
        return;
    }

    std::cout << DIM "    #   "
              << pad("NAME", 22) << "  "
              << pad("TYPE", 8)  << "  "
              << pad("RARITY", 10) << "  "
              << pad("PRICE", 6)  << "  "
              << "EFFECT" RST "\n";
    std::cout << DIM << rep(SH, 70) << RST "\n";

    for (int idx : results)
        printShopRow(idx, idx == selectedIndex);

    std::cout << GOLD << rep(SH, 70) << RST "\n";
}

// ── buy / sell ────────────────────────────────────────────────────────────
bool Shop::buyItem(int index, Inventory* inv) {
    if (index < 0 || index >= (int)shopInventory.size()) return false;
    Item* item = shopInventory[index];
    if (playerGold < item->getPrice()) return false;
    if (!inv->addItemAtRandomPosition(item)) return false;
    playerGold -= item->getPrice();
    shopInventory.erase(shopInventory.begin() + index);
    if (selectedIndex >= (int)shopInventory.size() && selectedIndex > 0) selectedIndex--;
    return true;
}

Item* Shop::buyItemNoPlace(int index) {
    if (index < 0 || index >= (int)shopInventory.size()) return nullptr;
    Item* item = shopInventory[index];
    if (playerGold < item->getPrice()) return nullptr;
    playerGold -= item->getPrice();
    shopInventory.erase(shopInventory.begin() + index);
    if (selectedIndex >= (int)shopInventory.size() && selectedIndex > 0) selectedIndex--;
    return item;
}

bool Shop::sellItem(Item* item) {
    if (!item) return false;
    playerGold += item->getPrice() / 2;
    shopInventory.push_back(item);
    return true;
}

bool Shop::sellItemForPrice(Item* item, int price) {
    if (!item) return false;
    playerGold += price;
    shopInventory.push_back(item);
    return true;
}

// ── navigation ────────────────────────────────────────────────────────────
void Shop::selectNext() {
    if (shopInventory.empty()) return;
    selectedIndex = (selectedIndex + 1) % (int)shopInventory.size();
}
void Shop::selectPrevious() {
    if (shopInventory.empty()) return;
    selectedIndex = (selectedIndex - 1 + (int)shopInventory.size()) % (int)shopInventory.size();
}
void Shop::selectNextFiltered(const std::vector<int>& r) {
    if (r.empty()) return;
    int pos = 0;
    for (int i = 0; i < (int)r.size(); i++) if (r[i] == selectedIndex) { pos = i; break; }
    selectedIndex = r[(pos + 1) % (int)r.size()];
}
void Shop::selectPreviousFiltered(const std::vector<int>& r) {
    if (r.empty()) return;
    int pos = 0;
    for (int i = 0; i < (int)r.size(); i++) if (r[i] == selectedIndex) { pos = i; break; }
    selectedIndex = r[(pos - 1 + (int)r.size()) % (int)r.size()];
}

// ── search / filter ───────────────────────────────────────────────────────
void Shop::setSearchQuery(const std::string& q) { searchQuery = q; }
std::string Shop::getSearchQuery() const { return searchQuery; }
void Shop::clearSearch() { searchQuery = ""; }

std::vector<int> Shop::getSearchResults() {
    std::vector<int> results;
    for (int i = 0; i < (int)shopInventory.size(); i++)
        if (shopItemMatches(shopInventory[i], searchQuery,
                            rarityFilterEnabled, rarityFilter,
                            typeFilterEnabled, typeFilter))
            results.push_back(i);
    return results;
}

void Shop::setRarityFilter(Rarity r)   { rarityFilterEnabled = true;  rarityFilter = r; }
void Shop::clearRarityFilter()          { rarityFilterEnabled = false; }
bool Shop::isRarityFilterEnabled() const{ return rarityFilterEnabled; }
void Shop::setTypeFilter(ItemType t)    { typeFilterEnabled = true;   typeFilter   = t; }
void Shop::clearTypeFilter()            { typeFilterEnabled = false; }
bool Shop::isTypeFilterEnabled()  const { return typeFilterEnabled; }
void Shop::clearAllFilters()            { searchQuery = ""; rarityFilterEnabled = false; typeFilterEnabled = false; }

// ── gold helpers ──────────────────────────────────────────────────────────
int  Shop::getPlayerGold()    const { return playerGold; }
void Shop::setPlayerGold(int g)     { playerGold = g; }
void Shop::addGold(int a)           { playerGold += a; }
bool Shop::removeGold(int a)        { if (playerGold < a) return false; playerGold -= a; return true; }

// ── misc ──────────────────────────────────────────────────────────────────
int  Shop::getSelectedIndex()  const { return selectedIndex; }
void Shop::setSelectedIndex(int i)   { if (i >= 0 && i < (int)shopInventory.size()) selectedIndex = i; }
Item* Shop::getItemAt(int i)         { return (i >= 0 && i < (int)shopInventory.size()) ? shopInventory[i] : nullptr; }
int  Shop::getShopInventorySize() const { return (int)shopInventory.size(); }
void Shop::addItemToShop(Item* i)    { shopInventory.push_back(i); }
void Shop::removeItemFromShop(int i) { if (i >= 0 && i < (int)shopInventory.size()) { delete shopInventory[i]; shopInventory.erase(shopInventory.begin() + i); } }
void Shop::restockShop()             { generateShopInventory(10); }
Item* Shop::getSelectedItem()        { return (selectedIndex >= 0 && selectedIndex < (int)shopInventory.size()) ? shopInventory[selectedIndex] : nullptr; }
