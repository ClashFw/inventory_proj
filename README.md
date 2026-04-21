
# Terminal-Based Servant Battle Royale

A C++ terminal application combining an RPG-style inventory system with a Fate-series Servant battle arena, featuring grid inventory management, a shop, and turn-based combat with Noble Phantasms.

## Description

This project is a cross-platform terminal application inspired by the Fate franchise. Players are assigned a random Servant (Saber, Archer, Lancer, etc.) from either Fate/stay night or Fate/Zero, manage a grid-based item inventory, buy and sell equipment in a shop, and engage in turn-based duels against enemy Servants. Items boost the player's effective stats in combat, bridging the inventory and battle systems.

## Features

### Inventory
- **Grid-based Inventory**: 2x3 bag layout for equipping items
- **Keyboard Navigation**: WASD cursor movement with drag-and-drop (`G` to grab/drop, `Q` to cancel)
- **Item Information Panel**: Toggle detailed stats with `I`
- **Rarity Filtering**: Filter by Common, Magic, or Rare items
- **Sell from Inventory**: Press `H` on any item to sell it at 80-90% of its value (randomised per sale)
- **Equip Items**: Press `T` to equip an item to an equipment slot, boosting combat stats
- **Smart Name Display**: Grid cells show the shortest unambiguous prefix to distinguish items

### Shop
- Browse **10 randomly generated items** per stock
- **Buy** items with manual placement -- choose your grid slot or auto-place with ESC
- **Sell** items directly from the shop sell menu
- **Search** by item name or type with live filtering
- Filter by **rarity** and **item type** simultaneously
- **Restock** the shop with a fresh batch at any time
- Starting gold is randomised: 200-400g (70%), 400-500g (20%), or 500-800g (10%)

### Battle Arena
- Fight randomly selected **enemy Servants** from the same Fate series as your Servant
- Turn-based combat: choose **Basic Attack** or one of your Servant's **Noble Phantasms** each round
- Hit chance is agility-based: `75% + (playerAgility - enemyAgility) x 2`, capped between 10-95%
- Noble Phantasms are **single-use per battle** and deal scaled damage based on Servant strength
- **Escape** at any time with `E`
- Battle again immediately by pressing `B` after a fight concludes

## Servants

### Fate/stay night

| Servant | HP | STR | DUR | AGI | Noble Phantasms |
|---|---|---|---|---|---|
| Saber (Artoria) | 160 | 26 | 20 | 18 | Excalibur, Invisible Air |
| Archer (Emiya) | 145 | 24 | 17 | 23 | Unlimited Blade Works, Kanshou & Bakuya |
| Lancer (Cu Chulainn) | 150 | 23 | 16 | 25 | Gae Bolg, Soaring Spear |
| Rider (Medusa) | 140 | 22 | 15 | 24 | Bellerophon, Blood Fort Andromeda |
| Berserker (Heracles) | 210 | 30 | 22 | 13 | Nine Lives |
| Archer (Gilgamesh) | 155 | 27 | 18 | 20 | Gate of Babylon, Enuma Elish |
| Assassin (Sasaki) | 135 | 22 | 14 | 26 | Tsubame Gaeshi |
| Caster (Medea) | 135 | 20 | 15 | 18 | Rule Breaker, Age of Gods Magecraft |
| Assassin (Hassan) | 135 | 21 | 15 | 24 | Zabaniya |

### Fate/Zero

| Servant | HP | STR | DUR | AGI | Noble Phantasms |
|---|---|---|---|---|---|
| Saber (Artoria Zero) | 165 | 27 | 21 | 19 | Excalibur, Invisible Air |
| Archer (Gilgamesh Zero) | - | - | - | - | Gate of Babylon, Enuma Elish |

## Controls

### Main Menu

| Key | Action |
|---|---|
| W / S | Navigate options |
| Enter | Select (Inventory / Shop / Battle Arena / Quit) |
| ESC | Quit |

### Inventory

| Key | Action |
|---|---|
| W / A / S / D | Move cursor |
| G | Grab item; press again to drop/swap |
| Q | Cancel drag |
| K | Cancel drag (alternate) |
| I | Toggle item info panel |
| H | Sell item at cursor (80-90% value) |
| T | Equip item at cursor |
| 1 / 2 / 3 | Filter Common / Magic / Rare |
| 0 | Clear filter |
| P | Open Shop |
| Backspace | Exit inventory |

### Shop

| Key | Action |
|---|---|
| W / S | Navigate items |
| B | Buy selected item (manual placement) |
| V | Open sell menu |
| F | Search by name |
| C | Clear all filters and search |
| R | Restock shop |
| E | Exit shop |

### Battle Arena

| Key | Action |
|---|---|
| A | Basic attack |
| 1-9 | Use Noble Phantasm (single-use per battle) |
| E | Escape battle |

## Item Types & Pricing

| Type | Common | Magic | Rare | Effect % (C/M/R) |
|---|---|---|---|---|
| Potion | 15g | 30g | 55g | 10 / 20 / 35 |
| Sword | 25g | 50g | 90g | 7 / 12 / 17 |
| Armor | 25g | 50g | 90g | 6 / 14 / 24 |
| Movement | 20g | 40g | 75g | 5 / 10 / 18 |

Rarity distribution: **Common 40% - Magic 30% - Rare 30%**

## Building the Project

### Requirements

- CMake 3.16 or higher
- C++17 compatible compiler
- Terminal with ANSI escape code support

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
./inventory
```

### Platform Notes

**Windows**: Uses `conio.h` for single-character input.  
**macOS / Linux**: Uses `termios.h` for terminal manipulation.

## Project Structure

```
inventory/
+-- CMakeLists.txt         # Build configuration
+-- main.cpp               # Entry point, seeds RNG
+-- game.h / game.cpp      # Main loop, menus, battle arena, shop flow
+-- player.h / player.cpp  # Player class with Servant + Inventory
+-- servant.h / servant.cpp# Servant stats, NPs, ASCII art, enemy pool
+-- enemy.h                # Enemy wrapper around a Servant
+-- inventory.h / .cpp     # 2x3 grid, drag & drop, filtering, equip
+-- item.h / item.cpp      # Item properties: name, type, rarity, price
+-- ItemGenerator.h / .cpp # Random item factory
+-- shop.h / shop.cpp      # Shop logic: buy/sell, search, rarity/type filter
+-- singleCharacter.h      # Cross-platform raw keyboard input
+-- inventory_uml.drawio   # UML class diagram
+-- README.md
```

## Class Overview

| Class | Responsibility |
|---|---|
| `Game` | Main loop, screen refresh, input dispatch, battle arena, sell confirmation |
| `Player` | Owns `Inventory` and assigned `Servant`; provides effective combat stats |
| `Servant` | Stats (HP/STR/DUR/AGI), Noble Phantasms, ASCII art, series, factory methods |
| `Enemy` | Thin wrapper holding an enemy `Servant` instance |
| `Inventory` | 2x3 grid, drag-and-drop, equip slots, rarity filtering, smart name prefixes |
| `Item` | Properties: name, type, rarity, price, durability, level, percent effect |
| `ItemGenerator` | Factory: creates random `Item` instances with type/rarity-based pricing |
| `Shop` | 10-item stock; buy/sell, gold management, name+rarity+type filtering |
```