# Software Requirements Specification
## for Terminal-Based Servant Battle Royale

**Version:** 1.0 approved
**Prepared by:** Franciszek Wasiak
**Organization:** School Project
**Created:** April 26, 2026
**Repository:** https://github.com/ClashFw/inventory_proj

***

## Revision History

| Name              | Date | Reason For Changes | Version |
|-------------------|---|---|---|
| Franciszek Wasiak | April 26, 2026 | Initial release | 1.0 |

***

## 1. Introduction

### 1.1 Purpose

This document specifies the software requirements for *Terminal-Based Servant Battle Royale* (`inventory_proj`), version 1.0. It covers the complete system, including the Naming Screen, Inventory Management, Shop, and Battle Arena subsystems.

### 1.2 Document Conventions

- **Bold** text is used for key terms defined in Appendix A: Glossary.
- All requirements use "shall" to indicate mandatory behavior and "should" to indicate recommended behavior.
- Requirements are assigned a unique ID (e.g., FR-01) and one of three priority levels: High, Medium, or Low.
- Each requirement has its own explicitly stated priority; higher-level requirements do not automatically propagate priority downward.

### 1.3 Intended Audience and Reading Suggestions

| Reader | Recommended Sections |
|---|---|
| **Developers** | All sections, with emphasis on Sections 2, 3, 4, and the Requirements Chart |
| **Testers** | Section 4 (Use Cases) and Section 5 (Nonfunctional Requirements) |
| **Project Managers / Instructors** | Sections 1 and 2 for overview; Requirements Chart for scope summary |
| **End Users** | Section 1.4 and Section 3.1 |

Start with Section 1 for context, then Section 2 for high-level understanding, then Sections 3–5 for detailed specifications.

### 1.4 Product Scope

*Terminal-Based Servant Battle Royale* is a cross-platform C++ terminal application created as a school project. Its purpose is to demonstrate object-oriented programming concepts — specifically class design, encapsulation, inheritance, and operator/function overloading — through a game that combines RPG grid inventory management with turn-based Servant combat. Players enter a **Master** name at launch, are randomly assigned a **Servant** from the Fate franchise, manage equipment in a grid inventory, purchase and sell items at the *Shop of Chaldea*, and duel enemy Servants in a side-by-side FGO-style battle arena.

### 1.5 References

None.

***

## 2. Overall Description

### 2.1 Product Perspective

This is a new, self-contained terminal application developed from scratch. It is not a component of a larger platform and does not replace any existing system. The application runs entirely within a terminal window and is composed of four tightly integrated subsystems — **Naming Screen**, **Inventory**, **Shop**, and **Battle Arena** — all managed by the central `Game` class. These subsystems share state through the `Player` object, which owns both the player's `Inventory` and their assigned `Servant`.

```
+--------------------------------------------+
|                   Game                     |
|  (Main loop, screen refresh, input dispatch)|
|                                             |
|   +----------+        +-----------------+  |
|   |  Player  |        |      Shop       |  |
|   | +--------+|       | (10-item stock, |  |
|   | |Servant ||       |  gold, search,  |  |
|   | | + NPs  ||       |  rarity/type    |  |
|   | +--------+|       |  filters)       |  |
|   | +--------+|       +-----------------+  |
|   | |Inventory|                            |
|   | | 2x3 grid|   +---------+              |
|   | +--------+|   |  Enemy  |              |
|   +----------+    | Servant |              |
|                   +---------+              |
+--------------------------------------------+
           |
    Terminal (stdout / stdin)
```

### 2.2 Product Functions

The system shall provide the following high-level functions:

