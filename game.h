#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "ItemGenerator.h"
#include "shop.h"
#include "enemy.h"
#include "servant.h"
#include <set>
#include <string>

class Game
{
private:
    Player*        player;
    ItemGenerator* itemGenerator;
    Shop*          shop;

    // Set in the naming / renaming screens
    std::string masterName;

    // Names of enemies defeated in the current war
    std::set<std::string> defeatedEnemies;

    // Core battle arena flow and single-enemy duel
    void playBattleArena();
    bool battleOneEnemy(Enemy& enemy);

public:
    Game();
    ~Game();

    // First screen shown before the main menu
    void showNamingScreen();

    // Shown when switching to the second Holy Grail War
    void showRenamingScreen();

    // Shows current Servant’s art, stats, and NPs
    void showServantProfile(const Servant& sv);

    // Two-column roster (Fate/stay night | Fate/Zero)
    void showEnemyGallery();

    // Main menu and sub-modes
    void showMainMenu();
    void play();               // Inventory / Servant status screen
    void playShop();           // Shop loop
    void playSearchMenu();     // Shop search input
    void sellItemsMenu();      // Sell items from inventory
    void placeItemMenu(Item* item);
    void showSellConfirmScreen(Item* item, int row, int col);

    // Item / inventory setup
    void initializeSampleItems();
    void generateRandomInventory();

    // Accessors
    Player* getPlayer() const;
    void    setPlayer(Player* newPlayer);
    Shop*   getShop()   const;
};

#endif // GAME_Hx