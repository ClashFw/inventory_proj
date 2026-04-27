# Software Requirements Specification

Title: Holy Grail War — Terminal RPG
Version: 1.0
Prepared by: [Your Name]
Organization: [Your Organization]
Date: April 27, 2026

Presentation: https://gamma.app/docs/Terminal-Based-Servant-Battle-Royale-azwmq41nce7pymc

---

## Revision History

Name         | Date       | Reason For Changes | Version
-------------|------------|--------------------|--------
[Your Name]  | 2026-04-27 | Initial SRS        | 1.0

---

## Table of Contents

1. Introduction
   1.1 Purpose
   1.2 Document Conventions
   1.3 Intended Audience and Reading Suggestions
   1.4 Product Scope
   1.5 References
2. Overall Description
   2.1 Product Perspective
   2.2 Product Functions
   2.3 User Classes and Characteristics
   2.4 Operating Environment
   2.5 Design and Implementation Constraints
   2.6 User Documentation
   2.7 Assumptions and Dependencies
3. External Interface Requirements
   3.1 User Interfaces
   3.2 Hardware Interfaces
   3.3 Software Interfaces
   3.4 Communications Interfaces
4. System Use Cases
5. Other Nonfunctional Requirements
   5.1 Performance Requirements
   5.2 Safety Requirements
   5.3 Security Requirements
   5.4 Software Quality Attributes
   5.5 Business Rules
6. Other Requirements
7. System Requirements Chart
   Appendix A: Glossary
   Appendix B: Analysis Models
   Appendix C: To Be Determined List

---

## 1. Introduction

### 1.1 Purpose

This Software Requirements Specification describes the functional and
non-functional requirements for the Holy Grail War Terminal RPG, version 1.0.
The document guides development, testing, and maintenance of the system.
It covers the complete feature set including Master registration, Servant
summoning, inventory management, shop mechanics, turn-based battle, and
dual Holy Grail War progression.

### 1.2 Document Conventions

- Requirements are written in the form "The system shall...".
- Priority levels used throughout: Low, Medium, High.
- Functional requirements are tagged F, non-functional requirements tagged NF.
- All keyboard shortcuts use the format KEY (e.g. W, S, Enter, Backspace).
- ANSI escape codes and UTF-8 are assumed throughout for terminal rendering.

### 1.3 Intended Audience and Reading Suggestions

This document is intended for the following readers:

- Developers: Read all sections. Pay particular attention to Sections 3, 4,
  and the System Requirements Chart.
- Testers: Focus on Section 4 (Use Cases) and the Requirements Chart
  to derive test cases.
- Project Managers: Read Sections 1, 2, and the Requirements Chart
  for scope and priority overview.
- End Users / Players: Section 2.2 (Product Functions) provides a high-level
  summary of what the game does.

Suggested reading order:
Section 1 -> Section 2 -> Section 4 -> Section 3 -> Requirements Chart.

### 1.4 Product Scope

Holy Grail War Terminal RPG is a single-player, terminal-based role-playing
game written in C++. The player takes the role of a Master from either
Fate/stay night or Fate/Zero, summons a Servant based on their typed name,
manages an item inventory, purchases and sells equipment in a shop, and fights
enemy Servants in turn-based combat. The game spans two Holy Grail Wars.
Clearing the first war unlocks the second. The player has three lives across
the full run. The product is entirely self-contained and requires no network
access or external services.

### 1.5 References

- Fate/stay night visual novel — TYPE-MOON, 2004
- Fate/Zero light novel — Gen Urobuchi, TYPE-MOON, 2006-2007
- ISO/IEC 14882:2017 — C++17 Standard
- CMake documentation — https://cmake.org/documentation/
- Project presentation — https://gamma.app/docs/Terminal-Based-Servant-Battle-Royale-azwmq41nce7pymc

---

## 2. Overall Description

### 2.1 Product Perspective

Holy Grail War Terminal RPG is a new, self-contained product. It is not part
of a larger system and has no dependency on external applications, databases,
or network services. It runs entirely inside a terminal emulator. The game is
inspired thematically by the Fate anime and visual novel franchise but is an
independent project.

Internal modules:

    +------------------+
    |    main.cpp      |
    +------------------+
             |
             v
    +------------------+       +------------------+
    |    Game          |<----->|    Player        |
    |  (game.cpp)      |       |  (player.cpp)    |
    +------------------+       +------------------+
        |       |                      |
        v       v                      v
    +-------+ +-------+         +------------+
    | Shop  | | Enemy |         | Inventory  |
    +-------+ +-------+         +------------+
        |                              |
        v                              v
    +-------+                   +----------+
    | Item  |                   | Servant  |
    +-------+                   +----------+

