#ifndef SERVANT_H
#define SERVANT_H

#include <string>
#include <vector>

enum class Series {
    StayNight,
    Zero
};

struct NoblePhantasm {
    std::string name;
    std::string description;
    int baseDamage;      // flat damage component
    float damageScale;   // multiplier on Strength
};

class Servant {
private:
    std::string name;
    Series series;

    int maxHP;
    int currentHP;
    int strength;
    int durability;  // base damage reduction
    int agility;     // hit / evade modifier

    std::vector<NoblePhantasm> noblePhantasms;
    std::vector<bool> npUsedThisBattle;

    std::string asciiArt;

public:
    Servant();
    Servant(const std::string& name,
            Series series,
            int maxHP,
            int strength,
            int durability,
            int agility,
            const std::vector<NoblePhantasm>& nps,
            const std::string& art);

    const std::string& getName()      const { return name; }
    Series             getSeries()    const { return series; }
    int                getMaxHP()     const { return maxHP; }
    int                getCurrentHP() const { return currentHP; }
    int                getStrength()  const { return strength; }
    int                getDurability()const { return durability; }
    int                getAgility()   const { return agility; }

    void setCurrentHP(int hp);
    bool isAlive() const { return currentHP > 0; }

    const std::vector<NoblePhantasm>& getNPs() const { return noblePhantasms; }
    bool isNPAvailable(size_t idx) const;
    void resetForNewBattle();

    // Damage helpers; defenderDurEff is the defender's effective durability
    int basicAttackDamage(int attackerStrBonus, int defenderDurEff) const;
    int noblePhantasmDamage(size_t idx,
                            int attackerStrBonus,
                            int defenderDurEff) const;
    void markNPUsed(size_t idx);

    void renderAscii() const;

    // Factory helpers
    static Servant randomServant();
    static std::vector<Servant> enemyPoolForSeries(Series s);
};

#endif // SERVANT_H
