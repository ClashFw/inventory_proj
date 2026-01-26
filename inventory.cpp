#include "inventory.h"
#include <iostream>
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

    items = new Item**[rows];
    for(int i = 0; i < rows; i++) {
        items[i] = new Item*[cols];
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

void Inventory::display()
{
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(i == currentRow && j == currentCol) {
                std::cout << "{";
            }
            else {
                std::cout << "[";
            }
            if(items[i][j] == nullptr) {
                std::cout << " ";
            }
            else {
                std::cout << char(toupper(items[i][j]->getName()[0]));
            }
            if(i == currentRow && j == currentCol) {
                std::cout << "}";
            }
            else {
                std::cout << "]";
            }
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