- **Naming Screen**: Player enters their Master name at game launch before reaching the main menu.
- **Grid Inventory Management**: Navigate a 2×3 bag grid; grab/drop items (`G`), cancel drag (`Q`/`K`), equip items to slots (`T`), sell items (`H`), filter by rarity, and view item detail panels.
- **Shop System** (*Shop of Chaldea*): Browse up to 10 randomly generated items; buy with gold (manual placement or auto-place), sell via sell menu (`V`), live-search by name/type (`F`), filter by rarity and item type simultaneously, clear all filters (`C`), and restock at any time (`R`).
- **Battle Arena**: Select a Servant from the Fate series, fight randomly chosen enemy Servants from the same series, perform basic attacks or Noble Phantasms, view both combatants side-by-side in a wider FGO-style layout, and escape battles.
- **Item System**: Items of four types (Potion, Sword, Armor, Movement) in five rarities (Common, Magic, Rare, Legendary, Divine). Legendary and Divine items are shop-exclusive. Equipped items boost the player's effective combat stats.
- **Random Servant Assignment**: The player is randomly assigned one of the available Servants from the Fate/stay night or Fate/Zero roster at game start.

### 2.3 User Classes and Characteristics

**Casual Player**
The primary user class. Interacts with all subsystems at a basic level — navigating menus, purchasing items, and fighting battles without using advanced filtering or multi-slot equip strategies. Expected to have minimal prior game knowledge. Frequency of use: occasional. Primarily cares about readable item info, clear navigation, and understandable battle feedback.

**Experienced / Power User**
A player familiar with the game's systems who uses advanced features: simultaneous rarity + type filtering, live search in the shop, multi-step inventory optimization (grab/drop/equip), and Noble Phantasm targeting strategies. Interacts frequently and expects fast, fully keyboard-driven responsiveness with no input lag.

### 2.4 Operating Environment

| Platform | Version | Notes |
|---|---|---|
| **Windows** | 10 / 11 | Terminal with ANSI escape support (Windows Terminal, PowerShell). Uses `conio.h`. |
| **macOS** | 10.15 Catalina and later | Terminal.app or iTerm2. Uses `termios.h`. |
| **Linux** | Ubuntu 20.04+, Debian 10+ | Standard terminal emulator with ANSI support. Uses `termios.h`. |
| **Build Tools** | CMake 3.16+, C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+) | Required on all platforms. |

### 2.5 Design and Implementation Constraints

- The application must be written in **C++17**. Structured bindings and other C++17 features are actively used; no lower standard is acceptable.
- The build system is **CMake 3.16+**. No other build system is supported.
- All terminal rendering uses **ANSI escape codes** for color and cursor control. No external GUI library (SDL, ncurses, etc.) may be used.
- Platform-specific raw input is handled via `conio.h` (Windows) and `termios.h` (Unix/macOS), abstracted through `singleCharacter.h`. This cross-platform contract must not be broken.
- The project is a school assignment; no paid or proprietary third-party libraries may be used.
- Memory management follows RAII principles. Manual `new`/`delete` is used only where justified (e.g., `ItemGenerator`, shop item ownership).
- **Legendary** and **Divine** rarities are designated `shopOnly = true` in the `Item` class. The inventory generator must not produce these rarities; only the shop generator (`generateShopItem()`) may do so.

### 2.6 User Documentation

The following documentation is delivered with the project:

- **README.md** (included): Covers all controls, features, Servant stats, item pricing, build instructions, and class overview.
- **In-game Key Reference**: Each subsystem (inventory, shop, battle arena) displays active keybindings on-screen at all times.
- **UML Class Diagram** (`inventory_uml.drawio`): Included in the repository for developer reference.

For a production release, the following would additionally be recommended: a short video walkthrough tutorial and a dedicated project wiki page.

### 2.7 Assumptions and Dependencies

- It is assumed that the player's terminal supports **ANSI escape codes**. If it does not, rendering artifacts will appear and the game will be unplayable.
- It is assumed that the terminal window is at minimum **120 columns × 40 rows** to render the FGO-style side-by-side battle arena correctly. Smaller terminals produce undefined layout behavior.
- The project depends only on the **C++ Standard Library**; no third-party dependencies.
- It is assumed that the build environment has CMake 3.16+ and a C++17-compatible compiler installed and available on PATH.
- The **Fate/Zero Servant roster is incomplete** in the current version (only Saber and Archer are implemented; Archer/Gilgamesh Zero has placeholder `-` stats). The full roster addition is a known outstanding item (see Appendix C).

***

## 3. External Interface Requirements

### 3.1 User Interfaces

The application uses a **full-screen terminal UI** rendered entirely with ANSI escape codes and Unicode box-drawing characters. The following standards apply across all screens:

