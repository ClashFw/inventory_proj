#ifndef ITEMGENERATOR_H
#define ITEMGENERATOR_H

#include "item.h"
#include <string>
#include <vector>

class ItemGenerator
{
private:
    std::vector<std::string> itemNames;
    int minPrice;
    int maxPrice;

public:
    ItemGenerator();

    // Generate a single random item with random properties
    Item* generateRandomItem();

    // Generate multiple random items (between min and max count)
    std::vector<Item*> generateRandomItems(int minCount, int maxCount);

    // Get random item name from the list
    std::string getRandomItemName();

    // Get random price within range
    int getRandomPrice();

    // Get random rarity
    Rarity getRandomRarity();

    // Setters
    void setMinPrice(int price);
    void setMaxPrice(int price);
};

#endif // ITEMGENERATOR_H