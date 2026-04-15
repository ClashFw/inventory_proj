#include "shop.h"
#include <iostream>
#include <algorithm>
#include <cctype>

static bool shopItemMatches(Item* item,
                            const std::string& searchQuery,
                            bool rarityFilterEnabled,
                            Rarity rarityFilter,
                            bool typeFilterEnabled,
                            ItemType typeFilter)
{
    if (!item) return false;

    if (rarityFilterEnabled && item->getRarity() != rarityFilter) return false;
    if (typeFilterEnabled && item->getType() != typeFilter) return false;

    if (searchQuery.empty()) return true;

    std::string lowerQuery = searchQuery;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });

    std::string itemName = item->getName();
    std::transform(itemName.begin(), itemName.end(), itemName.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });

    std::string itemType = Item::typeToString(item->getType());
    std::transform(itemType.begin(), itemType.end(), itemType.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });

    return itemName.find(lowerQuery) != std::string::npos ||
           itemType.find(lowerQuery) != std::string::npos;
}

Shop::Shop()
{
    itemGenerator = new ItemGenerator();

    int roll = rand() % 100;
    if (roll < 70) {
        playerGold = 200 + (rand() % 201);
    } else if (roll < 90) {
        playerGold = 400 + (rand() % 101);
    } else {
        playerGold = 500 + (rand() % 301);
    }

    selectedIndex = 0;
    searchQuery = "";

    rarityFilterEnabled = false;
    rarityFilter = common;

    typeFilterEnabled = false;
    typeFilter = potion;

    generateShopInventory(10);
}

Shop::~Shop()
{
    for (Item* item : shopInventory) {
        delete item;
    }
    shopInventory.clear();
    delete itemGenerator;
}

