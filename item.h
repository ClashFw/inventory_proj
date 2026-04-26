#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>

enum Rarity {
    common,
    magic,
    rare,
    legendary,
    divine
};

enum ItemType {
    potion,
    sword,
    armor,
    movement
};

class Item
{
private:
    std::string name;
    Rarity rarity;
    ItemType type;
    int durability;
    int level;
    int price;
    int percentValue;
    bool shopOnly;   // legendary / divine are shop-exclusive

public:
    Item();
    Item(const std::string& name, int price, ItemType type);

    std::string getName() const;
    void setName(const std::string& newName);

    Rarity getRarity() const;
    void setRarity(Rarity newRarity);

    ItemType getType() const;
    void setType(ItemType newType);

    int getDurability() const;
    void setDurability(int newDurability);

    int getLevel() const;
    void setLevel(int newLevel);

    int getPrice() const;
    void setPrice(int newPrice);

    int getPercentValue() const;
    void setPercentValue(int newPercentValue);

    bool isShopOnly() const;
    void setShopOnly(bool v);

    void use();
    void displayItemInfo() const;

    // ANSI colour string for this item
    std::string rarityColor() const;

    static std::string rarityToString(Rarity rarity);
    static std::string rarityColorStr(Rarity rarity);
    static std::string typeToString(ItemType type);
    static std::string typeColorStr(ItemType type);
};

#endif // ITEM_H