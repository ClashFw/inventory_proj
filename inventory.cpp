#include "inventory.h"
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <cctype>

// ─── Layout (3 rows x 8 cols) ────────────────────────────────
//
//  col:  0   1   2   3   4   5   6   7
// row 0: BAG BAG BAG  .   .  TOP  .   .
// row 1: BAG BAG BAG  .  LFT MID RGT  .
// row 2: BAG BAG BAG  .   .  BOT  .   .
//
// col 3 = gap
// ─────────────────────────────────────────────────────────────

static const bool ACTIVE_MAP[3][8] = {
    { true,  true,  true,  false, false, true,  false, false },
    { true,  true,  true,  false, true,  true,  true,  false },
    { true,  true,  true,  false, false, true,  false, false },
};

Inventory::Inventory()
{
    currentRow = 0;
    currentCol = 0;
    filterEnabled = false;
    currentFilter = common;
    isDragging = false;
    dragRow = -1;
    dragCol = -1;

    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++) {
            items[i][j]       = nullptr;
            activeSlots[i][j] = ACTIVE_MAP[i][j];
        }
}

Inventory::~Inventory() { clear(); }

void Inventory::clear()
{
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++)
            if(items[i][j]) { delete items[i][j]; items[i][j] = nullptr; }
}

bool Inventory::isActive(int r, int c) const
{
    if(r < 0 || r >= ROWS || c < 0 || c >= COLS) return false;
    return activeSlots[r][c];
}

Item* Inventory::getItemAt(int r, int c) const
{
    if(r < 0 || r >= ROWS || c < 0 || c >= COLS) return nullptr;
    return items[r][c];
}

void Inventory::setItemAt(int r, int c, Item* item)
{
    if(r < 0 || r >= ROWS || c < 0 || c >= COLS) return;
    items[r][c] = item;
}

Item* Inventory::getCurrentItem() const { return items[currentRow][currentCol]; }
Item*** Inventory::getItems()     const { return nullptr; }
Item*** Inventory::getItemsGrid() const { return nullptr; }

// ── cursor navigation ────────────────────────────────────────
void Inventory::moveToNextActive(int dRow, int dCol)
{
    int r = currentRow, c = currentCol;
    for(int attempt = 0; attempt < ROWS * COLS; attempt++) {
        c += dCol; r += dRow;
        if(c < 0) c = COLS-1;
        if(c >= COLS) c = 0;
        if(r < 0) r = ROWS-1;
        if(r >= ROWS) r = 0;
        if(activeSlots[r][c]) { currentRow = r; currentCol = c; return; }
    }
}

void Inventory::setCurrentRow(int r) { if(r != currentRow) moveToNextActive((r < currentRow) ? -1 : 1, 0); }
void Inventory::setCurrentCol(int c) { if(c != currentCol) moveToNextActive(0, (c < currentCol) ? -1 : 1); }

// ── display helpers ──────────────────────────────────────────
std::string Inventory::getItemDisplayStr(int row, int col) const
{
    if(!items[row][col]) return "   ";
    const std::string& name = items[row][col]->getName();
    std::string r;
    for(int k = 0; k < 3; k++)
        r += (k < (int)name.size()) ? (char)toupper((unsigned char)name[k]) : ' ';
    return r;
}

void Inventory::renderCell(int i, int j, bool shouldDisplay) const
{
    if(!activeSlots[i][j]) { std::cout << "      "; return; }

    bool isCursor  = (i == currentRow && j == currentCol);
    bool isDragSrc = (isDragging && i == dragRow   && j == dragCol);
    bool carrying  = (isCursor && isDragging);

    std::cout << (isCursor ? "<" : "[");

    if     (isDragSrc && !carrying)  std::cout << "   ";
    else if(carrying)                std::cout << getItemDisplayStr(dragRow, dragCol);
    else if(!items[i][j])            std::cout << "   ";
    else if(shouldDisplay)           std::cout << getItemDisplayStr(i, j);
    else                             std::cout << "···";

    std::cout << (isCursor ? "> " : "] ");
}

