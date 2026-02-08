#include "ItemGenerator.h"
#include <cstdlib>

ItemGenerator::ItemGenerator()
{
    // Initialize the list of possible item names
    itemNames = {
        "Sword",
        "Shield",
        "Potion",
        "Helmet",
        "Boots",
        "Ring",
        "Amulet",
        "Bow",
        "Arrow",
        "Armor"
    };

    minPrice = 5;
    maxPrice = 100;
}

std::string ItemGenerator::getRandomItemName()
{
    int randomIndex = rand() % itemNames.size();
    return itemNames[randomIndex];
}

int ItemGenerator::getRandomPrice()
{
    return minPrice + (rand() % (maxPrice - minPrice + 1));
}

Rarity ItemGenerator::getRandomRarity()
{
    int randNum = rand() % 100;

    if(randNum < 60) {
        return common;
    }
    else if(randNum < 90) {
        return magic;
    }
    else {
        return rare;
    }
}

Item* ItemGenerator::generateRandomItem()
{
    std::string name = getRandomItemName();
    int price = getRandomPrice();

    Item* newItem = new Item(name, price);

    // Override the rarity with a new random one
    newItem->setRarity(getRandomRarity());

    // Set durability to always 100
    newItem->setDurability(100);

    // Set level to always 1
    newItem->setLevel(1);

    return newItem;
}

std::vector<Item*> ItemGenerator::generateRandomItems(int minCount, int maxCount)
{
    std::vector<Item*> items;

    // Generate random count between min and max
    int count = minCount + (rand() % (maxCount - minCount + 1));

    for(int i = 0; i < count; i++) {
        items.push_back(generateRandomItem());
    }

    return items;
}

void ItemGenerator::setMinPrice(int price)
{
    minPrice = price;
}

void ItemGenerator::setMaxPrice(int price)
{
    maxPrice = price;
}