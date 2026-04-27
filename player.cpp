#include "player.h"
#include <string>

static void computeEquipmentBonuses(const Inventory* inv,
                                    const Servant*   sv,
                                    int& hpBonus,
                                    int& strBonus,
                                    int& durBonus,
                                    int& agiBonus)
{
    hpBonus = strBonus = durBonus = agiBonus = 0;
    if (!inv || !sv) return;

    int baseHP  = sv->getMaxHP();
    int baseSTR = sv->getStrength();
    int baseDUR = sv->getDurability();
    int baseAGI = sv->getAgility();

    int rows = inv->getRows();
    int cols = inv->getCols();

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (c < 3) continue;            // bag cols — skip
            Item* it = inv->getItemAt(r, c);
            if (!it) continue;
            int pct = it->getPercentValue();
            if (pct <= 0) continue;
            switch (it->getType()) {
                case potion:   hpBonus  += baseHP  * pct / 100; break;
                case sword:    strBonus += baseSTR * pct / 100; break;
                case armor:    durBonus += baseDUR * pct / 100; break;
                case movement: agiBonus += baseAGI * pct / 100; break;
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
    *servant = s;
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