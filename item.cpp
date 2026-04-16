#include "item.h"

Item::Item()
    : name("Potion"),
      rarity(common),
      type(potion),
      durability(100),
      level(1),
      price(10),
      percentValue(10)
{
}

Item::Item(const std::string& name, int price, ItemType type)
    : name(name),
      rarity(common),
      type(type),
      durability(100),
      level(1),
      price(price),
      percentValue(10)
{
}

std::string Item::getName() const {
    return name;
}

void Item::setName(const std::string& newName) {
    name = newName;
}

Rarity Item::getRarity() const {
    return rarity;
}

void Item::setRarity(Rarity newRarity) {
    rarity = newRarity;
}

ItemType Item::getType() const {
    return type;
}

void Item::setType(ItemType newType) {
    type = newType;
}

int Item::getDurability() const {
    return durability;
}

void Item::setDurability(int newDurability) {
    durability = newDurability;
}

int Item::getLevel() const {
    return level;
}

void Item::setLevel(int newLevel) {
    level = newLevel;
}

int Item::getPrice() const {
    return price;
}

void Item::setPrice(int newPrice) {
    price = newPrice;
}

int Item::getPercentValue() const {
    return percentValue;
}

void Item::setPercentValue(int newPercentValue) {
    percentValue = newPercentValue;
}

void Item::use() {
}

std::string Item::rarityToString(Rarity rarity) {
    switch (rarity) {
        case common: return "Common";
        case magic: return "Magic";
        case rare: return "Rare";
    }
    return "Unknown";
}

std::string Item::typeToString(ItemType type) {
    switch (type) {
        case potion: return "Potion";
        case sword: return "Sword";
        case armor: return "Armor";
        case movement: return "Movement";
    }
    return "Unknown";
}

void Item::displayItemInfo() const {
    std::cout << "\n=== Item Information ===" << std::endl;
    std::cout << "\n Name: " << name << std::endl;
    std::cout << "\n Type: " << typeToString(type) << std::endl;
    std::cout << "Price: " << price << "g" << std::endl;
    std::cout << "Durability: " << durability << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Rarity: " << rarityToString(rarity) << std::endl;
    std::cout << "Effect: " << percentValue << "%" << std::endl;
    std::cout << "========================" << std::endl;
}
