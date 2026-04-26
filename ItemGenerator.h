#ifndef ITEMGENERATOR_H
#define ITEMGENERATOR_H

#include "item.h"
#include <vector>
#include <string>

class ItemGenerator {
public:
    ItemGenerator();

    Item* generateRandomItem();
    Item* generateShopItem();
    std::vector<Item*> generateRandomItems(int minCount, int maxCount);
    std::vector<Item*> generateShopItems(int count);

    void setMinPrice(int price);
    void setMaxPrice(int price);

    // kept public for compat
    std::string getItemNameFromType(ItemType type) const;
    int  getPercentForRarity(ItemType type, Rarity rarity) const;
    int  getPriceFor(ItemType type, Rarity rarity) const;
    ItemType getRandomItemType() const;
    Rarity   getRandomRarity() const;
    Rarity   getRandomShopRarity() const;

private:
    std::vector<ItemType> itemTypes;
    int minPrice, maxPrice;

    std::string getItemNameForRarity(ItemType type, Rarity rarity) const;
    Item* buildItem(ItemType type, Rarity rarity, bool shopOnly) const;
};

#endif // ITEMGENERATOR_H
