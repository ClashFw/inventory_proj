#include "player.h"
#include <string>

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

// Scan equipment cells and apply name-based bonuses.
// Bag: cols 0–2, Equipment: cols 3–4 in the new 2x5 layout.
static void computeEquipmentBonuses(const Inventory* inv,
                                    int& hpBonus,
                                    int& strBonus,
                                    int& durBonus,
                                    int& agiBonus) {
    hpBonus = strBonus = durBonus = agiBonus = 0;

    int rows = inv->getRows();
    int cols = inv->getCols();
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (c < 3) continue; // only equipment columns
            Item* it = inv->getItemAt(r, c);
            if (!it) continue;
            const std::string& name = it->getName();

            if (name.find("Potion") != std::string::npos) {
                hpBonus += 30;
            } else if (name.find("Sword") != std::string::npos) {
                strBonus += 5;
            } else if (name.find("Armor") != std::string::npos) {
                durBonus += 5;
            } else if (name.find("Boots") != std::string::npos) {
                agiBonus += 3;
            }
        }
    }
}

int Player::getEffectiveStrength() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, hpB, sB, dB, aB);
    return servant->getStrength() + sB;
}

int Player::getEffectiveDurability() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, hpB, sB, dB, aB);
    return servant->getDurability() + dB;
}

int Player::getEffectiveAgility() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, hpB, sB, dB, aB);
    return servant->getAgility() + aB;
}

int Player::getEffectiveMaxHP() const {
    int hpB, sB, dB, aB;
    computeEquipmentBonuses(inventory, hpB, sB, dB, aB);
    return servant->getMaxHP() + hpB;
}
