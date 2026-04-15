#ifndef PLAYER_H
#define PLAYER_H

#include "inventory.h"
#include "servant.h"

class Player {
private:
    Inventory* inventory;
    Servant* servant;   // owned by Player

public:
    Player();
    ~Player();

    Inventory* getInventory() const { return inventory; }

    Servant* getServant() const { return servant; }
    void setServant(const Servant& s); // copy template into owned Servant

    // Effective stats including equipment bonuses
    int getEffectiveStrength() const;
    int getEffectiveDurability() const;
    int getEffectiveAgility() const;
    int getEffectiveMaxHP() const;
};

#endif // PLAYER_H
