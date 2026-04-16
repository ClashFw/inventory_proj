#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"
#include <string>

// Layout uses a fixed 2x5 grid.
// Bag side:   cols 0-2 (2x3 = 6 slots)
// Equip side: cols 3-4 (2x2 = 4 slots)
//
// row 0: [BAG][BAG][BAG][EQUIP][EQUIP]
// row 1: [BAG][BAG][BAG][EQUIP][EQUIP]

class Inventory
{
private:
    static const int ROWS = 2;
    static const int COLS = 5;
    Item* items[ROWS][COLS];
    bool activeSlots[ROWS][COLS];

    int currentRow;
    int currentCol;

    bool isDragging;
    int dragRow;
    int dragCol;

    void renderCell(int i, int j) const;
    void moveToNextActive(int dRow, int dCol);

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

    bool equipItem();
    bool startDrag();
    bool dropItem();
    void cancelDrag();
    bool getIsDragging() const;

    void clear();
    int getRows() const { return ROWS; }
    int getCols() const { return COLS; }
    Item*** getItems() const;
    int getCurrentRow() const { return currentRow; }
    int getCurrentCol() const { return currentCol; }
    void setCurrentRow(int r);
    void setCurrentCol(int c);

    Item* getItemAt(int r, int c) const;
    void setItemAt(int r, int c, Item* item);

    Item*** getItemsGrid() const;
};

#endif // INVENTORY_H
