#include "inventory.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <cctype>

static const bool ACTIVE_MAP[2][5] = {
    { true, true, true, true, true },
    { true, true, true, true, true }
};

Inventory::Inventory() {
    currentRow = 0; currentCol = 0;
    isDragging = false; dragRow = -1; dragCol = -1;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            items[i][j]       = nullptr;
            activeSlots[i][j] = ACTIVE_MAP[i][j];
        }
}

Inventory::~Inventory() { clear(); }

void Inventory::clear() {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            if (items[i][j]) { delete items[i][j]; items[i][j] = nullptr; }
}

bool Inventory::isActive(int r, int c) const {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return false;
    return activeSlots[r][c];
}

Item* Inventory::getItemAt(int r, int c) const {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return nullptr;
    return items[r][c];
}

void Inventory::setItemAt(int r, int c, Item* item) {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return;
    items[r][c] = item;
}

Item*   Inventory::getCurrentItem() const { return items[currentRow][currentCol]; }
Item*** Inventory::getItems()        const { return nullptr; }
Item*** Inventory::getItemsGrid()    const { return nullptr; }

void Inventory::moveToNextActive(int dRow, int dCol) {
    int r = currentRow, c = currentCol;
    for (int attempt = 0; attempt < ROWS * COLS; attempt++) {
        c += dCol; r += dRow;
        if (c < 0)     c = COLS - 1;
        if (c >= COLS) c = 0;
        if (r < 0)     r = ROWS - 1;
        if (r >= ROWS) r = 0;
        if (activeSlots[r][c]) { currentRow = r; currentCol = c; return; }
    }
}

void Inventory::setCurrentRow(int r) {
    if (r != currentRow) moveToNextActive((r < currentRow) ? -1 : 1, 0);
}
void Inventory::setCurrentCol(int c) {
    if (c != currentCol) moveToNextActive(0, (c < currentCol) ? -1 : 1);
}

std::string Inventory::getItemDisplayStr(int row, int col) const {
    if (!items[row][col]) return "   ";
    const std::string& name = items[row][col]->getName();
    std::string r;
    for (int k = 0; k < 3; k++)
        r += (k < (int)name.size()) ? (char)std::toupper((unsigned char)name[k]) : ' ';
    return r;
}

void Inventory::renderCell(int i, int j) const {
    if (!activeSlots[i][j]) { std::cout << "      "; return; }
    bool isCursor  = (i == currentRow && j == currentCol);
    bool isDragSrc = (isDragging && i == dragRow && j == dragCol);
    bool carrying  = (isCursor && isDragging);
    std::cout << (isCursor ? '<' : '[');
    if (isDragSrc && !carrying) std::cout << "   ";
    else if (carrying)          std::cout << getItemDisplayStr(dragRow, dragCol);
    else if (!items[i][j])      std::cout << "   ";
    else                        std::cout << getItemDisplayStr(i, j);
    std::cout << (isCursor ? '>' : ']') << ' ';
}

void Inventory::display() {
    std::cout << "\n";
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) renderCell(i, j);
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Inventory::displayWithItemInfo(Item* item) {
    std::string lines[6] = {
        "Name: "   + item->getName(),
        "Type: "   + Item::typeToString(item->getType()),
        "Price: "  + std::to_string(item->getPrice()) + "g",
        "Level: "  + std::to_string(item->getLevel()),
        "Rarity: " + Item::rarityToString(item->getRarity()),
        "Effect: " + std::to_string(item->getPercentValue()) + "%"
    };
    std::cout << "\n";
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) renderCell(i, j);
        std::cout << "  ";
        if (i < 2) std::cout << lines[i];
        std::cout << "\n";
    }
    for (int k = 2; k < 6; k++) std::cout << lines[k] << "\n";
    std::cout << "\n";
}

void Inventory::displayWithEmptyInfo() {
    std::cout << "\n";
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) renderCell(i, j);
        std::cout << "\n";
    }
    std::cout << "\n";
}

bool Inventory::addItem(Item* item) {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < 3; j++)
            if (activeSlots[i][j] && !items[i][j]) { items[i][j] = item; return true; }
    return false;
}

bool Inventory::addItemAtPosition(Item* item, int row, int col) {
    if (!isActive(row, col) || items[row][col]) return false;
    items[row][col] = item; return true;
}

bool Inventory::addItemAtRandomPosition(Item* item) {
    std::vector<std::pair<int,int>> empty;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < 3; j++)
            if (activeSlots[i][j] && !items[i][j]) empty.push_back({i, j});
    if (empty.empty()) return false;
    auto rc = empty[std::rand() % empty.size()];
    items[rc.first][rc.second] = item;
    return true;
}

