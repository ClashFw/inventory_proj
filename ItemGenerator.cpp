#include "ItemGenerator.h"
#include <cstdlib>
#include <cstring>

// -----------------------------------------------------------------------
// FGO-themed item name pools  [type][rarity]
// -----------------------------------------------------------------------
static const char* POTION_NAMES[5][5] = {
    // common
    { "Mana Herb",   "Flux Draught",  "Sap of Ansuz", "Brine Tonic",  "Verdant Vial" },
    // magic
    { "Mana Prism",  "Spirit Seed",   "Mystic Code",  "Ether Shard",  "Alter Elixir" },
    // rare
    { "Dragon Lung", "Grail Fragment","Phoenix Ash",  "Void Catalyst","Witch's Brew" },
    // legendary
    { "Philosopher Stone", "Ambrosia Flask", "Undying Serum", "Azoth Vial", "Prima Materia" },
    // divine
    { "Elixir of Ea", "Nectar of Alaya", "Gorgon Blood", "Avalon Dew", "Holy Grail Sip" }
};

static const char* SWORD_NAMES[5][5] = {
    // common
    { "Iron Falchion", "Rusted Saber",  "Knightly Blade","Steel Edge",   "Guard Sword" },
    // magic
    { "Caliburn Shard","Arondight Copy","Traced Gram",   "Mystic Blade", "Rune Glaive" },
    // rare
    { "Caliburn",      "Hrunting",      "Caladbolg II",  "Gram",         "Fragarach" },
    // legendary
    { "Arondight",     "Durandal",      "Curtana",       "Galatine",     "Secace" },
    // divine
    { "Excalibur",     "Ea",            "Claíomh Solais","Crocea Mors",  "Collbrande" }
};

static const char* ARMOR_NAMES[5][5] = {
    // common
    { "Leather Vest",  "Chain Hauberk", "Iron Pauldron", "Crude Shield",  "Soldier Greave" },
    // magic
    { "Mystic Plate",  "Rune Cuirass",  "Mage Mantle",   "Warding Mail",  "Barrier Coat" },
    // rare
    { "Millennium Chain","Berserker Hide","Rider Carapace","Caster Robe",  "Lancer Vambraces" },
    // legendary
    { "Armor of Fafnir","Hide of Nemea", "Twelve Labors", "Argonaut Aegis","Scale of Jormungandr" },
    // divine
    { "Rho Aias",       "Avalon",        "Camelot Plate", "Mirror of Medusa","Skin of Siegfried" }
};

static const char* MOVE_NAMES[5][5] = {
    // common
    { "Light Greaves",  "Wind Sandals",  "Swift Boots",   "Sprinter Wrap","Runner Shoes" },
    // magic
    { "Mystic Treads",  "Rune Gliders",  "Blink Step",    "Shadow Sole",  "Phantom Walk" },
    // rare
    { "Rider's Stirrup","Skadi's Stride","Swallow Wing",  "Ganesh Boots", "Banshee Veil" },
    // legendary
    { "Hermes Sandals", "Sleipnir Shoe", "Gungnir Dash",  "Valkyrie Stride","Erlang Leap" },
    // divine
    { "Gate of Babylon","Camelot Step",  "Merlin's Veil", "Primal Stride","Ley Line Walk" }
};

ItemGenerator::ItemGenerator() {
    itemTypes = { potion, sword, armor, movement };
    minPrice  = 5;
    maxPrice  = 100;
}

ItemType ItemGenerator::getRandomItemType() const {
    return itemTypes[rand() % itemTypes.size()];
}

// Rarity weights for random inventory drops (no legendary/divine there)
Rarity ItemGenerator::getRandomRarity() const {
    int r = rand() % 100;
    if (r < 45) return common;
    if (r < 75) return magic;
    return rare;
}

// Shop can also roll legendary (6%) and divine (2%)
Rarity ItemGenerator::getRandomShopRarity() const {
    int r = rand() % 100;
    if (r < 35) return common;
    if (r < 63) return magic;
    if (r < 85) return rare;
    if (r < 94) return legendary;
    return divine;
}

std::string ItemGenerator::getItemNameForRarity(ItemType type, Rarity rarity) const {
    int ri = (int)rarity;          // 0-4
    int ni = rand() % 5;
    switch (type) {
        case potion:   return POTION_NAMES[ri][ni];
        case sword:    return SWORD_NAMES [ri][ni];
        case armor:    return ARMOR_NAMES [ri][ni];
        case movement: return MOVE_NAMES  [ri][ni];
    }
    return "Unknown";
}

// kept for compat
std::string ItemGenerator::getItemNameFromType(ItemType type) const {
    return getItemNameForRarity(type, common);
}

int ItemGenerator::getPercentForRarity(ItemType type, Rarity rarity) const {
    // base percent per type, scaled by rarity tier
    int base = 0;
    switch (type) {
        case potion:   base = 12; break;
        case sword:    base =  8; break;
        case armor:    base =  7; break;
        case movement: base =  6; break;
    }
    // multipliers: common x1, magic x2, rare x3.5, legendary x5, divine x8
    float mult[] = { 1.0f, 2.0f, 3.5f, 5.0f, 8.0f };
    int result = (int)(base * mult[(int)rarity]);
    return result;
}

int ItemGenerator::getPriceFor(ItemType type, Rarity rarity) const {
    int base = 0;
    switch (type) {
        case potion:   base =  15; break;
        case sword:    base =  25; break;
        case armor:    base =  25; break;
        case movement: base =  20; break;
    }
    // price tiers: common x1, magic x2, rare x4, legendary x9, divine x18
    float mult[] = { 1.0f, 2.0f, 4.0f, 9.0f, 18.0f };
    return (int)(base * mult[(int)rarity]);
}

Item* ItemGenerator::generateRandomItem() {
    ItemType type   = getRandomItemType();
    Rarity   rarity = getRandomRarity();          // no legendary/divine in drops
    return buildItem(type, rarity, false);
}

Item* ItemGenerator::generateShopItem() {
    ItemType type   = getRandomItemType();
    Rarity   rarity = getRandomShopRarity();      // can roll legendary/divine
    return buildItem(type, rarity, rarity >= legendary);
}

Item* ItemGenerator::buildItem(ItemType type, Rarity rarity, bool shopOnly) const {
    std::string name    = getItemNameForRarity(type, rarity);
    int         price   = getPriceFor(type, rarity);
    int         percent = getPercentForRarity(type, rarity);

    Item* item = new Item(name, price, type);
    item->setRarity(rarity);
    item->setPercentValue(percent);
    item->setDurability(100);
    item->setLevel(1);
    item->setShopOnly(shopOnly);
    return item;
}

std::vector<Item*> ItemGenerator::generateRandomItems(int minCount, int maxCount) {
    std::vector<Item*> items;
    int count = minCount + (rand() % (maxCount - minCount + 1));
    for (int i = 0; i < count; i++)
        items.push_back(generateRandomItem());
    return items;
}

std::vector<Item*> ItemGenerator::generateShopItems(int count) {
    std::vector<Item*> items;
    for (int i = 0; i < count; i++)
        items.push_back(generateShopItem());
    return items;
}

void ItemGenerator::setMinPrice(int p) { minPrice = p; }
void ItemGenerator::setMaxPrice(int p) { maxPrice = p; }