- All UI panels use **double box-drawing borders** (e.g., `╔═╗║╚╝╠╣`) for primary frames and **single borders** (`─│`) for inner dividers.
- **Color coding by rarity**: Common = white (`\033[37m`), Magic = blue (`\033[38;5;39m`), Rare = gold (`\033[38;5;220m`), Legendary = orange (`\033[38;5;202m`), Divine = magenta (`\033[38;5;201m`).
- **Color coding by item type**: Potion = green, Sword = red, Armor = blue, Movement = orange.
- **Shop header** displays "✿ SHOP OF CHALDEA ✿" with current gold treasury inline.
- **Screen layout** has four distinct modes:
    - **Naming Screen**: Centered input prompt for the Master name before the main menu.
    - **Main Menu**: Centered list navigated with W/S + Enter.
    - **Inventory/Shop Screen**: Left panel = grid or item list; right panel = item detail / gold / active filters.
    - **Battle Arena**: Side-by-side Servant ASCII art (standard + mirrored for the enemy), HP bars below each Servant, action menu at the bottom. The arena is wider than standard columns to accommodate two Servants.
- **Screen resolution**: Designed for a minimum of 120×40 terminal characters. Behavior on smaller terminals is undefined.
- **Keyboard only**: No mouse input is used or required.
- **Status/error messages**: Displayed inline within the active panel (e.g., "Not enough gold!", "Inventory full.", "Already used!").
- **Smart Name Display**: Inventory grid cells show the shortest unambiguous prefix that distinguishes items from each other, not the full name.
- Section 508 / accessibility compliance: Not applicable for this version (terminal-only, keyboard-driven).

### 3.2 Hardware Interfaces

No dedicated hardware interfaces. The only hardware dependencies are a standard keyboard (for input) and a monitor (for terminal display). No special devices, sensors, or peripherals are required.

### 3.3 Software Interfaces

The following internal software components are used:

- **C++ Standard Library (C++17)**: Containers (`std::vector`, `std::string`), algorithms (`std::transform`, `std::find`), random number generation (`<random>`, `rand()`), and standard I/O.
- **`conio.h`** (Windows) / **`termios.h`** (macOS/Linux): Raw single-character keyboard input, abstracted by `singleCharacter.h`.
- **CMake 3.16+**: Cross-platform build configuration (`CMakeLists.txt`).
- No external databases, web servers, or network components are used.

### 3.4 Communications Interfaces

None. The application is entirely offline and does not communicate with any external servers, APIs, databases, or network services. No internet connection is required or used at any point.

***

## 4. System Use Cases

### Use Case Diagram (Textual)

```
[ Player ] ---> (U1) Enter Master Name
[ Player ] ---> (U2) Manage Inventory
[ Player ] ---> (U3) Buy Item from Shop
[ Player ] ---> (U4) Sell Item
[ Player ] ---> (U5) Engage in Battle
[ Player ] ---> (U6) Use Noble Phantasm
(U5) includes --> (U6)
(U3) includes --> (U4) [optional sell path]
```

***

### Enter Master Name (U1)

- **Objective**: The player enters their Master name at game launch. The name is stored in `Game::masterName` and displayed throughout the session.
- **Priority**: High
- **Source**: Franciszek Wasiak (Developer / End-user)
- **Actors**: Player
- **Flow of Events**

  **Basic Flow**
    1. Game launches and displays the naming screen.
    2. Player types their desired Master name.
    3. Player presses Enter to confirm.
    4. System stores the name in `Game::masterName`.
    5. System proceeds to the main menu.

  **Alternative Flow 1** — Player presses Enter with an empty name:
    - System displays: "Name cannot be empty."
    - Return to step 2.

  **Exception Flow 1** — Input exceeds maximum character limit:
    - System truncates input to the allowed maximum and proceeds.

- **Includes**: None
- **Preconditions**: Game has just launched; no `masterName` has been set yet.
- **Postconditions**: `Game::masterName` is set; player is on the main menu.
- **Notes/Issues**: Maximum name length is currently undefined — see Appendix C, TBD-1.

***

