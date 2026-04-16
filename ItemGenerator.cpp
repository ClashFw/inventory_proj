#include "ItemGenerator.h"
#include <cstdlib>

ItemGenerator::ItemGenerator() {
    itemTypes = { potion, sword, armor, movement };
    minPrice = 5;
    maxPrice = 100;
}

ItemType ItemGenerator::getRandomItemType() const {
    int randomIndex = rand() % itemTypes.size();
    return itemTypes[randomIndex];
}

Rarity ItemGenerator::getRandomRarity() const {
    int randNum = rand() % 100;

    if (randNum < 40) return common;
    if (randNum < 70) return magic;
    return rare;
}

std::string ItemGenerator::getItemNameFromType(ItemType type) const {
    switch (type) {
        case potion: return "Potion";
        case sword: return "Sword";
        case armor: return "Armor";
        case movement: return "Movement";
    }
    return "Potion";
}

int ItemGenerator::getPercentForRarity(ItemType type, Rarity rarity) const {
    switch (type) {
        case potion:
            switch (rarity) {
                case common: return 10;
                case magic: return 20;
                case rare: return 35;
            }
            break;
        case sword:
            switch (rarity) {
                case common: return 7;
                case magic: return 12;
                case rare: return 17;
            }
            break;
        case armor:
            switch (rarity) {
                case common: return 6;
                case magic: return 14;
                case rare: return 24;
            }
            break;
        case movement:
            switch (rarity) {
                case common: return 5;
                case magic: return 10;
                case rare: return 18;
            }
            break;
    }
    return 0;
}

int ItemGenerator::getPriceFor(ItemType type, Rarity rarity) const {
    switch (type) {
        case potion:
            switch (rarity) {
                case common: return 15;
                case magic: return 30;
                case rare: return 55;
            }
            break;
        case sword:
            switch (rarity) {
                case common: return 25;
                case magic: return 50;
                case rare: return 90;
            }
            break;
        case armor:
            switch (rarity) {
                case common: return 25;
                case magic: return 50;
                case rare: return 90;
            }
            break;
        case movement:
            switch (rarity) {
                case common: return 20;
                case magic: return 40;
                case rare: return 75;
            }
            break;
    }
    return 10;
}

Item* ItemGenerator::generateRandomItem() {
    ItemType type = getRandomItemType();
    Rarity rarity = getRandomRarity();

    std::string name = getItemNameFromType(type);
    int price = getPriceFor(type, rarity);
    int percent = getPercentForRarity(type, rarity);

    Item* newItem = new Item(name, price, type);
    newItem->setRarity(rarity);
    newItem->setPercentValue(percent);
    newItem->setDurability(100);
    newItem->setLevel(1);

    return newItem;
}

std::vector<Item*> ItemGenerator::generateRandomItems(int minCount, int maxCount) {
    std::vector<Item*> items;
    int count = minCount + (rand() % (maxCount - minCount + 1));

    for (int i = 0; i < count; i++) {
        items.push_back(generateRandomItem());
    }

    return items;
}

void ItemGenerator::setMinPrice(int price) {
    minPrice = price;
}

void ItemGenerator::setMaxPrice(int price) {
    maxPrice = price;
}