int Inventory::getEmptySlotCount() const {
    int n = 0;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            if (activeSlots[i][j] && !items[i][j]) n++;
    return n;
}

bool Inventory::equipItem() {
    Item* item = items[currentRow][currentCol];
    if (!item) return false;

    bool inEquipZone = (currentCol >= 3);

    if (inEquipZone) {
        // UNEQUIP: move back to any free bag slot
        std::vector<std::pair<int,int>> bagEmpty;
        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < 3; j++)
                if (activeSlots[i][j] && !items[i][j])
                    bagEmpty.push_back({i, j});
        if (bagEmpty.empty()) return false;
        auto rc = bagEmpty[std::rand() % bagEmpty.size()];
        items[rc.first][rc.second] = item;
        items[currentRow][currentCol] = nullptr;
        return true;
    } else {
        // EQUIP: move to any free equip slot
        std::vector<std::pair<int,int>> equipEmpty;
        for (int i = 0; i < ROWS; i++)
            for (int j = 3; j < COLS; j++)
                if (activeSlots[i][j] && !items[i][j])
                    equipEmpty.push_back({i, j});
        if (equipEmpty.empty()) return false;
        auto rc = equipEmpty[std::rand() % equipEmpty.size()];
        items[rc.first][rc.second] = item;
        items[currentRow][currentCol] = nullptr;
        return true;
    }
}

bool Inventory::startDrag() {
    if (isDragging || !items[currentRow][currentCol]) return false;
    isDragging = true; dragRow = currentRow; dragCol = currentCol;
    return true;
}

bool Inventory::dropItem() {
    if (!isDragging) return false;
    std::swap(items[dragRow][dragCol], items[currentRow][currentCol]);
    isDragging = false; dragRow = -1; dragCol = -1;
    return true;
}

void Inventory::cancelDrag() { isDragging = false; dragRow = -1; dragCol = -1; }
bool Inventory::getIsDragging() const { return isDragging; }

std::string Inventory::renderCellStr(int i, int j) const {
    if (!activeSlots[i][j]) return "      ";

    bool isCursor  = (i == currentRow && j == currentCol);
    bool isDragSrc = (isDragging && i == dragRow && j == dragCol);
    bool carrying  = (isCursor && isDragging);
    bool isEquip   = (j >= 3);

    std::string s;

    if (isCursor && !isDragging)     s += "\033[93m";
    else if (carrying)               s += "\033[95m";
    else if (isDragSrc && !carrying) s += "\033[90m";
    else if (items[i][j]) {
        switch (items[i][j]->getRarity()) {
            case magic: s += "\033[96m"; break;
            case rare:  s += "\033[93m"; break;
            default: break;
        }
    }

    s += isCursor ? '<' : (isEquip ? '{' : '[');

    if (isDragSrc && !carrying) s += "   ";
    else if (carrying)          s += getItemDisplayStr(dragRow, dragCol);
    else if (!items[i][j])      s += "   ";
    else                        s += getItemDisplayStr(i, j);

    s += isCursor ? '>' : (isEquip ? '}' : ']');
    s += ' ';
    s += "\033[0m";
    return s;
}

std::vector<std::string> Inventory::renderBagLines() const {
    std::vector<std::string> lines;
    lines.push_back("\033[38;5;240m\u250c\u2500\u2500\u2500\u2500\u2500\u2500 \033[97mBAG\033[38;5;240m \u2500\u2500\u2500\u2500\u2500\u2500\u2510\033[0m");
    for (int i = 0; i < ROWS; i++) {
        std::string row = "\033[38;5;240m\u2502\033[0m ";
        for (int j = 0; j < 3; j++) row += renderCellStr(i, j);
        row += "\033[38;5;240m\u2502\033[0m";
        lines.push_back(row);
    }
    lines.push_back("\033[38;5;240m\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518\033[0m");
    return lines;
}

std::vector<std::string> Inventory::renderEquipLines() const {
    std::vector<std::string> lines;
    lines.push_back("\033[38;5;178m\u250c\u2500\u2500\u2500 \033[93mEQUIP\033[38;5;178m \u2500\u2500\u2500\u2510\033[0m");
    for (int i = 0; i < ROWS; i++) {
        std::string row = "\033[38;5;178m\u2502\033[0m ";
        for (int j = 3; j < COLS; j++) row += renderCellStr(i, j);
        row += "\033[38;5;178m\u2502\033[0m";
        lines.push_back(row);
    }
    lines.push_back("\033[38;5;178m\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518\033[0m");
    return lines;
}