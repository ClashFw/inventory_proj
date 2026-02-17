# Terminal-Based Inventory System

A C++ terminal-based inventory management system with grid navigation, item filtering, and random item generation.

## Description

This project is a cross-platform terminal application that simulates an RPG-style inventory system. Players can navigate through a grid-based inventory using keyboard controls, view detailed item information, and filter items by rarity. The system features a powerful random item generator that creates unique inventory setups each time you play, with items placed at random positions throughout the grid.

## Features

- **Grid-based Inventory**: 6x7 grid layout for organizing items
- **Keyboard Navigation**: Intuitive WASD controls for cursor movement
- **Item Information Display**: Toggle detailed item stats with the 'I' key
- **Rarity Filtering**: Filter inventory by Common, Magic, or Rare items
- **Random Item Generation**: Each game session generates 5-20 random items with randomized properties
- **Random Positioning**: Items appear at random positions in the grid for natural distribution
- **Cross-platform Support**: Works on Windows, macOS, and Linux
- **Dynamic Item Properties**:
  - Name (randomly selected from 10 item types)
  - Price (randomized between 5-100 gold)
  - Durability (always 100)
  - Level (always 1)
  - Rarity (randomly assigned: Common 60%, Magic 30%, Rare 10%)

## Controls

| Key | Action |
|-----|--------|
| W | Move cursor up |
| A | Move cursor left |
| S | Move cursor down |
| D | Move cursor right |
| I | Toggle item information panel |
| 1 | Filter by Common items |
| 2 | Filter by Magic items |
| 3 | Filter by Rare items |
| 0 | Clear filter (show all items) |
| Backspace | Exit application |

## Building the Project

### Requirements
- CMake 3.16 or higher
- C++17 compatible compiler
- Terminal with ANSI escape code support (for best experience)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build .

# Run the application
./inventory
```

### Platform-Specific Notes

**Windows**: Uses `conio.h` for single character input
**macOS/Linux**: Uses `termios.h` for terminal manipulation

## Project Structure

```
inventory/
├── CMakeLists.txt          # Build configuration
├── main.cpp                # Application entry point
├── game.h/cpp              # Game loop and controls
├── player.h/cpp            # Player class
├── inventory.h/cpp         # Inventory management and filtering
├── item.h/cpp              # Item class with properties
├── itemGenerator.h/cpp     # Random item generation system
├── singleCharacter.h       # Cross-platform keyboard input
└── README.md               # This file
```

## Class Overview

- **Game**: Manages the main game loop, user input, and screen updates
- **Player**: Represents the player and owns an inventory
- **Inventory**: Handles item storage, display, filtering logic, and random positioning
- **Item**: Represents individual items with properties and rarity
- **ItemGenerator**: Creates random items with randomized properties and positions

## Item Generation

The system uses a random item generator that creates 5-20 items each game session.

**Available Item Types:**
- Sword
- Shield
- Potion
- Helmet
- Boots
- Ring
- Amulet
- Bow
- Arrow
- Armor

**Randomization Details:**
- **Name**: Randomly selected from the 10 item types above
- **Price**: Random value between 5-100 gold
- **Rarity**: Common (60% chance), Magic (30% chance), or Rare (10% chance)
- **Durability**: Always 100
- **Level**: Always 1
- **Position**: Placed at random empty slots in the grid

Each playthrough will have a unique combination of items at different positions!

## Future Enhancements

- Item stacking functionality
- Drag and drop item movement
- Save/load inventory state
- Item sorting options
- Equipment slots
- Legendary rarity tier
- Item name modifiers (e.g., "Flaming Sword")
- Adjustable item generation parameters
- Item categories (weapons, armor, consumables)

## Authors

School project in a duo with da Mini Pekka
