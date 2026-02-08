#include "inventory.h"
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>

int Inventory::getRows() const
{
    return rows;
}

void Inventory::setRows(int newRows)
{
    rows = newRows;
}

int Inventory::getCols() const
{
    return cols;
}

void Inventory::setCols(int newCols)
{
    cols = newCols;
}

Item ***Inventory::getItems() const
{
    return items;
}

void Inventory::setItems(Item ***newItems)
{
    items = newItems;
}

int Inventory::getCurrentRow() const
{
    return currentRow;
}

void Inventory::setCurrentRow(int newCurrentRow)
{
    if(newCurrentRow >= 0 && newCurrentRow < rows) {
        currentRow = newCurrentRow;
    }
    else if(newCurrentRow < 0) {
        currentRow = rows - 1;
    }
    else {
        currentRow = 0;
    }
}

int Inventory::getCurrentCol() const
{
    return currentCol;
}

void Inventory::setCurrentCol(int newCurrentCol)
{
    if(newCurrentCol >= 0 && newCurrentCol < cols) {
        currentCol = newCurrentCol;
    }
    else if(newCurrentCol < 0) {
        currentCol = cols - 1;
    }
    else {
        currentCol = 0;
    }
}

Inventory::Inventory()
{
    rows = 6;
    cols = 7;
    currentRow = 0;
    currentCol = 0;
    filterEnabled = false;
    currentFilter = common;

    items = new Item**[rows];
    for(int i = 0; i < rows; i++) {
        items[i] = new Item*[cols];
        for(int j = 0; j < cols; j++) {
            items[i][j] = nullptr;
        }
    }
}

Inventory::~Inventory()
{
    clear();
    for(int i = 0; i < rows; i++) {
        delete[] items[i];
    }
    delete[] items;
}

void Inventory::clear()
{
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(items[i][j] != nullptr) {
                delete items[i][j];
                items[i][j] = nullptr;
            }
        }
    }
}

void Inventory::setFilter(Rarity rarity)
{
    filterEnabled = true;
    currentFilter = rarity;
}

void Inventory::clearFilter()
{
    filterEnabled = false;
}

bool Inventory::isFilterEnabled() const
{
    return filterEnabled;
}

Rarity Inventory::getCurrentFilter() const
{
    return currentFilter;
}

int Inventory::getFilteredItemCount() const
{
    int count = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(items[i][j] != nullptr) {
                if(!filterEnabled || items[i][j]->getRarity() == currentFilter) {
                    count++;
                }
            }
        }
    }
    return count;
}

void Inventory::display()
{
    // Display filter status
    if(filterEnabled) {
        std::cout << "Filter: ";
        switch(currentFilter) {
            case common:
                std::cout << "Common";
                break;
            case magic:
                std::cout << "Magic";
                break;
            case rare:
                std::cout << "Rare";
                break;
        }
        std::cout << " (" << getFilteredItemCount() << " items)" << std::endl;
    } else {
        std::cout << "Filter: None (All items)" << std::endl;
    }
    std::cout << std::endl;

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            bool isCurrentPos = (i == currentRow && j == currentCol);
            bool shouldDisplay = true;

            // Check if item should be displayed based on filter
            if(items[i][j] != nullptr && filterEnabled) {
                shouldDisplay = (items[i][j]->getRarity() == currentFilter);
            }

            if(isCurrentPos) {
                std::cout << "{";
            } else {
                std::cout << "[";
            }

            if(items[i][j] == nullptr) {
                std::cout << " ";
            } else if(shouldDisplay) {
                std::cout << char(toupper(items[i][j]->getName()[0]));
            } else {
                std::cout << "·"; // Dimmed dot for filtered out items
            }

            if(isCurrentPos) {
                std::cout << "}";
            } else {
                std::cout << "]";
            }
        }
        std::cout << std::endl;
    }
}

