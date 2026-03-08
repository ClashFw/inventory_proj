#include "shop.h"
#include <iostream>
#include <algorithm>
#include <cctype>

Shop::Shop()
{
    itemGenerator = new ItemGenerator();
    // Gold: mostly 200-400, small chance up to 500, very rare above 500
    int roll = rand() % 100;
    if(roll < 70) {
        playerGold = 200 + (rand() % 201);  // 70%: 200-400
    } else if(roll < 90) {
        playerGold = 400 + (rand() % 101);  // 20%: 400-500
    } else {
        playerGold = 500 + (rand() % 301);  // 10%: 500-800
    }
    selectedIndex = 0;
    searchQuery = "";
    generateShopInventory(20); // Generate 20 items for shop
}

Shop::~Shop()
{
    // Clean up shop inventory
    for(Item* item : shopInventory) {
        delete item;
    }
    shopInventory.clear();
    delete itemGenerator;
}

void Shop::generateShopInventory(int itemCount)
{
    // Clear existing inventory
    for(Item* item : shopInventory) {
        delete item;
    }
    shopInventory.clear();

    // Generate new items
    std::vector<Item*> newItems = itemGenerator->generateRandomItems(itemCount, itemCount);
    shopInventory = newItems;
    selectedIndex = 0;
}

