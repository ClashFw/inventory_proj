#include "item.h"
#include <iostream>

std::string Item::getName() const
{
    return name;
}

void Item::setName(const std::string &newName)
{
    name = newName;
}

Rarity Item::getRarity() const
{
    return rarity;
}

void Item::setRarity(Rarity newRarity)
{
    rarity = newRarity;
}

int Item::getDurability() const
{
    return durability;
}

void Item::setDurability(int newDurability)
{
    durability = newDurability;
}

int Item::getLevel() const
{
    return level;
}

void Item::setLevel(int newLevel)
{
    level = newLevel;
}

int Item::getPrice() const
{
    return price;
}

void Item::setPrice(int newPrice)
{
    price = newPrice;
}

void Item::use()
{

}

Item::Item() {
    name = "Stick";
    price = 1;
    durability = 10;
    level = 1;
    rarity = magic;

}

Item::Item(const std::string &name, int price)
    : name(name),
    durability(100),
    level(1),
    price(price)
{
    int randNum = rand() % 101;
    if(randNum < 60) {
        rarity = common;
    }
    else if(randNum >= 60 && randNum < 90) {
        rarity = magic;
    }
    else {
        rarity = rare;
    }
}

void Item::displayItemInfo() const
{
    std::cout << "\n=== Item Information ===" << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Price: " << price << std::endl;
    std::cout << "Durability: " << durability << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Rarity: ";
    switch(rarity) {
        case common:
            std::cout << "Common";
            break;
        case magic:
            std::cout << "Magic";
            break;
        case rare:
            std::cout << "Rare";
            break;
    }
    std::cout << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "Press any key to continue..." << std::endl;
}
