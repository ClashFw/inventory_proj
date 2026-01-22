#include "item.h"

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
    srand(time(NULL));
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