void Shop::displayShop()
{
    std::cout << "=== SHOP ===" << std::endl;
    std::cout << "Your Gold: " << playerGold << "g" << std::endl;
    std::cout << "Items in shop: " << shopInventory.size() << std::endl;
    std::cout << std::endl;

    if(shopInventory.empty()) {
        std::cout << "Shop is empty! Press 'R' to restock." << std::endl;
        return;
    }

    std::cout << "ID | Item Name    | Price | Rarity   | Durability | Level" << std::endl;
    std::cout << "---+-------------+-------+----------+------------+------" << std::endl;

    for(size_t i = 0; i < shopInventory.size(); i++) {
        Item* item = shopInventory[i];

        // Highlight selected item
        if(i == selectedIndex) {
            std::cout << ">>";
        } else {
            std::cout << "  ";
        }

        // ID
        std::cout << (i + 1);
        if(i + 1 < 10) std::cout << "  | ";
        else std::cout << " | ";

        // Name (padded to 12 chars)
        std::string name = item->getName();
        std::cout << name;
        for(size_t j = name.length(); j < 12; j++) std::cout << " ";
        std::cout << " | ";

        // Price
        std::cout << item->getPrice();
        if(item->getPrice() < 10) std::cout << "    | ";
        else if(item->getPrice() < 100) std::cout << "   | ";
        else std::cout << "  | ";

        // Rarity
        switch(item->getRarity()) {
            case common:
                std::cout << "Common  ";
                break;
            case magic:
                std::cout << "Magic   ";
                break;
            case rare:
                std::cout << "Rare    ";
                break;
        }
        std::cout << " | ";

        // Durability
        std::cout << item->getDurability();
        if(item->getDurability() < 10) std::cout << "          | ";
        else if(item->getDurability() < 100) std::cout << "         | ";
        else std::cout << "        | ";

        // Level
        std::cout << item->getLevel();

        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  W/S - Select item | B - Buy | E - Exit shop" << std::endl;
    std::cout << "  F - Search | R - Restock | C - Clear search" << std::endl;
}

void Shop::displayShopWithSearch()
{
    std::cout << "=== SHOP (SEARCH MODE) ===" << std::endl;
    std::cout << "Your Gold: " << playerGold << "g" << std::endl;
    std::cout << "Search: \"" << searchQuery << "\"" << std::endl;
    std::cout << std::endl;

    std::vector<int> results = getSearchResults();

    if(results.empty()) {
        std::cout << "No items found matching \"" << searchQuery << "\"" << std::endl;
        std::cout << "Press 'C' to clear search." << std::endl;
        return;
    }

    std::cout << "Found " << results.size() << " items:" << std::endl;
    std::cout << std::endl;
    std::cout << "ID | Item Name    | Price | Rarity   | Durability | Level" << std::endl;
    std::cout << "---+-------------+-------+----------+------------+------" << std::endl;

    for(size_t i = 0; i < results.size(); i++) {
        int actualIndex = results[i];
        Item* item = shopInventory[actualIndex];

        // Highlight selected item
        if(actualIndex == selectedIndex) {
            std::cout << ">>";
        } else {
            std::cout << "  ";
        }

        // ID
        std::cout << (actualIndex + 1);
        if(actualIndex + 1 < 10) std::cout << "  | ";
        else std::cout << " | ";

        // Name (padded to 12 chars)
        std::string name = item->getName();
        std::cout << name;
        for(size_t j = name.length(); j < 12; j++) std::cout << " ";
        std::cout << " | ";

        // Price
        std::cout << item->getPrice();
        if(item->getPrice() < 10) std::cout << "    | ";
        else if(item->getPrice() < 100) std::cout << "   | ";
        else std::cout << "  | ";

        // Rarity
        switch(item->getRarity()) {
            case common:
                std::cout << "Common  ";
                break;
            case magic:
                std::cout << "Magic   ";
                break;
            case rare:
                std::cout << "Rare    ";
                break;
        }
        std::cout << " | ";

        // Durability
        std::cout << item->getDurability();
        if(item->getDurability() < 10) std::cout << "          | ";
        else if(item->getDurability() < 100) std::cout << "         | ";
        else std::cout << "        | ";

        // Level
        std::cout << item->getLevel();

        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Controls: W/S - Select | B - Buy | C - Clear search | E - Exit" << std::endl;
}

bool Shop::buyItem(int index, Inventory* playerInventory)
{
    if(index < 0 || index >= (int)shopInventory.size()) return false;
    Item* item = shopInventory[index];
    if(playerGold < item->getPrice()) return false;
    if(!playerInventory->addItemAtRandomPosition(item)) return false;
    playerGold -= item->getPrice();
    shopInventory.erase(shopInventory.begin() + index);
    if(selectedIndex >= (int)shopInventory.size() && selectedIndex > 0) selectedIndex--;
    return true;
}

Item* Shop::buyItemNoPlace(int index)
{
    if(index < 0 || index >= (int)shopInventory.size()) return nullptr;
    Item* item = shopInventory[index];
    if(playerGold < item->getPrice()) return nullptr;
    playerGold -= item->getPrice();
    shopInventory.erase(shopInventory.begin() + index);
    if(selectedIndex >= (int)shopInventory.size() && selectedIndex > 0) selectedIndex--;
    return item;
}

bool Shop::sellItem(Item* item)
{
    if(item == nullptr) return false;
    int sellPrice = item->getPrice() / 2;
    playerGold += sellPrice;
    shopInventory.push_back(item);
    return true;
}

bool Shop::sellItemForPrice(Item* item, int price)
{
    if(item == nullptr) return false;
    playerGold += price;
    shopInventory.push_back(item);
    return true;
}

void Shop::selectNext()
{
    if(shopInventory.empty()) return;

    selectedIndex++;
    if(selectedIndex >= (int)shopInventory.size()) {
        selectedIndex = 0; // Wrap around
    }
}

void Shop::selectPrevious()
{
    if(shopInventory.empty()) return;

    selectedIndex--;
    if(selectedIndex < 0) {
        selectedIndex = shopInventory.size() - 1; // Wrap around
    }
}

int Shop::getSelectedIndex() const
{
    return selectedIndex;
}

void Shop::setSearchQuery(const std::string& query)
{
    searchQuery = query;
}

std::string Shop::getSearchQuery() const
{
    return searchQuery;
}

void Shop::clearSearch()
{
    searchQuery = "";
}

std::vector<int> Shop::getSearchResults()
{
    std::vector<int> results;
    
    if(searchQuery.empty()) {
        // Return all indices
        for(size_t i = 0; i < shopInventory.size(); i++) {
            results.push_back(i);
        }
        return results;
    }
    
    // Convert search query to lowercase
    std::string lowerQuery = searchQuery;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    // Search for items
    for(size_t i = 0; i < shopInventory.size(); i++) {
        std::string itemName = shopInventory[i]->getName();
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
        
        if(itemName.find(lowerQuery) != std::string::npos) {
            results.push_back(i);
        }
    }
    
    return results;
}

int Shop::getPlayerGold() const
{
    return playerGold;
}

void Shop::setPlayerGold(int gold)
{
    playerGold = gold;
}

void Shop::addGold(int amount)
{
    playerGold += amount;
}

bool Shop::removeGold(int amount)
{
    if(playerGold < amount) {
        return false;
    }
    playerGold -= amount;
    return true;
}

Item* Shop::getItemAt(int index)
{
    if(index < 0 || index >= (int)shopInventory.size()) {
        return nullptr;
    }
    return shopInventory[index];
}

int Shop::getShopInventorySize() const
{
    return shopInventory.size();
}

void Shop::addItemToShop(Item* item)
{
    shopInventory.push_back(item);
}

void Shop::removeItemFromShop(int index)
{
    if(index >= 0 && index < (int)shopInventory.size()) {
        delete shopInventory[index];
        shopInventory.erase(shopInventory.begin() + index);
    }
}

void Shop::restockShop()
{
    generateShopInventory(20);
}

Item* Shop::getSelectedItem()
{
    if(selectedIndex >= 0 && selectedIndex < (int)shopInventory.size()) {
        return shopInventory[selectedIndex];
    }
    return nullptr;
}