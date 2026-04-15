#ifndef ITEMGENERATOR_H
#define ITEMGENERATOR_H

#include "item.h"
#include <string>
#include <vector>

class ItemGenerator
{
private:
    std::vector<ItemType> itemTypes;
    int minPrice;
    int maxPrice;

public:
    ItemGenerator();

    Item* generateRandomItem();
    std::vector<Item*> generateRandomItems(int minCount, int maxCount);

    ItemType getRandomItemType() const;
    Rarity getRandomRarity() const;

    std::string getItemNameFromType(ItemType type) const;
    int getPercentForRarity(ItemType type, Rarity rarity) const;
    int getPriceFor(ItemType type, Rarity rarity) const;

    void setMinPrice(int price);
    void setMaxPrice(int price);
};

#endif // ITEMGENERATOR_H
