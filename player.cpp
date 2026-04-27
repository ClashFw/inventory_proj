#include "player.h"
#include <string>

// Compute equipment bonuses based on item type and percentValue.
// Bag: cols 0–2, Equipment: cols 3–4 in the 2x5 layout.
static void computeEquipmentBonuses(const Inventory* inv,
                                    const Servant*   sv,
                                    int& hpBonus,
                                    int& strBonus,
                                    int& durBonus,
                                    int& agiBonus)
{
    hpBonus  = 0;
    strBonus = 0;
    durBonus = 0;
    agiBonus = 0;

    if (!inv || !sv) return;

    int baseHP  = sv->getMaxHP();
    int baseSTR = sv->getStrength();
    int baseDUR = sv->getDurability();
    int baseAGI = sv->getAgility();

    int rows = inv->getRows();
    int cols = inv->getCols();

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            // Only equipment columns (3 and 4) affect stats
            if (c < 3) continue;

            Item* it = inv->getItemAt(r, c);
            if (!it) continue;

            int pct = it->getPercentValue();
            if (pct <= 0) continue;

            switch (it->getType()) {
                case potion:
                    // Potions in equip slots act as max HP boosters
                    hpBonus += baseHP * pct / 100;
                    break;
                case sword:
                    // Swords boost STR
                    strBonus += baseSTR * pct / 100;
                    break;
                case armor:
                    // Armor boosts DUR
                    durBonus += baseDUR * pct / 100;
                    break;
                case movement:
                    // Movement gear boosts AGI
                    agiBonus += baseAGI * pct / 100;
                    break;
            }
        }
    }
}

Player::Player()
    : inventory(new Inventory()),
      servant(new Servant())
{}

Player::~Player() {
    delete inventory;
    delete servant;
}

void Player::setServant(const Servant& s) {
    *servant = s;          // copy full template
    servant->resetForNewBattle();
}

int Player::getEffectiveStrength() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, servant, hpB, sB, dB, aB);
    return servant->getStrength() + sB;
}

int Player::getEffectiveDurability() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, servant, hpB, sB, dB, aB);
    return servant->getDurability() + dB;
}

int Player::getEffectiveAgility() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, servant, hpB, sB, dB, aB);
    return servant->getAgility() + aB;
}

int Player::getEffectiveMaxHP() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, servant, hpB, sB, dB, aB);
    return servant->getMaxHP() + hpB;
}