### 2.2 Product Functions

- Master Registration: Player types a name; canon names summon specific
  Servants and place the player in the correct war.
- Servant Showcase: After summoning, the player sees their Servant's ASCII
  art, stats, and Noble Phantasms.
- Main Menu: Navigation hub with Inventory, Shop, Battle, Gallery, Quit.
- Inventory Management: Bag slots (storage) and equip slots (stat boosts).
  Drag-and-drop interface. Item info panel. Servant status panel with live
  equipment bonuses shown to the right of each rank bar.
- Shop: Buy and sell items. Filter by rarity and type. Search by name.
  Restock.
- Battle Arena: Turn-based combat. Basic attack and Noble Phantasm actions.
  Hit chance and damage based on Servant stats. Gold reward on victory.
- Dual War Progression: Clear Fate/stay night or Fate/Zero, then start the
  second war with reset inventory and gold but carried-over lives.
- Lives System: 3 lives across both wars. Third loss ends the game.
- Gallery: Side-by-side roster of both wars with colour-coded Servant status.

### 2.3 User Classes and Characteristics

Regular Player
The primary user. Has basic keyboard navigation skills. May or may not be
familiar with the Fate franchise. Interacts with all features: naming,
inventory, shop, battle, gallery. Expected to read on-screen hints for
controls. No technical knowledge required.

Power User / Speedrunner
Familiar with all game mechanics and canon Master names. Will deliberately
type specific Master names to control which war and Servant they start with.
Uses keyboard shortcuts efficiently and engages fully with Noble Phantasm
and equipment mechanics.

### 2.4 Operating Environment

- Operating Systems: Linux (Ubuntu 20.04+), macOS 12+,
  Windows 10/11 with Windows Terminal
- Compiler: GCC 9+ or Clang 11+ with C++17 support
- Build System: CMake 3.10 or higher
- Terminal: Any terminal emulator with ANSI 256-colour support and UTF-8
  encoding (GNOME Terminal, iTerm2, Windows Terminal).
  Classic Windows CMD.exe is NOT supported.
- Hardware: Any machine capable of running the above operating systems.
  No GPU or network card required.

### 2.5 Design and Implementation Constraints

- The entire UI must be rendered using ANSI escape codes and UTF-8
  box-drawing characters. No external GUI libraries may be used.
- All input must be read as raw single-character keypresses without requiring
  Enter confirmation except in text input fields.
- The project must build using CMake and produce a single executable binary.
- The codebase must compile under C++17 with no external dependencies beyond
  the standard library.
- All Servant and item data is hardcoded at compile time. No external data
  file or database is used.
- The game is single-player only. No networking or multiplayer is in scope.

### 2.6 User Documentation

For a production release the following documentation should be provided:

- README.md: Included in the repository. Covers build instructions, all
  keyboard controls, Master name table, and game mechanics summary.
- In-game control hints: Printed at the bottom of each screen so players
  can always see available actions without external documentation.
- Man page: A Unix man page for the binary describing launch options
  and basic gameplay.

### 2.7 Assumptions and Dependencies

- It is assumed the player's terminal supports ANSI 256-colour codes and
  UTF-8. The game will display incorrectly otherwise.
- It is assumed the C++ standard library provides stdlib>, <vector>,
  <set>, <string>, and <algorithm> with standard-conforming behaviour.
- The game depends on std::rand() for randomisation. It is assumed that
  std::srand() is called with a time-based seed in main.cpp before any
  random servant or item generation.
- It is assumed the terminal window is at least 80 columns wide and 40 rows
  tall. Smaller terminals will cause layout wrapping.

---

## 3. External Interface Requirements

### 3.1 User Interfaces

- The entire interface is rendered in a terminal using ANSI colour codes
  and UTF-8 box-drawing characters.
- All screens use a consistent dark-background colour scheme with gold,
  cyan, and red accent colours.
- Each screen has a clearly labelled header rendered as a double-line box
  or horizontal rule.
- A control hint bar is displayed at the bottom of every interactive screen.
- All text input fields show a blinking cursor character and support
  backspace for correction.
