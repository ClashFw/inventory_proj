# Terminal-Based Inventory System

A C++ terminal-based inventory management system with grid navigation, item filtering, a shop, and drag-and-drop item movement.

## Description

This project is a cross-platform terminal application that simulates an RPG-style inventory system. Players can navigate through a grid-based inventory using keyboard controls, view detailed item information, filter items by rarity, and visit a shop to buy and sell items. The system features a random item generator that creates unique inventory setups each session, smart adaptive name display that uses the shortest unambiguous prefix for each item, and drag-and-drop item movement directly within the grid.

## Features

- **Grid-based Inventory**: 6×7 grid layout for organizing items
- **Keyboard Navigation**: Intuitive WASD controls for cursor movement
- **Drag & Drop Movement**: Pick up any item with `G` and drop it at any slot — items swap if the destination is occupied
- **Smart Name Display**: Each item shows the shortest prefix that uniquely identifies it among current inventory contents (e.g. `SW` for Sword when Shield is also present, `BOO`/`BOW` for Boots/Bow)
- **Item Information Panel**: Toggle detailed item stats with `I`
- **Rarity Filtering**: Filter inventory by Common, Magic, or Rare items
- **Random Item Generation**: 5–20 random items placed at random positions each session
- **Shop System**:
  - Browse 20 randomly generated items
  - Buy items (deducted from gold) and sell items (50% return)
  - Search shop items by name with live filtering
  - Navigation in search mode stays within results only
  - Restock shop with a fresh inventory at any time
- **Gold System**: Start with 500g, earn gold by selling, spend it buying
- **Cross-platform**: Windows, macOS, and Linux supported

## Controls

### Inventory

| Key | Action |
|-----|--------|
| W / A / S / D | Move cursor |
| G | Grab item — press again on any slot to drop/swap |
| Q | Cancel current drag |
| I | Toggle item information panel |
| 1 | Filter: Common items only |
| 2 | Filter: Magic items only |
| 3 | Filter: Rare items only |
| 0 | Clear filter (show all) |
| P | Open Shop |
| Backspace | Exit |

### Shop

| Key | Action |
|-----|--------|
| W / S | Navigate items (search-aware when filtering) |
| B | Buy selected item |
| V | Open sell menu |
| F | Search by item name |
| C | Clear search |
| R | Restock shop with new items |
| E | Exit shop |

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

**Windows**: Uses `conio.h` for single character input.  
**macOS / Linux**: Uses `termios.h` for terminal manipulation.

## Project Structure

```
inventory/
├── CMakeLists.txt          # Build configuration
├── main.cpp                # Entry point, seeds RNG
├── game.h / game.cpp       # Main game loop, input handling
├── player.h / player.cpp   # Player class
├── inventory.h / inventory.cpp  # Grid, display, filtering, drag & drop
├── item.h / item.cpp       # Item properties and rarity
├── ItemGenerator.h / .cpp  # Random item factory
├── shop.h / shop.cpp       # Shop, buy/sell, search, navigation
├── singleCharacter.h       # Cross-platform raw keyboard input
├── inventory_uml.drawio    # UML class diagram (draw.io)
└── README.md
```

## Class Overview

| Class | Responsibility |
|-------|---------------|
| `Game` | Main loop, screen refresh, user input dispatch |
| `Player` | Owns the player's `Inventory` |
| `Inventory` | 6×7 grid storage, display with drag state, rarity filtering, smart name prefixes |
| `Item` | Properties: name, rarity, price, durability, level |
| `ItemGenerator` | Factory: creates random `Item` instances with randomised properties |
| `Shop` | Items for sale; buy/sell, gold management, name search, search-aware navigation |

## Item Generation

Items are generated at startup (5–20) and placed at random grid positions.

**Available types**: Sword · Shield · Potion · Helmet · Boots · Ring · Amulet · Bow · Arrow · Armor

**Randomisation**: Name (uniform), Price (5–100g), Rarity (Common 60% · Magic 30% · Rare 10%), Durability always 100, Level always 1.

## Smart Name Display

Instead of a single initial, the grid shows the minimum prefix needed to tell items apart:

| Items present | Displayed as |
|---------------|-------------|
| Sword only | `S  ` |
| Sword + Shield | `SW ` / `SH ` |
| Bow + Boots | `BOW` / `BOO` |
| Arrow + Armor + Amulet | `ARR` / `ARM` / `AMU` |

## Drag & Drop

Press `G` on any occupied slot to pick it up — the source slot shows `< >` and the item follows your cursor as `{XXX}`. Navigate to any destination and press `G` again to place it. If the destination already holds an item the two are swapped. Press `Q` to cancel and return the item to its original slot.

