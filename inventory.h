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
public:
    Inventory();
    ~Inventory();
    void display();
    bool addItem(Item* item);

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