// ── display ──────────────────────────────────────────────────
void Inventory::display()
{
    std::cout << std::endl;
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            if(j == 3) { std::cout << "   "; continue; } // gap
            bool sd = true;
            if(activeSlots[i][j] && items[i][j] && filterEnabled)
                sd = (items[i][j]->getRarity() == currentFilter);
            renderCell(i, j, sd);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Inventory::displayWithItemInfo(Item* item)
{
    auto rs = [](Rarity r) -> std::string {
        switch(r){ case common: return "Common"; case magic: return "Magic"; case rare: return "Rare"; }
        return "";
    };

    std::string lines[5] = {
        "Name:   " + item->getName(),
        "Price:  " + std::to_string(item->getPrice()) + "g",
        "Dur:    " + std::to_string(item->getDurability()),
        "Level:  " + std::to_string(item->getLevel()),
        "Rarity: " + rs(item->getRarity()),
    };

    std::cout << std::endl;
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            if(j == 3) { std::cout << "   "; continue; }
            bool sd = true;
            if(activeSlots[i][j] && items[i][j] && filterEnabled)
                sd = (items[i][j]->getRarity() == currentFilter);
            renderCell(i, j, sd);
        }
        std::cout << "   ";
        if(i < 5) std::cout << lines[i];
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Inventory::displayWithEmptyInfo()
{
    std::cout << std::endl;
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLS; j++) {
            if(j == 3) { std::cout << "   "; continue; }
            bool sd = true;
            if(activeSlots[i][j] && items[i][j] && filterEnabled)
                sd = (items[i][j]->getRarity() == currentFilter);
            renderCell(i, j, sd);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// ── item adding ──────────────────────────────────────────────
bool Inventory::addItem(Item* item)
{
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < 3; j++) // bag cols only (0-2)
            if(activeSlots[i][j] && !items[i][j]) { items[i][j] = item; return true; }
    return false;
}

bool Inventory::addItemAtPosition(Item* item, int row, int col)
{
    if(!isActive(row,col) || items[row][col]) return false;
    items[row][col] = item; return true;
}

bool Inventory::addItemAtRandomPosition(Item* item)
{
    std::vector<std::pair<int,int>> empty;
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < 3; j++) // bag cols only (0-2)
            if(activeSlots[i][j] && !items[i][j])
                empty.push_back({i,j});
    if(empty.empty()) return false;
    auto [r,c] = empty[rand() % empty.size()];
    items[r][c] = item; return true;
}

int Inventory::getEmptySlotCount() const
{
    int n = 0;
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++)
            if(activeSlots[i][j] && !items[i][j]) n++;
    return n;
}

bool Inventory::equipItem()
{
    Item* item = items[currentRow][currentCol];
    if(!item) return false;

    // Find a random empty equipment slot (cols 4-7)
    std::vector<std::pair<int,int>> empty;
    for(int i = 0; i < ROWS; i++)
        for(int j = 4; j < COLS; j++)
            if(activeSlots[i][j] && !items[i][j])
                empty.push_back({i,j});
    if(empty.empty()) return false;

    auto [r,c] = empty[rand() % empty.size()];
    items[r][c] = item;
    items[currentRow][currentCol] = nullptr;
    return true;
}

// ── drag & drop ──────────────────────────────────────────────
bool Inventory::startDrag()
{
    if(isDragging || !items[currentRow][currentCol]) return false;
    isDragging = true; dragRow = currentRow; dragCol = currentCol; return true;
}

bool Inventory::dropItem()
{
    if(!isDragging) return false;
    std::swap(items[dragRow][dragCol], items[currentRow][currentCol]);
    isDragging = false; dragRow = dragCol = -1; return true;
}

void Inventory::cancelDrag() { isDragging = false; dragRow = dragCol = -1; }
bool Inventory::getIsDragging() const { return isDragging; }

// ── filtering ─────────────────────────────────────────────────
void Inventory::setFilter(Rarity r) { filterEnabled = true; currentFilter = r; }
void Inventory::clearFilter()       { filterEnabled = false; }
bool Inventory::isFilterEnabled()   const { return filterEnabled; }
Rarity Inventory::getCurrentFilter() const { return currentFilter; }

int Inventory::getFilteredItemCount() const
{
    int n = 0;
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++)
            if(activeSlots[i][j] && items[i][j])
                if(!filterEnabled || items[i][j]->getRarity() == currentFilter) n++;
    return n;
}