#ifndef SHOP_H
#define SHOP_H

#include "item.h"
#include "inventory.h"
#include "ItemGenerator.h"
#include <vector>
#include <string>

class Shop
{
private:
    std::vector<Item*> shopInventory;
    ItemGenerator* itemGenerator;
    int playerGold;
    int selectedIndex;
    std::string searchQuery;

    bool rarityFilterEnabled;
    Rarity rarityFilter;

    bool typeFilterEnabled;
    ItemType typeFilter;

public:
    Shop();
    ~Shop();

    void generateShopInventory(int itemCount);
    void displayShop();
    void displayShopWithSearch();
    bool buyItem(int index, Inventory* playerInventory);
    Item* buyItemNoPlace(int index);
    bool sellItem(Item* item);
    bool sellItemForPrice(Item* item, int price);

    void selectNext();
    void selectPrevious();
    void selectNextFiltered(const std::vector<int>& results);
    void selectPreviousFiltered(const std::vector<int>& results);
    int getSelectedIndex() const;
    void setSelectedIndex(int index);

    void setSearchQuery(const std::string& query);
    std::string getSearchQuery() const;
    void clearSearch();
    std::vector<int> getSearchResults();

    void setRarityFilter(Rarity rarity);
    void clearRarityFilter();
    bool isRarityFilterEnabled() const;

    void setTypeFilter(ItemType type);
    void clearTypeFilter();
    bool isTypeFilterEnabled() const;

    void clearAllFilters();

    int getPlayerGold() const;
    void setPlayerGold(int gold);
    void addGold(int amount);
    bool removeGold(int amount);

    Item* getItemAt(int index);
    int getShopInventorySize() const;
    void addItemToShop(Item* item);
    void removeItemFromShop(int index);
    void restockShop();

    Item* getSelectedItem();
};

#endif // SHOP_H