- Error messages are displayed inline in red. No separate error screen.
- The game does not support mouse input. All interaction is keyboard-only.
- Minimum supported terminal width: 80 columns. Minimum height: 40 rows.
- The game is not Section 508 compliant due to reliance on colour for game
  state. Text labels ([FALLEN], [WINNER], [YOU]) are included alongside
  colours as a partial mitigation.

### 3.2 Hardware Interfaces

None. The product has no direct hardware interfaces beyond the keyboard
(input) and terminal display (output), both mediated by the operating system.

### 3.3 Software Interfaces

- C++ Standard Library (C++17): Used for all data structures, I/O,
  and algorithms.
- CMake 3.10+: Used as the build system to produce the final binary.
- Operating system terminal API: On Linux/macOS, termios.h is used via
  singleCharacter.h to set raw terminal mode for single-keypress input.
  On Windows, equivalent CRT functions are used.

No database, web server, or third-party library is used.

### 3.4 Communications Interfaces

None. The product does not connect to any external network, server, or API.
All data is local and in-memory for the duration of a session.
No data is persisted between sessions.

---

## 4. System Use Cases

Overall Use Case Summary:

    Player
      |
      |---> [U1] Register as Master
      |---> [U2] Manage Inventory
      |---> [U3] Buy Item from Shop
      |---> [U4] Fight in Battle Arena
      |---> [U5] View Gallery
      |---> [U6] Progress to Second War


### Use Case U1: Register as Master

1. Objective: The player enters a Master name to summon their Servant
   and be placed in the correct Holy Grail War.
2. Priority: High
3. Source: Jan Kowalski, End-user
4. Actors: Player
5. Flow of Events
  1. Basic Flow
    1. System displays the Master Registration screen.
    2. Player types their name using keyboard input.
    3. Player presses Enter to confirm.
    4. System matches the name (case-insensitive, whitespace-stripped)
       against the canon Master table.
    5. System selects the corresponding Servant and war.
    6. System displays the Servant Profile screen.
    7. System transitions to the Main Menu.
  2. Alternative Flow 1 - Player presses Esc
    1. System assigns a random Servant from a random war.
    2. System displays the Servant Profile screen.
    3. System transitions to the Main Menu.
  3. Alternative Flow 2 - Name not recognised
    1. System assigns a random Servant.
    2. Continue from Basic Flow step 6.
  4. Alternative Flow 3 - Player types "Kirei Kotomine"
    1. System randomly selects Lancer (Cu) in FSN or
       Assassin (Hassan Zero) in Zero.
    2. Continue from Basic Flow step 6.
  5. Exception Flow 1 - Player presses Enter with empty input
    1. System does nothing and returns to step 5.1.2.
6. Includes: None
7. Preconditions: Game has been launched. No Master registered yet.
8. Postconditions: Player has a Master name, a Servant, and is assigned
   to a war. Main Menu is displayed.
9. Notes/Issues: None


### Use Case U2: Manage Inventory

1. Objective: The player organises items between bag and equip slots
   to boost Servant stats.
2. Priority: High
3. Source: Jan Kowalski, End-user
4. Actors: Player
5. Flow of Events
  1. Basic Flow
    1. Player selects Inventory from the Main Menu.
    2. System displays the inventory grid and Servant panel.
    3. Player uses WASD to move the cursor to an item.
    4. Player presses T to equip the item to an equip slot.
    5. System moves item and recalculates effective stats.
    6. System updates STATUS panel with new values and green bonus.
  2. Alternative Flow 1 - Player drags and drops an item
    1. Player presses G to grab item at cursor.
    2. Player moves cursor to target slot.
    3. Player presses G again to drop.
    4. System places item and updates display.
  3. Alternative Flow 2 - Player sells an item
    1. Player presses H on an item.
    2. System displays sell confirmation with price (80-90% of base).
    3. Player presses Y to confirm.
    4. System removes item and adds gold to treasury.
  4. Exception Flow 1 - Player equips to occupied slot
    1. System displays red error message.
    2. Returns to step 5.1.3.
6. Includes: None
7. Preconditions: Player is registered. Main Menu is displayed.
8. Postconditions: Items organised. Servant effective stats updated.
9. Notes/Issues: Only equip slot items (columns 4-5) affect stats.


### Use Case U3: Buy Item from Shop