### Manage Inventory (U2)

- **Objective**: The player navigates their 2×3 inventory grid, repositions items via drag-and-drop, equips items to combat slots, and sells unwanted items.
- **Priority**: High
- **Source**: Franciszek Wasiak (Developer / End-user)
- **Actors**: Player
- **Flow of Events**

  **Basic Flow**
    1. Player selects "Inventory" from the main menu.
    2. System renders the 2×3 inventory grid. Grid cells show the shortest unambiguous item name prefix.
    3. Player navigates using WASD.
    4. Player presses `G` to grab the item at the cursor position.
    5. Player moves the cursor to a target slot and presses `G` again to drop or swap.
    6. System updates the grid to reflect new item positions.
    7. Player presses Backspace to exit the inventory.

  **Alternative Flow 1** — Player presses `Q` or `K` to cancel drag:
    - System returns the grabbed item to its original slot without moving it.

  **Alternative Flow 2** — Player presses `T` on an item to equip it:
    - System moves the item to the equipment slot.
    - Player's effective combat stats are recalculated to include the item's percent effect.

  **Alternative Flow 3** — Player presses `H` on an item to sell it:
    - System calculates a sale price of 80–90% of the item's base value (randomised per sale).
    - System displays the sale price and prompts confirmation.
    - Player confirms; gold is credited to the shop balance; item is removed from the grid.

  **Alternative Flow 4** — Player presses `1`, `2`, or `3` to filter by rarity:
    - System highlights only items matching the selected rarity (Common / Magic / Rare).
    - Player presses `0` to clear the filter.

  **Alternative Flow 5** — Player presses `I` to toggle the item info panel:
    - System renders a detailed stats panel (name, type, rarity, price, percent effect, durability, level) for the item at the cursor.

  **Alternative Flow 6** — Player presses `P` to open the Shop directly from inventory:
    - System transitions to the Shop screen without returning to the main menu.

  **Exception Flow 1** — Player attempts to drop a grabbed item onto an occupied slot:
    - Items are swapped rather than stacked; no item is lost.

- **Includes**: None
- **Preconditions**: Player is on the main menu with an existing `Player` object and `Inventory`.
- **Postconditions**: Inventory state is updated; equipped items affect `Player` effective combat stats.
- **Notes/Issues**: The number of equipment slots available to the player is TBD — see Appendix C, TBD-2.

***

### Buy Item from Shop (U3)

- **Objective**: The player browses the *Shop of Chaldea* (up to 10 randomly generated items) and purchases one, placing it in their inventory.
- **Priority**: High
- **Source**: Franciszek Wasiak (Developer / End-user)
- **Actors**: Player
- **Flow of Events**

  **Basic Flow**
    1. Player selects "Shop" from the main menu (or presses `P` from inventory).
    2. System displays up to 10 items with columns: index, name, type, rarity, price, and effect percentage.
    3. Player navigates with W/S and selects an item by pressing `B`.
    4. System prompts the player to choose an inventory grid slot (press ESC to auto-place).
    5. System deducts the item's gold cost from the player's balance.
    6. System places the item in the chosen (or auto-selected) grid slot.

  **Alternative Flow 1** — Player has insufficient gold:
    - System displays: "Not enough gold!"
    - Return to step 3.

  **Alternative Flow 2** — Player's inventory is full and no auto-place slot is available:
    - System displays: "Inventory full!"
    - Return to step 3.

  **Alternative Flow 3** — Player presses `F` to open live search:
    - System accepts a text query and filters the item list to show only names or types containing the query string (case-insensitive).
    - Player presses `C` to clear all active filters and the search query simultaneously.

  **Alternative Flow 4** — Player filters by rarity or type simultaneously:
    - Rarity and type filters compound: only items matching both active filters are shown.

  **Alternative Flow 5** — Player presses `R` to restock the shop:
    - System generates a new set of 10 items via `generateShopItem()`, which may include Legendary and Divine rarities.
    - The previous stock is discarded.

  **Exception Flow 1** — Player presses `E` at any time:
    - System exits the shop and returns to the main menu.