void Shop::generateShopInventory(int itemCount)
{
    for (Item* item : shopInventory) {
        delete item;
    }
    shopInventory.clear();

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

    std::cout << "ID | Name      | Type      | Price | Rarity | Effect" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;

    for (size_t i = 0; i < shopInventory.size(); i++) {
        Item* item = shopInventory[i];

        if ((int)i == selectedIndex) std::cout << ">>";
        else std::cout << "  ";

        std::cout << (i + 1);
        if (i + 1 < 10) std::cout << "  ";
        else std::cout << " ";

        std::string name = item->getName();
        std::cout << "| " << name;
        for (size_t j = name.length(); j < 9; j++) std::cout << " ";

        std::string type = Item::typeToString(item->getType());
        std::cout << "| " << type;
        for (size_t j = type.length(); j < 9; j++) std::cout << " ";

        std::cout << "| " << item->getPrice();
        if (item->getPrice() < 10) std::cout << "    ";
        else if (item->getPrice() < 100) std::cout << "   ";
        else std::cout << "  ";

        std::string rarity = Item::rarityToString(item->getRarity());
        std::cout << "| " << rarity;
        for (size_t j = rarity.length(); j < 6; j++) std::cout << " ";

        std::cout << "| " << item->getPercentValue() << "%" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "W/S-Select  B-Buy  F-Search  R-Restock  E-Exit" << std::endl;
}

void Shop::displayShopWithSearch()
{
    std::vector<int> results = getSearchResults();

    std::cout << "=== SHOP FILTERED ===" << std::endl;
    std::cout << "Your Gold: " << playerGold << "g" << std::endl;
    std::cout << "Search: \"" << searchQuery << "\"" << std::endl;
    std::cout << std::endl;

    if (results.empty()) {
        std::cout << "No items matched current search/filter." << std::endl;
        return;
    }

    std::cout << "ID | Name      | Type      | Price | Rarity | Effect" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;

    for (size_t i = 0; i < results.size(); i++) {
        int actualIndex = results[i];
        Item* item = shopInventory[actualIndex];

        if (actualIndex == selectedIndex) std::cout << ">>";
        else std::cout << "  ";

        std::cout << (actualIndex + 1);
        if (actualIndex + 1 < 10) std::cout << "  ";
        else std::cout << " ";

        std::string name = item->getName();
        std::cout << "| " << name;
        for (size_t j = name.length(); j < 9; j++) std::cout << " ";

        std::string type = Item::typeToString(item->getType());
        std::cout << "| " << type;
        for (size_t j = type.length(); j < 9; j++) std::cout << " ";

        std::cout << "| " << item->getPrice();
        if (item->getPrice() < 10) std::cout << "    ";
        else if (item->getPrice() < 100) std::cout << "   ";
        else std::cout << "  ";

        std::string rarity = Item::rarityToString(item->getRarity());
        std::cout << "| " << rarity;
        for (size_t j = rarity.length(); j < 6; j++) std::cout << " ";

        std::cout << "| " << item->getPercentValue() << "%" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Controls: W/S-Select  B-Buy  C-Clear filters/search  E-Exit" << std::endl;
}

bool Shop::buyItem(int index, Inventory* playerInventory)
{
    if (index < 0 || index >= (int)shopInventory.size()) return false;

    Item* item = shopInventory[index];
    if (playerGold < item->getPrice()) return false;
    if (!playerInventory->addItemAtRandomPosition(item)) return false;

    playerGold -= item->getPrice();
    shopInventory.erase(shopInventory.begin() + index);

    if (selectedIndex >= (int)shopInventory.size() && selectedIndex > 0) {
        selectedIndex--;
    }

    return true;
}

Item* Shop::buyItemNoPlace(int index)
{
    if (index < 0 || index >= (int)shopInventory.size()) return nullptr;

    Item* item = shopInventory[index];
    if (playerGold < item->getPrice()) return nullptr;

    playerGold -= item->getPrice();
    shopInventory.erase(shopInventory.begin() + index);

    if (selectedIndex >= (int)shopInventory.size() && selectedIndex > 0) {
        selectedIndex--;
    }

    return item;
}

bool Shop::sellItem(Item* item)
{
    if (item == nullptr) return false;
    int sellPrice = item->getPrice() / 2;
    playerGold += sellPrice;
    shopInventory.push_back(item);
    return true;
}

bool Shop::sellItemForPrice(Item* item, int price)
{
    if (item == nullptr) return false;
    playerGold += price;
    shopInventory.push_back(item);
    return true;
}

void Shop::selectNext()
{
    if (shopInventory.empty()) return;
    selectedIndex++;
    if (selectedIndex >= (int)shopInventory.size()) selectedIndex = 0;
}

void Shop::selectPrevious()
{
    if (shopInventory.empty()) return;
    selectedIndex--;
    if (selectedIndex < 0) selectedIndex = (int)shopInventory.size() - 1;
}

void Shop::selectNextFiltered(const std::vector<int>& results)
{
    if (results.empty()) return;

    int pos = 0;
    for (size_t i = 0; i < results.size(); i++) {
        if (results[i] == selectedIndex) {
            pos = (int)i;
            break;
        }
    }

    pos = (pos + 1) % (int)results.size();
    selectedIndex = results[pos];
}

void Shop::selectPreviousFiltered(const std::vector<int>& results)
{
    if (results.empty()) return;

    int pos = 0;
    for (size_t i = 0; i < results.size(); i++) {
        if (results[i] == selectedIndex) {
            pos = (int)i;
            break;
        }
    }

    pos = (pos - 1 + (int)results.size()) % (int)results.size();
    selectedIndex = results[pos];
}

int Shop::getSelectedIndex() const
{
    return selectedIndex;
}

void Shop::setSelectedIndex(int index)
{
    if (index >= 0 && index < (int)shopInventory.size()) {
        selectedIndex = index;
    }
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

    for (size_t i = 0; i < shopInventory.size(); i++) {
        if (shopItemMatches(shopInventory[i],
                            searchQuery,
                            rarityFilterEnabled, rarityFilter,
                            typeFilterEnabled, typeFilter)) {
            results.push_back((int)i);
        }
    }

    return results;
}

void Shop::setRarityFilter(Rarity rarity)
{
    rarityFilterEnabled = true;
    rarityFilter = rarity;
}

void Shop::clearRarityFilter()
{
    rarityFilterEnabled = false;
}

bool Shop::isRarityFilterEnabled() const
{
    return rarityFilterEnabled;
}

void Shop::setTypeFilter(ItemType type)
{
    typeFilterEnabled = true;
    typeFilter = type;
}

void Shop::clearTypeFilter()
{
    typeFilterEnabled = false;
}

bool Shop::isTypeFilterEnabled() const
{
    return typeFilterEnabled;
}

void Shop::clearAllFilters()
{
    searchQuery = "";
    rarityFilterEnabled = false;
    typeFilterEnabled = false;
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
    if (playerGold < amount) return false;
    playerGold -= amount;
    return true;
}

Item* Shop::getItemAt(int index)
{
    if (index < 0 || index >= (int)shopInventory.size()) return nullptr;
    return shopInventory[index];
}

int Shop::getShopInventorySize() const
{
    return (int)shopInventory.size();
}

void Shop::addItemToShop(Item* item)
{
    shopInventory.push_back(item);
}

void Shop::removeItemFromShop(int index)
{
    if (index >= 0 && index < (int)shopInventory.size()) {
        delete shopInventory[index];
        shopInventory.erase(shopInventory.begin() + index);
    }
}

void Shop::restockShop()
{
    generateShopInventory(10);
}

Item* Shop::getSelectedItem()
{
    if (selectedIndex >= 0 && selectedIndex < (int)shopInventory.size()) {
        return shopInventory[selectedIndex];
    }
    return nullptr;
}
