#ifndef INVENTORY_H
#define INVENTORY_H
#include "item.h"
#include <string>

// Layout uses a fixed 4x6 grid.
// Some cells are INACTIVE (no slot) to form the shaped layout.
// Active slot map (row, col):
//
//  Equipment side (cols 0-2):       Bag side (cols 4-5):
//  Row 0:  . H .  . .               (H = Head)
//  Row 1:  L B R  . .               (L=Left, B=Body, R=Right)
//  Row 2:  . G .  Q Q               (G=Groin/Legs, Q=quick bag)
//  Row 3:  . f s  . .               (f=foot-left, s=foot-right / boots)
//
// col indices: 0  1  2  3  4  5
//              ^equip^   gap ^bag^

class Inventory
{
private:
    // Grid storage: 4 rows x 6 cols
    static const int ROWS = 3;
    static const int COLS = 8;
    Item* items[ROWS][COLS];

    // Which cells are valid slots
    bool activeSlots[ROWS][COLS];

    // Slot labels for equipment display

    int currentRow;
    int currentCol;

    Rarity currentFilter;
    bool filterEnabled;
    bool isDragging;
    int dragRow;
    int dragCol;

    void renderCell(int i, int j, bool shouldDisplay) const;
    void moveToNextActive(int dRow, int dCol); // navigate only active slots

public:
    Inventory();
    ~Inventory();

    void display();
    void displayWithItemInfo(Item* item);
    void displayWithEmptyInfo();

    bool addItem(Item* item);
    bool addItemAtPosition(Item* item, int row, int col);
    bool addItemAtRandomPosition(Item* item);
    Item* getCurrentItem() const;
    int getEmptySlotCount() const;
    std::string getItemDisplayStr(int row, int col) const;

    bool isActive(int row, int col) const;

    // Drag and drop
    bool equipItem(); // Move item at cursor to a random equipment slot (cols 4-7)
    bool startDrag();
    bool dropItem();
    void cancelDrag();
    bool getIsDragging() const;

    // Filtering
    void setFilter(Rarity rarity);
    void clearFilter();
    bool isFilterEnabled() const;
    Rarity getCurrentFilter() const;
    int getFilteredItemCount() const;

    // Getters/setters (keep compatible interface)
    void clear();
    int getRows() const { return ROWS; }
    int getCols() const { return COLS; }
    Item*** getItems() const; // returns flat accessor — see .cpp
    int getCurrentRow() const { return currentRow; }
    int getCurrentCol() const { return currentCol; }
    void setCurrentRow(int r);
    void setCurrentCol(int c);

    // raw 2d access for game.cpp placement / sell logic
    Item* getItemAt(int r, int c) const;
    void  setItemAt(int r, int c, Item* item);

    // legacy compat
    Item ***getItemsGrid() const;
};

#endif // INVENTORY_H