- **Includes**: None
- **Preconditions**: Player is on the main menu; player has a gold balance (initialised on first shop entry).
- **Postconditions**: Player's gold is reduced by the item's price; item is placed in inventory.
- **Notes/Issues**: None.

***

### Sell Item (U4)

- **Objective**: The player sells an item either directly from inventory (`H`) or through the shop sell menu (`V`), receiving a gold refund.
- **Priority**: High
- **Source**: Franciszek Wasiak (Developer / End-user)
- **Actors**: Player
- **Flow of Events**

  **Basic Flow — Sell from Inventory**
    1. Player navigates to an item in the inventory grid.
    2. Player presses `H`.
    3. System calculates the sale price: 80–90% of the item's base price (randomised).
    4. System displays the sale price and requests confirmation.
    5. Player confirms; gold is added to the player balance; item is removed from the grid.

  **Basic Flow — Sell from Shop Menu**
    1. While in the shop, player presses `V` to open the sell menu.
    2. System displays the player's inventory items available for sale.
    3. Player selects an item to sell.
    4. System adds `item->getPrice() / 2` (50% of base price) to the player's gold.
    5. Item is removed from inventory and added to the shop's stock.

  **Alternative Flow 1** — Player cancels sell confirmation:
    - No gold is transferred; the item remains in the grid.

  **Exception Flow 1** — Player attempts to sell from an empty inventory:
    - System displays: "Nothing to sell."

- **Includes**: None
- **Preconditions**: Player has at least one item in their inventory.
- **Postconditions**: Gold is credited to the player; item is removed from inventory.
- **Notes/Issues**: The sell-from-inventory price (80–90%) and the sell-via-shop price (50%) are intentionally different and reflect different sell channels.

***

### Engage in Battle (U5)

- **Objective**: The player selects "Battle Arena" from the main menu and fights a randomly chosen enemy Servant from the same Fate series, in a side-by-side FGO-style arena.
- **Priority**: High
- **Source**: Franciszek Wasiak (Developer / End-user)
- **Actors**: Player
- **Flow of Events**

  **Basic Flow**
    1. Player selects "Battle Arena" from the main menu.
    2. System randomly selects an enemy Servant from the same Fate series (Fate/stay night or Fate/Zero) as the player's Servant.
    3. System renders both Servants side-by-side: player Servant ASCII art on the left, enemy Servant **mirrored ASCII art** on the right, with HP bars below each.
    4. Player chooses an action: `A` (Basic Attack) or `1–9` (Noble Phantasm by index).
    5. System calculates hit chance: `75% + (playerAgility − enemyAgility) × 2`, capped between 10% and 95%.
    6. If the attack hits, damage is applied to the enemy's HP.
    7. Enemy Servant automatically performs a counterattack using the same agility-based formula.
    8. Steps 4–7 repeat until one Servant's HP reaches 0.
    9. System displays the battle result: Victory or Defeat.
    10. Player presses `B` to immediately battle again, or any other key to return to the main menu.

  **Alternative Flow 1** — Player uses a Noble Phantasm (step 4):
    - System checks if the chosen NP has already been used this battle (NPs are single-use per battle).
    - If available: NP is activated; scaled damage is applied; NP is flagged as used for this battle.
    - If already used: System displays "Already used!" and returns to step 4.

  **Alternative Flow 2** — Player presses `E` to escape:
    - System ends the battle with no result and returns to the main menu.

  **Exception Flow 1** — Both HP values reach 0 in the same turn:
    - Behavior is TBD — see Appendix C, TBD-4.

- **Includes**: U6 (Use Noble Phantasm)
- **Preconditions**: Player is on the main menu; player has an assigned Servant with valid stats.
- **Postconditions**: Battle result is displayed; NP usage flags are reset for the next battle.
- **Notes/Issues**: Gold reward for winning a battle is TBD — see Appendix C, TBD-5.

***

### Use Noble Phantasm (U6)

