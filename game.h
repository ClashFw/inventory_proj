#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "ItemGenerator.h"
#include "shop.h"
#include "enemy.h"
#include <set>
#include <string>

class Game
{
private:
    Player*        player;
    ItemGenerator* itemGenerator;
    Shop*          shop;

    std::set<std::string> defeatedEnemies;

    void playBattleArena();
    bool battleOneEnemy(Enemy& enemy);

public:
    Game();
    ~Game();

    void showMainMenu();
    void play();
    void playShop();
    void playSearchMenu();
    void sellItemsMenu();
    void placeItemMenu(Item* item);
    void showSellConfirmScreen(Item* item, int row, int col);
    void initializeSampleItems();
    void generateRandomInventory();

    Player* getPlayer() const;
    void    setPlayer(Player* newPlayer);
    Shop*   getShop()   const;
};

#endif // GAME_H
