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

public:
    Shop();
    ~Shop();

    // Shop operations
    void generateShopInventory(int itemCount);
    void displayShop();
    void displayShopWithSearch();
    bool buyItem(int index, Inventory* playerInventory);
    bool sellItem(Item* item);

    // Navigation
    void selectNext();
    void selectPrevious();
    int getSelectedIndex() const;

    // Search
    void setSearchQuery(const std::string& query);
    std::string getSearchQuery() const;
    void clearSearch();
    std::vector<int> getSearchResults();

    // Gold management
    int getPlayerGold() const;
    void setPlayerGold(int gold);
    void addGold(int amount);
    bool removeGold(int amount);

    // Shop inventory management
    Item* getItemAt(int index);
    int getShopInventorySize() const;
    void addItemToShop(Item* item);
    void removeItemFromShop(int index);
    void restockShop();

    // Get selected item
    Item* getSelectedItem();
};

#endif // SHOP_H