- **Objective**: During battle, the player activates one of their Servant's Noble Phantasms for enhanced scaled damage.
- **Priority**: Medium
- **Source**: Franciszek Wasiak (Developer / End-user)
- **Actors**: Player
- **Flow of Events**

  **Basic Flow**
    1. During battle, player presses a number key `1–9` corresponding to a Noble Phantasm by index.
    2. System checks whether the NP has already been used this battle.
    3. System calculates NP damage scaled from the Servant's STR stat.
    4. Damage is applied to the enemy; the NP is flagged as used for the remainder of the battle.

  **Alternative Flow 1** — NP already used:
    - System displays "Already used!" and returns to the action selection prompt.

  **Alternative Flow 2** — Key pressed has no corresponding NP:
    - System ignores the input and returns to the action selection prompt.

- **Includes**: None
- **Preconditions**: Player is in an active battle; the selected NP has not yet been used this battle.
- **Postconditions**: Enemy HP is reduced; the NP is flagged as used for this battle session.
- **Notes/Issues**: NP damage scaling formula is TBD — see Appendix C, TBD-3.

***

## 5. Other Nonfunctional Requirements

### 5.1 Performance Requirements

- The game shall render any screen (inventory, shop, battle arena) within **100 milliseconds** of a keypress under normal operating conditions.
- Random item generation for a 10-item shop restock shall complete in under **50 milliseconds**.
- The application shall consume no more than **50 MB of RAM** during normal operation.
- The application shall launch and display the naming screen within **500 milliseconds** of execution.

### 5.2 Safety Requirements

No safety-critical requirements apply. The application is a single-player terminal game with no real-world hardware control, financial transactions, or persistent user data storage.

### 5.3 Security Requirements

- The application stores no personal data, passwords, or sensitive user information.
- The only user-provided input is the Master name (stored in memory only; never written to disk).
- No network communication occurs; there is no remote attack surface.

### 5.4 Software Quality Attributes

- **Portability**: The application shall compile and run identically on Windows 10+, macOS 10.15+, and Ubuntu 20.04+ using only the CMake build system and the C++ Standard Library, with no platform-specific source changes required by the developer beyond the existing `singleCharacter.h` abstraction.
- **Maintainability**: Each subsystem (Inventory, Shop, Battle Arena) shall be encapsulated in its own class with a clean public interface, enabling independent modification without cascading changes.
- **Reliability**: The application shall not crash on invalid or unexpected keyboard input. All input shall be validated before processing.
- **Usability**: All active keybindings shall be visible on-screen within each subsystem at all times, so players never need to consult external documentation during gameplay.
- **Testability**: Each class (`Item`, `Servant`, `Inventory`, `Shop`, `Player`) shall expose public methods that can be exercised independently of the `Game` main loop.
- **Correctness**: The `shopOnly` flag on `Item` shall be enforced at the generator level. Items with `shopOnly = true` (Legendary, Divine) shall never appear in the in-game inventory via the normal `ItemGenerator`; only `generateShopItem()` may produce them.

### 5.5 Business Rules

- **Starting gold** is randomised on first shop entry: 200–400g with 70% probability, 400–500g with 20% probability, 500–800g with 10% probability.
- **Sell from inventory** (`H` key): yields 80–90% of the item's base price, randomised per sale transaction.
- **Sell via shop sell menu** (`V` key): yields exactly 50% of the item's base price (`item->getPrice() / 2`).
- **Noble Phantasms** are single-use per battle and reset automatically between battles.
- **Enemy Servants** are always drawn from the same Fate series (Fate/stay night or Fate/Zero) as the player's assigned Servant.
- **Rarity distribution for generated items** (inventory generator): Common 40%, Magic 30%, Rare 30%. Legendary and Divine may appear in shop stock only.
- **Hit chance formula**: `75% + (playerAgility − enemyAgility) × 2`, capped to the range [10%, 95%].

***

## 6. Other Requirements

None at this time.

***

## System Requirements Chart