1. Objective: The player purchases an item and places it in inventory.
2. Priority: High
3. Source: Jan Kowalski, End-user
4. Actors: Player
5. Flow of Events
  1. Basic Flow
    1. Player selects Shop from the Main Menu.
    2. System displays the shop item list with prices and filters.
    3. Player navigates with W/S to select an item.
    4. Player presses B to buy.
    5. System checks player has enough gold and an empty bag slot.
    6. System deducts gold and opens the item placement menu.
    7. Player selects a bag slot and presses Enter.
    8. System places item in the selected slot.
  2. Alternative Flow 1 - Insufficient gold
    1. System displays red error message.
    2. Returns to step 5.1.3.
  3. Alternative Flow 2 - Inventory full
    1. System displays red error message.
    2. Returns to step 5.1.3.
  4. Alternative Flow 3 - Player cancels placement (Q)
    1. System refunds gold and returns item to shop.
6. Includes: None
7. Preconditions: Player is registered and has gold. Shop is open.
8. Postconditions: Item is in player inventory. Gold deducted.
9. Notes/Issues: None


### Use Case U4: Fight in Battle Arena

1. Objective: The player fights an enemy Servant in turn-based combat
   and earns gold on victory.
2. Priority: High
3. Source: Jan Kowalski, End-user
4. Actors: Player
5. Flow of Events
    1. Basic Flow
        1. Player selects Battle from the Main Menu.
        2. System selects a random undefeated enemy Servant from the pool.
        3. System displays the battle screen with both Servant panels,
           HP bars, and action menu.
        4. Player presses A for a basic attack.
        5. System calculates hit chance based on AGI difference.
        6. If hit, system calculates damage: player STR minus half enemy DUR.
        7. Enemy takes damage. HP bar updates.
        8. Enemy takes their turn (basic attack or Noble Phantasm).
        9. Player takes damage if hit. HP bar updates.
        10. Steps 4-9 repeat until one side reaches 0 HP.
        11. If enemy reaches 0 HP, system displays victory screen,
            awards gold, and marks enemy as defeated.
        12. System returns to battle lobby.
    2. Alternative Flow 1 - Player uses Noble Phantasm
        1. Player presses 1-9 to select an available NP.
        2. System displays NP banner with name, damage, and description.
        3. System calculates hit chance (higher base than basic attack).
        4. If hit, system calculates scaled NP damage.
        5. NP is marked as used for this battle.
        6. Continue from Basic Flow step 8.
    3. Alternative Flow 2 - Player flees
        1. Player presses E.
        2. System displays flee message and returns to battle lobby.
        3. No life is lost.
    4. Exception Flow 1 - Player HP reaches 0
        1. System decrements lives counter.
        2. System displays defeat screen showing remaining lives.
        3. If lives > 0, system returns to battle lobby.
        4. If lives == 0, system displays game over screen and exits.
6. Includes: None
7. Preconditions: Player is registered. Enemies remain in current war.
8. Postconditions: Enemy is defeated and marked in gallery. Gold awarded.
   Or player lost a life and returned to lobby. Or game ended.
9. Notes/Issues: Noble Phantasms reset between battles but not within
   a single battle.


### Use Case U5: View Gallery

1. Objective: The player views the full roster of both Holy Grail Wars
   with colour-coded status for each Servant.
2. Priority: Medium
3. Source: Jan Kowalski, End-user
4. Actors: Player
5. Flow of Events
    1. Basic Flow
        1. Player selects Gallery from the Main Menu.
        2. System displays two columns side by side:
           left column for Fate/stay night,
           right column for Fate/Zero.
        3. Each row shows Servant name and Master name.
        4. Colour coding applied:
            - Cyan [YOU]: current active Servant
            - Green star [WINNER]: Servant used to clear that war
            - Red [FALLEN]: enemy defeated by player
            - Gold: Servant still standing
        5. Player presses any key to return to Main Menu.
    2. Alternative Flow 1 - No battles fought yet
        1. All Servants shown in gold (none fallen, no winner).
6. Includes: None
7. Preconditions: Player is registered. Main Menu is displayed.
8. Postconditions: Gallery viewed. Player returned to Main Menu.
9. Notes/Issues: Master names are context-aware. Player rows show the
   typed Master name. Winner rows show the name used during that war.
   All other rows show canon lore Master name.


### Use Case U6: Progress to Second War

1. Objective: After clearing all enemies in the first war the player
   is transitioned to the second Holy Grail War with a new Master name,
   new Servant, reset inventory, and reset gold.
