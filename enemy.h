#ifndef ENEMY_H
#define ENEMY_H

#include "servant.h"

class Enemy {
private:
    Servant servant;

public:
    Enemy(const Servant& base) : servant(base) {}

    Servant&       getServant()       { return servant; }
    const Servant& getServant() const { return servant; }
};

#endif // ENEMY_H
