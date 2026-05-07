# Terminal-Based Servant Battle Royale

> A cross-platform C++ terminal application combining RPG grid inventory management with turn-based Fate-series Servant combat.

**Version:** 1.0 | **Author:** Franciszek Wasiak | **Build System:** CMake 3.16+ | **Standard:** C++17

***

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Building & Running](#building--running)
- [Controls](#controls)
- [Game Systems](#game-systems)
  - [Naming Screen](#naming-screen)
  - [Inventory](#inventory)
  - [Shop of Chaldea](#shop-of-chaldea)
  - [Battle Arena](#battle-arena)
- [Servants](#servants)
- [Items](#items)
- [Class Overview](#class-overview)
- [Known Issues & To-Do](#known-issues--to-do)

***

## Overview

*Terminal-Based Servant Battle Royale* is a school project written in C++17 that demonstrates object-oriented programming concepts — class design, encapsulation, inheritance, and operator/function overloading — through an interactive terminal game.

You play as a **Master** who commands a randomly assigned **Servant** from the Fate franchise. Manage your equipment in a 2×3 grid inventory, buy and sell items at the *Shop of Chaldea*, and duel enemy Servants in a side-by-side FGO-style battle arena — all rendered entirely in your terminal using ANSI escape codes.

***

## Features

- **Naming Screen** — Enter your Master name before the main menu; it persists throughout the session.
- **Grid Inventory** — Navigate a 2×3 bag with WASD; grab, drop, and swap items with a drag-and-drop system.
- **Equipment Slots** — Equip items to boost your Servant's effective combat stats.
- **Shop of Chaldea** — Browse up to 10 randomly generated items; live-search by name/type, filter by rarity and type simultaneously, restock at any time.
- **Buy & Sell** — Purchase items with gold; sell from inventory (80–90% of base price) or through the shop sell menu (50% of base price).
- **Battle Arena** — FGO-style side-by-side layout with Servant ASCII art, mirrored enemy art, and HP bars. Fight Servants from the same Fate series as yours.
- **Noble Phantasms** — Single-use special attacks per battle, scaled from your Servant's STR stat.
- **Rarity System** — Five item rarities: Common, Magic, Rare (all generators), Legendary, Divine (shop-exclusive).
- **Color-coded UI** — Items and rarities are color-coded throughout using ANSI escape codes.

***

## Requirements

| Requirement | Details |
|---|---|
| **OS** | Windows 10/11, macOS 10.15+, Ubuntu 20.04+ / Debian 10+ |
| **Compiler** | GCC 9+, Clang 10+, or MSVC 2019+ (C++17 required) |
| **Build System** | CMake 3.16+ |
| **Terminal** | Must support ANSI escape codes (Windows Terminal, PowerShell, Terminal.app, iTerm2) |
| **Terminal Size** | Minimum **120 columns × 40 rows** (required for the battle arena layout) |

***

## Building & Running

```bash
# Clone the repository
git clone https://github.com/ClashFw/inventory_proj.git
cd inventory_proj

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run (Linux/macOS)
./build/inventory_proj

# Run (Windows)
.\build\inventory_proj.exe
```

> **Note:** CMake 3.16+ and a C++17-compatible compiler must be available on your PATH.

***

## Controls

### Global

| Key | Action |
|---|---|
| `Backspace` | Exit current screen / return to main menu |

### Inventory

| Key | Action |
|---|---|
| `W` `A` `S` `D` | Navigate the inventory grid |
| `G` | Grab item at cursor / drop grabbed item (swap if slot occupied) |
| `Q` or `K` | Cancel active drag — return item to original slot |
| `T` | Equip item at cursor to equipment slot |
| `H` | Sell item at cursor (80–90% of base price) |
| `I` | Toggle item info panel |
| `P` | Open Shop directly from inventory |
| `1` / `2` / `3` | Filter by rarity: Common / Magic / Rare |
| `0` | Clear rarity filter |

### Shop of Chaldea

| Key | Action |
|---|---|
| `W` / `S` | Navigate item list |
| `B` | Buy selected item |
| `V` | Open sell menu (sell inventory items for 50% of base price) |
| `F` | Live search by name or type (case-insensitive) |
| `1` / `2` / `3` / `4` / `5` | Filter by rarity |
| Type filters | Filter by item type |
| `C` | Clear all active filters and search query |
| `R` | Restock shop (generates new 10-item stock) |
| `E` | Exit shop |

### Battle Arena

| Key | Action |
|---|---|
| `A` | Basic Attack |
| `1` – `9` | Use Noble Phantasm by index |
| `E` | Escape battle |
| `B` | (After battle) Start a new battle immediately |
| Any other key | (After battle) Return to main menu |

***

## Game Systems

### Naming Screen

On launch, you are prompted to enter your **Master name**. This name is stored in `Game::masterName` and displayed throughout the session (shop header, battle arena). Press Enter to confirm. An empty name is rejected with the message `"Name cannot be empty."`.

### Inventory

Your inventory is a **2×3 grid** where each cell holds exactly one item. Grid cells display the **shortest unambiguous name prefix** to distinguish items at a glance.

- **Drag and drop**: Press `G` to grab an item, move your cursor, then press `G` again to place it. Dropping onto an occupied slot **swaps** the items — nothing is lost.
- **Equipping**: Press `T` to equip an item. Equipped items apply a percentage boost to your Servant's effective combat stats.
- **Selling from inventory**: Press `H` on an item. A confirmation prompt shows the sale price (80–90% of base, randomised per transaction). Confirm to credit gold and remove the item.
- **Filters**: Press `1`/`2`/`3` to highlight items by rarity (Common/Magic/Rare). Press `0` to clear.

### Shop of Chaldea

The shop stocks up to **10 randomly generated items**. Unlike the inventory generator, shop items may include **Legendary** and **Divine** rarities.

- **Gold**: Starting gold is randomised on first shop entry — 200–400g (70% chance), 400–500g (20%), or 500–800g (10%).
- **Buying**: Navigate with W/S, press `B` to purchase. Choose a grid slot manually or press ESC to auto-place. Insufficient gold shows `"Not enough gold!"`. A full inventory shows `"Inventory full!"`.
- **Selling via shop menu**: Press `V` to open the sell menu. Items sold here yield exactly **50% of base price**; the item is removed from your inventory and added to the shop stock.
- **Restocking**: Press `R` to replace the entire stock with a new set of 10 items.
- **Filtering**: Rarity and item type filters **compound** — only items matching both active constraints are shown. Press `C` to clear everything.
- **Live search**: Press `F` to type a search query. The list filters in real time (case-insensitive match on name and type).

### Battle Arena

Each battle renders both Servants **side by side**: your Servant's ASCII art on the left, the enemy's **mirrored ASCII art** on the right, with HP bars below each. The arena requires a terminal at least **120 columns wide**.

- **Enemy selection**: The enemy Servant is randomly chosen from the **same Fate series** (Fate/stay night or Fate/Zero) as your Servant.
- **Hit chance formula**: `75% + (playerAGI − enemyAGI) × 2`, capped to the range `[10%, 95%]`.
- **Noble Phantasms**: Each NP is **single-use per battle**. NP usage flags reset automatically between battles. Attempting to reuse a spent NP displays `"Already used!"`.
- **Battle end**: After victory or defeat, press `B` to fight again immediately or any other key to return to the main menu.

***

## Servants

Servant stats: **HP**, **STR** (attack power / NP damage), **DUR** (damage reduction), **AGI** (hit chance modifier).

### Fate/stay night

| Servant | Class | HP | STR | DUR | AGI | Noble Phantasm(s) |
|---|---|---|---|---|---|---|
| Artoria Pendragon | Saber | — | — | — | — | Excalibur |
| EMIYA | Archer | — | — | — | — | Unlimited Blade Works |
| Cu Chulainn | Lancer | — | — | — | — | Gáe Bolg |
| Medusa | Rider | — | — | — | — | Bellerophon |
| Medea | Caster | — | — | — | — | Rule Breaker |
| Hassan-i-Sabbah | Assassin | — | — | — | — | Zabaniya |
| Heracles | Berserker | — | — | — | — | Nine Lives |

> Full stat values are defined in `servant.cpp`. See the source for exact numbers.

### Fate/Zero

| Servant | Class | HP | STR | DUR | AGI | Noble Phantasm(s) | Notes |
|---|---|---|---|---|---|---|---|
| Artoria Pendragon | Saber | — | — | — | — | Excalibur | Full stats |
| Gilgamesh | Archer | — | — | — | — | Ea | Placeholder stats — see [Known Issues](#known-issues--to-do) |
| Diarmuid Ua Duibhne | Lancer | — | — | — | — | — | Not yet implemented |
| Iskander | Rider | — | — | — | — | — | Not yet implemented |
| Gilles de Rais | Caster | — | — | — | — | — | Not yet implemented |
| Lancelot | Berserker | — | — | — | — | — | Not yet implemented |
| Hassan Hundred Faces | Assassin | — | — | — | — | — | Not yet implemented |

***

## Items

Items have four **types** and five **rarities**.

### Item Types

| Type | Color | Effect |
|---|---|---|
| Potion | Green | — |
| Sword | Red | — |
| Armor | Blue | — |
| Movement | Orange | — |

### Item Rarities

| Rarity | Color | Generator | Shop |
|---|---|---|---|
| Common | White | ✓ (40%) | ✓ |
| Magic | Blue | ✓ (30%) | ✓ |
| Rare | Gold | ✓ (30%) | ✓ |
| Legendary | Orange | ✗ | ✓ |
| Divine | Magenta | ✗ | ✓ |

> **Legendary** and **Divine** items are shop-exclusive (`shopOnly = true`). The standard `ItemGenerator` will never produce them.

### Pricing & Selling

| Channel | Sell Price |
|---|---|
| Inventory (`H` key) | 80–90% of base price (randomised per sale) |
| Shop sell menu (`V` key) | 50% of base price |

***

## Class Overview

| Class | Responsibility |
|---|---|
| `Game` | Main loop, screen dispatch, Master name storage |
| `Player` | Owns the `Inventory` and the assigned `Servant`; tracks gold |
| `Servant` | Stats (HP, STR, DUR, AGI), NPs, ASCII art, mirrored art, description, Fate series |
| `Inventory` | 2×3 grid of item pointers; grab/drop/equip logic |
| `Shop` | 10-item stock, buy/sell, search, and filter logic |
| `Item` | Name, type, rarity, price, percent effect, durability, level, `shopOnly` flag |
| `ItemGenerator` | Generates inventory-safe items (Common/Magic/Rare only) |
| `singleCharacter.h` | Cross-platform raw single-key input (`conio.h` on Windows, `termios.h` on Unix) |

For a full UML class diagram, see [`inventory_uml.drawio`](inventory_uml.drawio) in the repository root.

***

## Known Issues & To-Do

| ID | Description |
|---|---|
| TBD-1 | **Maximum Master name length** — No character limit enforced yet. Long names may overflow the shop header and battle arena display. |
| TBD-2 | **Equipment slot count** — The number of simultaneously equipped items is not yet documented or enforced. |
| TBD-3 | **NP damage scaling formula** — Defined in `servant.cpp` but not yet documented externally. |
| TBD-4 | **Simultaneous KO handling** — Behavior when both Servants reach 0 HP in the same turn (draw condition) is undefined. |
| TBD-5 | **Gold reward on battle victory** — No gold is currently awarded for defeating an enemy Servant. |
| TBD-6 | **Fate/Zero roster incomplete** — Only Saber (Artoria Zero) has full stats. Gilgamesh (Zero) has placeholder stats. The remaining five Fate/Zero Servants (Lancer/Diarmuid, Rider/Iskander, Caster/Gilles de Rais, Berserker/Lancelot, Assassin/Hassan Hundred Faces) need stats, NPs, ASCII art, mirrored art, and descriptions before Fate/Zero battles are playable. |

***