2. Priority: High
3. Source: Jan Kowalski, End-user
4. Actors: Player
5. Flow of Events
    1. Basic Flow
        1. Player defeats the last remaining enemy in the current war.
        2. System marks the war as cleared and records the winner Servant.
        3. System displays a war-cleared announcement screen.
        4. Player presses any key to accept the transition.
        5. System resets inventory to a new random starter set.
        6. System resets gold to 0.
        7. System assigns a random fallback Servant from the new war.
        8. System displays the New Master Registration screen.
        9. Player types a new Master name (or presses Esc to keep random).
        10. System assigns the correct Servant for that name in the new war.
        11. System displays the Servant Profile screen.
        12. System returns to the Main Menu now in the new war.
    2. Alternative Flow 1 - Both wars cleared
        1. System displays a final victory screen.
        2. System exits the game.
    3. Alternative Flow 2 - Player types unknown name at step 5.1.9
        1. System keeps the random fallback Servant.
        2. Continue from step 5.1.11.
6. Includes: U1 (registration logic reused in step 5.1.9)
7. Preconditions: All enemies in the current war have been defeated.
8. Postconditions: Player is in the second war with new Servant, empty
   inventory, and zero gold. Lives carry over unchanged.
9. Notes/Issues: None

---

## 5. Other Nonfunctional Requirements

### 5.1 Performance Requirements

- The system shall render each screen and respond to a keypress within
  100 milliseconds on any machine meeting the operating environment
  requirements described in section 2.4.
- The system shall generate a full random item set for the shop in under
  50 milliseconds.
- The system shall complete a battle damage calculation and update the
  display within 50 milliseconds of a player keypress.
- No frame rate or refresh cycle is used. The display updates only on
  player input, so CPU usage at idle shall be effectively 0%.

### 5.2 Safety Requirements

- The game contains no real-world financial transactions, personal data
  collection, or network communication. There are no safety risks
  associated with the use of this product.
- The game uses std::exit(0) on a third-life-loss game over. This is
  acceptable as the process owns no persistent resources that require
  cleanup at that point.

### 5.3 Security Requirements

- The system stores no user data. No login, account, or authentication
  mechanism exists.
- No data is written to disk during or after a session. All state is
  in-memory only.
- The binary accepts no command-line arguments that could be exploited.
- No external input beyond keyboard keypresses is processed.

### 5.4 Software Quality Attributes

- Usability: All available actions must be visible on screen at all times
  via the control hint bar. A new player shall be able to navigate all
  screens without consulting external documentation.
- Reliability: The game shall not crash or produce undefined behaviour
  during normal play on any supported platform.
- Maintainability: All Servant definitions are isolated in servant.cpp.
  Adding a new Servant requires only adding a factory function and
  registering it in allServants(). No changes to game logic are needed.
- Portability: The codebase shall compile without modification on Linux,
  macOS, and Windows (with Windows Terminal) using CMake and a C++17
  compiler.
- Correctness: Equipment bonuses shall be calculated as
  base stat x percent value / 100 and reflected accurately in both
  the STATUS panel and the battle damage formulas.

### 5.5 Business Rules

- A Servant can only be used once per Noble Phantasm per battle.
- A player can only equip items in columns 4-5 of the inventory grid.
- Sell price is always 80-90% of base item price, randomised per sale.
- Lives carry over between wars. Inventory and gold do not.
- Kirei Kotomine as a Master name always results in a random war
  assignment by design.

---

## 6. Other Requirements

None beyond what is described in sections 1-5.

---

## 7. System Requirements Chart

