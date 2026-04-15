#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>

enum Rarity {
    common,
    magic,
    rare
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

    void use();
    void displayItemInfo() const;

    static std::string rarityToString(Rarity rarity);
    static std::string typeToString(ItemType type);
};

#endif // ITEM_H
