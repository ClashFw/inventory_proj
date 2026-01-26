#ifndef GAME_H
#define GAME_H
#include "player.h"

class Game
{
private:
    Player* player;
public:
    Game();
    ~Game();
    void play();
    Player *getPlayer() const;
    void setPlayer(Player *newPlayer);
};

#endif // GAME_H