ID   | Priority | Type | Source              | Use Case | Description
-----|----------|------|---------------------|----------|--------------------------------------------------
R01  | High     | F    | Jan Kowalski, User  | U1       | The system shall match typed Master names to the correct Servant and Holy Grail War.
R02  | High     | F    | Jan Kowalski, User  | U1       | The system shall assign a random Servant when an unknown name is entered or Esc is pressed.
R03  | High     | F    | Jan Kowalski, User  | U1       | The system shall display the Servant profile with ASCII art, stats, and Noble Phantasms after summoning.
R04  | High     | F    | Jan Kowalski, User  | U2       | The system shall allow items to be moved between bag and equip slots using keyboard input.
R05  | High     | F    | Jan Kowalski, User  | U2       | The system shall apply equipment bonuses to Servant stats based on item type and percent value.
R06  | High     | F    | Jan Kowalski, User  | U2       | The system shall display the stat bonus to the right of each rank bar in the STATUS panel.
R07  | High     | F    | Jan Kowalski, User  | U3       | The system shall allow the player to buy items from the shop if they have sufficient gold.
R08  | Medium   | F    | Jan Kowalski, User  | U3       | The system shall allow filtering shop items by rarity and type.
R09  | Medium   | F    | Jan Kowalski, User  | U3       | The system shall allow searching shop items by name.
R10  | High     | F    | Jan Kowalski, User  | U2,U3    | The system shall allow the player to sell items for 80-90% of base price.
R11  | High     | F    | Jan Kowalski, User  | U4       | The system shall conduct turn-based combat between the player Servant and a random enemy Servant.
R12  | High     | F    | Jan Kowalski, User  | U4       | The system shall award gold to the player on defeating an enemy using the formula: 50 + STR x 2 + DUR + MaxHP / 10.
R13  | High     | F    | Jan Kowalski, User  | U4       | The system shall decrement the player life count on defeat and end the game on the third loss.
R14  | High     | F    | Jan Kowalski, User  | U4       | The system shall allow the player to use Noble Phantasms once per battle per NP.
R15  | Medium   | F    | Jan Kowalski, User  | U5       | The system shall display all Servants from both wars side by side in the gallery.
R16  | Medium   | F    | Jan Kowalski, User  | U5       | The system shall colour-code each Servant in the gallery as current (cyan), winner (green), fallen (red), or alive (gold).
R17  | High     | F    | Jan Kowalski, User  | U6       | The system shall transition the player to the second Holy Grail War after clearing the first.
R18  | High     | F    | Jan Kowalski, User  | U6       | The system shall reset inventory and gold when transitioning between wars.
R19  | High     | F    | Jan Kowalski, User  | U6       | The system shall carry over lives when transitioning between wars.
R20  | High     | NF   | Jan Kowalski, User  | NA       | The system shall respond to all keypresses and update the display within 100 milliseconds.
R21  | High     | NF   | Jan Kowalski, User  | NA       | The system shall compile and run without modification on Linux, macOS, and Windows Terminal.
R22  | Medium   | NF   | Jan Kowalski, User  | NA       | The system shall display all available controls on screen at all times via a hint bar.
R23  | Medium   | NF   | Jan Kowalski, User  | NA       | The system shall not crash or produce undefined behaviour during normal play.
R24  | Low      | NF   | Jan Kowalski, User  | NA       | Adding a new Servant shall require changes only in servant.cpp with no modifications to game logic.

---

## Appendix A: Glossary

Master
A player or character who summons and commands a Servant in the
Holy Grail War. In this game, the player takes the role of a Master.

Servant
A heroic spirit summoned to fight in the Holy Grail War. Each Servant
has stats (HP, STR, DUR, AGI) and Noble Phantasms.

Noble Phantasm (NP)
A special attack unique to each Servant. Can be used once per battle.
Has a base damage value and a damage scale multiplier.

Holy Grail War
A tournament-style battle between seven Masters and their Servants.
The game features two: the Fifth War (Fate/stay night) and the
Fourth War (Fate/Zero).

Bag Slot
Inventory columns 1-3. Items stored here have no effect on stats.

Equip Slot
Inventory columns 4-5. Items placed here boost Servant stats based
on item type and percent value.

Percent Value
A property of each item representing the percentage bonus it applies
to a base stat when equipped. Formula: base stat x percent / 100.

ANSI Escape Codes
Special character sequences interpreted by terminal emulators to
produce coloured and formatted text output.

UTF-8
A variable-width character encoding used to render box-drawing
characters and special symbols in the terminal UI.

Series
An enum in the codebase with values StayNight and Zero, used to
associate Servants and enemies with the correct Holy Grail War.

---

## Appendix B: Analysis Models

Not included in this SRS. Class diagrams, state-transition diagrams,
and data flow diagrams will be produced during the high-level design
deliverable.

---

## Appendix C: To Be Determined List

TBD-01: Save/load system. It is currently undecided whether a future
version will support saving game state between sessions.

TBD-02: Difficulty modes. A hard mode with stronger enemy stats or
fewer lives has been discussed but not specified.

TBD-03: Windows raw input. The singleCharacter.h implementation for
Windows raw keypress input needs cross-platform verification
before the Windows release can be confirmed as fully supported.

TBD-04: Additional Servants. Expanding the roster beyond the current
16 Servants (9 FSN + 7 Zero) has not been scoped or scheduled.