void Inventory::displayWithItemInfo(Item* item)
{
    // Get item info lines
    std::string itemLines[9];
    itemLines[0] = "=== Item Information ===";
    itemLines[1] = "Name: " + item->getName();
    itemLines[2] = "Price: " + std::to_string(item->getPrice());
    itemLines[3] = "Durability: " + std::to_string(item->getDurability());
    itemLines[4] = "Level: " + std::to_string(item->getLevel());
    itemLines[5] = "Rarity: ";
    switch(item->getRarity()) {
        case common:
            itemLines[5] += "Common";
            break;
        case magic:
            itemLines[5] += "Magic";
            break;
        case rare:
            itemLines[5] += "Rare";
            break;
    }
    itemLines[6] = "========================";
    itemLines[7] = "";
    itemLines[8] = "Press 'I' to close";

    // Display filter status
    if(filterEnabled) {
        std::cout << "Filter: ";
        switch(currentFilter) {
            case common:
                std::cout << "Common";
                break;
            case magic:
                std::cout << "Magic";
                break;
            case rare:
                std::cout << "Rare";
                break;
        }
        std::cout << " (" << getFilteredItemCount() << " items)" << std::endl;
    } else {
        std::cout << "Filter: None (All items)" << std::endl;
    }
    std::cout << std::endl;

    // Display inventory and item info side by side
    for(int i = 0; i < rows; i++) {
        // Display inventory row
        for(int j = 0; j < cols; j++) {
            bool isCurrentPos = (i == currentRow && j == currentCol);
            bool shouldDisplay = true;

            if(items[i][j] != nullptr && filterEnabled) {
                shouldDisplay = (items[i][j]->getRarity() == currentFilter);
            }

            if(isCurrentPos) {
                std::cout << "{";
            } else {
                std::cout << "[";
            }

            if(items[i][j] == nullptr) {
                std::cout << " ";
            } else if(shouldDisplay) {
                std::cout << char(toupper(items[i][j]->getName()[0]));
            } else {
                std::cout << "·";
            }

            if(isCurrentPos) {
                std::cout << "}";
            } else {
                std::cout << "]";
            }
        }

        // Add spacing and item info
        std::cout << "    ";
        if(i < 9) {
            std::cout << itemLines[i];
        }
        std::cout << std::endl;
    }
}

void Inventory::displayWithEmptyInfo()
{
    // Get empty info lines
    std::string itemLines[9];
    itemLines[0] = "=== Item Information ===";
    itemLines[1] = "";
    itemLines[2] = "   No item selected";
    itemLines[3] = "";
    itemLines[4] = "";
    itemLines[5] = "";
    itemLines[6] = "========================";
    itemLines[7] = "";
    itemLines[8] = "Press 'I' to close";

    // Display filter status
    if(filterEnabled) {
        std::cout << "Filter: ";
        switch(currentFilter) {
            case common:
                std::cout << "Common";
                break;
            case magic:
                std::cout << "Magic";
                break;
            case rare:
                std::cout << "Rare";
                break;
        }
        std::cout << " (" << getFilteredItemCount() << " items)" << std::endl;
    } else {
        std::cout << "Filter: None (All items)" << std::endl;
    }
    std::cout << std::endl;

    // Display inventory and empty info side by side
    for(int i = 0; i < rows; i++) {
        // Display inventory row
        for(int j = 0; j < cols; j++) {
            bool isCurrentPos = (i == currentRow && j == currentCol);
            bool shouldDisplay = true;

            if(items[i][j] != nullptr && filterEnabled) {
                shouldDisplay = (items[i][j]->getRarity() == currentFilter);
            }

            if(isCurrentPos) {
                std::cout << "{";
            } else {
                std::cout << "[";
            }

            if(items[i][j] == nullptr) {
                std::cout << " ";
            } else if(shouldDisplay) {
                std::cout << char(toupper(items[i][j]->getName()[0]));
            } else {
                std::cout << "·";
            }

            if(isCurrentPos) {
                std::cout << "}";
            } else {
                std::cout << "]";
            }
        }

        // Add spacing and empty info
        std::cout << "    ";
        if(i < 9) {
            std::cout << itemLines[i];
        }
        std::cout << std::endl;
    }
}

bool Inventory::addItem(Item *item)
{
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(items[i][j] == nullptr) {
                items[i][j] = item;
                return true;
            }
        }
    }
    return false;
}

Item* Inventory::getCurrentItem() const
{
    return items[currentRow][currentCol];
}

bool Inventory::addItemAtPosition(Item* item, int row, int col)
{
    if(row < 0 || row >= rows || col < 0 || col >= cols) {
        return false; // Invalid position
    }

    if(items[row][col] != nullptr) {
        return false; // Position already occupied
    }

    items[row][col] = item;
    return true;
}

bool Inventory::addItemAtRandomPosition(Item* item)
{
    // Get list of empty positions
    std::vector<std::pair<int, int>> emptyPositions;

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(items[i][j] == nullptr) {
                emptyPositions.push_back(std::make_pair(i, j));
            }
        }
    }

    if(emptyPositions.empty()) {
        return false; // No empty slots
    }

    // Pick random empty position
    int randomIndex = rand() % emptyPositions.size();
    int row = emptyPositions[randomIndex].first;
    int col = emptyPositions[randomIndex].second;

    items[row][col] = item;
    return true;
}

int Inventory::getEmptySlotCount() const
{
    int count = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(items[i][j] == nullptr) {
                count++;
            }
        }
    }
    return count;
}