| ID | Priority | Type | Source | Contained in Use Case(s) | Description |
|---|---|---|---|---|---|
| FR-01 | High | F | Franciszek Wasiak (Developer) | U1 | The system shall display a naming screen on game launch and store the entered Master name in `Game::masterName`. |
| FR-02 | High | F | Franciszek Wasiak (Developer) | U2 | The system shall display a 2×3 inventory grid navigable with WASD keys. |
| FR-03 | High | F | Franciszek Wasiak (Developer) | U2 | The system shall allow the player to grab and drop items using the `G` key; pressing `Q` or `K` shall cancel an active drag. |
| FR-04 | High | F | Franciszek Wasiak (Developer) | U2 | The system shall allow the player to equip items to an equipment slot using the `T` key, updating effective combat stats. |
| FR-05 | High | F | Franciszek Wasiak (Developer) | U2 U4 | The system shall allow the player to sell an inventory item with `H`, yielding 80–90% of its base price (randomised). |
| FR-06 | Medium | F | Franciszek Wasiak (Developer) | U2 | The system shall allow filtering inventory items by rarity using keys `1` (Common), `2` (Magic), `3` (Rare), and `0` to clear. |
| FR-07 | Medium | F | Franciszek Wasiak (Developer) | U2 | The system shall display the shortest unambiguous name prefix for each item in inventory grid cells. |
| FR-08 | High | F | Franciszek Wasiak (Developer) | U3 | The system shall generate up to 10 random items on shop entry or restock. Shop items may include Legendary and Divine rarities. |
| FR-09 | High | F | Franciszek Wasiak (Developer) | U3 | The system shall deduct the item's gold cost from the player's balance on purchase and prevent purchase if gold is insufficient. |
| FR-10 | High | F | Franciszek Wasiak (Developer) | U3 | The system shall prevent purchase if the player's inventory is full. |
| FR-11 | Medium | F | Franciszek Wasiak (Developer) | U3 | The system shall support live name/type search filtering in the shop via the `F` key (case-insensitive). |
| FR-12 | Medium | F | Franciszek Wasiak (Developer) | U3 | The system shall support simultaneous rarity and item type filtering in the shop, compounding both constraints. |
| FR-13 | Medium | F | Franciszek Wasiak (Developer) | U3 | The system shall allow the player to clear all active shop filters and the search query with the `C` key. |
| FR-14 | High | F | Franciszek Wasiak (Developer) | U4 | The system shall allow the player to open a sell menu from the shop via the `V` key, selling inventory items for 50% of their base price. |
| FR-15 | High | F | Franciszek Wasiak (Developer) | U5 | The system shall randomly select an enemy Servant from the same Fate series as the player's Servant for each battle. |
| FR-16 | High | F | Franciszek Wasiak (Developer) | U5 | The system shall display both Servants side-by-side in the battle arena: player Servant (standard ASCII art) on the left, enemy Servant (mirrored ASCII art) on the right, each with HP bars. |
| FR-17 | High | F | Franciszek Wasiak (Developer) | U5 | The system shall calculate hit chance using: `75% + (playerAGI − enemyAGI) × 2`, capped between 10% and 95%. |
| FR-18 | High | F | Franciszek Wasiak (Developer) | U5 U6 | The system shall allow the player to use a Noble Phantasm once per battle via number keys `1–9`. Attempting to reuse a spent NP shall display "Already used!". |
| FR-19 | High | F | Franciszek Wasiak (Developer) | U5 | The system shall allow the player to escape a battle at any time by pressing `E`. |
| FR-20 | Medium | F | Franciszek Wasiak (Developer) | U3 | Starting gold shall be randomised: 200–400g (70%), 400–500g (20%), 500–800g (10%). |
| FR-21 | Medium | F | Franciszek Wasiak (Developer) | — | The `Item` class shall enforce a `shopOnly` flag. Legendary and Divine items shall only be generated by `generateShopItem()`; the standard `ItemGenerator` shall never produce them. |
| NFR-01 | High | NF | Franciszek Wasiak (Developer) | N/A | The system shall render any screen within 100ms of a keypress under normal conditions. |
| NFR-02 | High | NF | Franciszek Wasiak (Developer) | N/A | The application shall compile and run on Windows 10+, macOS 10.15+, and Ubuntu 20.04+ without source code changes. |
| NFR-03 | High | NF | Franciszek Wasiak (Developer) | N/A | The application shall not crash on invalid or unexpected keyboard input; all input shall be validated before processing. |
| NFR-04 | Medium | NF | Franciszek Wasiak (Developer) | N/A | All active keybindings shall be visible on-screen within each subsystem at all times. |
| NFR-05 | Medium | NF | Franciszek Wasiak (Developer) | N/A | The application shall use no more than 50 MB of RAM during normal operation. |
| NFR-06 | Medium | NF | Franciszek Wasiak (Developer) | N/A | Each subsystem class (`Inventory`, `Shop`, `Servant`, `Item`) shall expose public methods exercisable independently of the `Game` main loop. |

