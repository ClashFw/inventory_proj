#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "inventory.h"

class Player
{
private:
    std::string name;
    Inventory* inventory;
public:
    Player();
    ~Player();
    std::string getName() const;
    void setName(const std::string &newName);
    Inventory *getInventory() const;
    void setInventory(Inventory *newInventory);
};

#endif // PLAYER_H
