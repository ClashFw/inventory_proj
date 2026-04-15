#include "item.h"
#include <cstdlib>

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

int Item::getStatValue() const {
    return statValue;
}

void Item::setStatValue(int newStatValue) {
    statValue = newStatValue;
}

void Item::use() {
}

Item::Item() {
    name = "Potion";
    price = 10;
    durability = 100;
    level = 1;
    rarity = common;
    type = potion;
    statValue = 20;
}

Item::Item(const std::string& name, int price, ItemType type)
    : name(name), type(type), durability(100), level(1), price(price), statValue(0) {
    int randNum = rand() % 100;
    if(randNum < 60) rarity = common;
    else if(randNum < 90) rarity = magic;
    else rarity = rare;
}

void Item::displayItemInfo() const {
    std::cout << "\n=== Item Information ===" << std::endl;
    std::cout << "Name: " << name << std::endl;

    std::cout << "Type: ";
    switch(type) {
        case potion: std::cout << "Potion"; break;
        case sword: std::cout << "Sword"; break;
        case armor: std::cout << "Armor"; break;
        case movement: std::cout << "Movement"; break;
    }
    std::cout << std::endl;

    std::cout << "Price: " << price << std::endl;
    std::cout << "Durability: " << durability << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Value: " << statValue << std::endl;

    std::cout << "Rarity: ";
    switch(rarity) {
        case common: std::cout << "Common"; break;
        case magic: std::cout << "Magic"; break;
        case rare: std::cout << "Rare"; break;
    }
    std::cout << std::endl;
    std::cout << "========================" << std::endl;
}