***

## Appendix A: Glossary

| Term | Definition |
|---|---|
| **Servant** | A heroic spirit summoned as a combatant. Defined by stats (HP, STR, DUR, AGI), Noble Phantasms, ASCII art, mirrored ASCII art, a description, and a Fate series designation. |
| **Master** | The player's in-game role. The name entered on the naming screen is stored in `Game::masterName` and also in the `Servant::masterName` field. |
| **Noble Phantasm (NP)** | A Servant's named special attack. Single-use per battle; deals damage scaled from the Servant's STR stat. |
| **Grid Inventory** | The 2×3 bag layout used to hold items. Each cell holds exactly one item. |
| **Rarity** | Item quality tier: Common, Magic, Rare (all generators), Legendary, Divine (shop-exclusive). Higher rarity = stronger stats and higher price. |
| **shopOnly** | An `Item` boolean flag (`true` for Legendary and Divine). Prevents these rarities from appearing via the standard `ItemGenerator`. |
| **Gold** | In-game currency used to purchase items at the shop. Starting balance is randomised on first shop entry. |
| **ANSI Escape Codes** | Terminal control sequences used for text color, cursor movement, and screen clearing. |
| **CMake** | A cross-platform build system generator used to build the project. Minimum required version: 3.16. |
| **WASD** | Standard keyboard navigation: W (up), A (left), S (down), D (right). |
| **STR / DUR / AGI** | Servant combat stats: Strength (attack power and NP damage), Durability (damage reduction), Agility (hit chance modifier). |
| **mirroredArt** | A horizontally flipped version of a Servant's ASCII art used to render the enemy Servant in the right panel of the battle arena. |
| **servantDescription** | A short flavour-text string describing a Servant, displayed in the battle arena or info panels. |
| **Shop of Chaldea** | The in-game name for the shop screen, as displayed in the shop header. |

***

## Appendix B: Analysis Models

The UML Class Diagram is available in the repository as `inventory_uml.drawio`. No additional analysis models are included in this SRS. Data flow diagrams and state-transition diagrams will be produced in the high-level design deliverable.

***

## Appendix C: To Be Determined List

1. **TBD-1 — Maximum Master name length**: The naming screen does not yet enforce a character limit. A reasonable maximum (e.g., 20 characters) should be defined and enforced to prevent layout overflow in the shop header and battle arena display.
2. **TBD-2 — Equipment slot count**: The number of equipment slots available to the player (e.g., 1 or 2 simultaneous equips) is not explicitly documented. This affects how many items can boost combat stats at once.
3. **TBD-3 — Noble Phantasm damage scaling formula**: The exact formula for calculating NP damage from the Servant's STR stat is present in `servant.cpp` but not yet documented in any external specification. It should be extracted and recorded here.
4. **TBD-4 — Simultaneous KO handling**: Behavior when both Servant HP values reach 0 in the same turn (draw condition) is not explicitly handled in the current implementation. A draw screen or tie-breaker rule should be defined.
5. **TBD-5 — Gold reward on battle victory**: Whether defeating an enemy Servant awards the player with gold is not yet implemented or specified. Adding this would significantly improve inventory and shop progression loop balance.
6. **TBD-6 — Fate/Zero roster completion**: The current Fate/Zero Servant table in the README lists only Saber (Artoria Zero) with full stats, and Archer (Gilgamesh Zero) with placeholder `-` values. The remaining five Fate/Zero Servants (Lancer/Diarmuid, Rider/Iskander, Caster/Gilles de Rais, Berserker/Lancelot, Assassin/Hassan Hundred Faces) need to be implemented with stats, Noble Phantasms, ASCII art, mirrored art, and descriptions before the Fate/Zero series can be used in battle.