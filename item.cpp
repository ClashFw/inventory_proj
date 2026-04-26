#include "item.h"

Item::Item()
    : name("Potion"), rarity(common), type(potion),
      durability(100), level(1), price(10), percentValue(10), shopOnly(false)
{}

Item::Item(const std::string& name, int price, ItemType type)
    : name(name), rarity(common), type(type),
      durability(100), level(1), price(price), percentValue(10), shopOnly(false)
{}

std::string Item::getName()               const { return name; }
void        Item::setName(const std::string& n)  { name = n; }
Rarity      Item::getRarity()             const { return rarity; }
void        Item::setRarity(Rarity r)            { rarity = r; }
ItemType    Item::getType()               const { return type; }
void        Item::setType(ItemType t)            { type = t; }
int         Item::getDurability()         const { return durability; }
void        Item::setDurability(int v)           { durability = v; }
int         Item::getLevel()              const { return level; }
void        Item::setLevel(int v)                { level = v; }
int         Item::getPrice()              const { return price; }
void        Item::setPrice(int v)                { price = v; }
int         Item::getPercentValue()       const { return percentValue; }
void        Item::setPercentValue(int v)         { percentValue = v; }
bool        Item::isShopOnly()            const { return shopOnly; }
void        Item::setShopOnly(bool v)            { shopOnly = v; }

void Item::use() {}

// -----------------------------------------------------------------------
// Rarity colour  (256-colour ANSI)
// -----------------------------------------------------------------------
std::string Item::rarityColorStr(Rarity r) {
    switch (r) {
        case common:    return "\033[37m";          // white
        case magic:     return "\033[38;5;39m";     // bright blue
        case rare:      return "\033[38;5;220m";    // gold
        case legendary: return "\033[38;5;202m";   // deep orange
        case divine:    return "\033[38;5;201m";   // bright magenta/pink
    }
    return "\033[0m";
}

std::string Item::rarityColor() const { return rarityColorStr(rarity); }

// -----------------------------------------------------------------------
// Type colour
// -----------------------------------------------------------------------
std::string Item::typeColorStr(ItemType t) {
    switch (t) {
        case potion:   return "\033[38;5;82m";   // green
        case sword:    return "\033[38;5;196m";  // red
        case armor:    return "\033[38;5;33m";   // blue
        case movement: return "\033[38;5;214m";  // orange
    }
    return "\033[0m";
}

std::string Item::rarityToString(Rarity r) {
    switch (r) {
        case common:    return "Common";
        case magic:     return "Magic";
        case rare:      return "Rare";
        case legendary: return "Legendary";
        case divine:    return "Divine";
    }
    return "Unknown";
}

std::string Item::typeToString(ItemType t) {
    switch (t) {
        case potion:   return "Potion";
        case sword:    return "Sword";
        case armor:    return "Armor";
        case movement: return "Movement";
    }
    return "Unknown";
}

void Item::displayItemInfo() const {
    std::string rc = rarityColorStr(rarity);
    std::string tc = typeColorStr(type);
    const char* RST = "\033[0m";
    std::cout << "\n\033[38;5;220m=== Item Information ===\033[0m\n";
    std::cout << "  Name:   " << rc << name           << RST << "\n";
    std::cout << "  Type:   " << tc << typeToString(type) << RST << "\n";
    std::cout << "  Rarity: " << rc << rarityToString(rarity) << RST << "\n";
    std::cout << "  Price:  \033[38;5;220m" << price << "g" << RST << "\n";
    std::cout << "  Effect: \033[92m+" << percentValue << "%" << RST << "\n";
    std::cout << "  Level:  " << level << "\n";
    std::cout << "\033[38;5;220m========================\033[0m\n";
}