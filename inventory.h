#ifndef INVENTORY_H
#define INVENTORY_H
#include "item.h"
class Inventory
{
private:
    int rows;
    int cols;
    Item*** items;
    int currentRow;
    int currentCol;
    Rarity currentFilter; // Filter for displaying items by rarity
    bool filterEnabled;   // Whether filtering is active
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

    // Filtering methods
    void setFilter(Rarity rarity);
    void clearFilter();
    bool isFilterEnabled() const;
    Rarity getCurrentFilter() const;
    int getFilteredItemCount() const;

    //getters & setters
    void clear();
    int getRows() const;
    void setRows(int newRows);
    int getCols() const;
    void setCols(int newCols);
    Item ***getItems() const;
    void setItems(Item ***newItems);
    int getCurrentRow() const;
    void setCurrentRow(int newCurrentRow);
    int getCurrentCol() const;
    void setCurrentCol(int newCurrentCol);
};

#endif // INVENTORY_H