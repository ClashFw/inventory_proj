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
    int         baseDamage;
    float       damageScale;
};

class Servant {
private:
    std::string name;
    std::string masterName;       // canon master name
    std::string description;      // short servant lore blurb
    Series      series;

    int maxHP;
    int currentHP;
    int strength;
    int durability;
    int agility;

    std::vector<NoblePhantasm> noblePhantasms;
    std::vector<bool>          npUsedThisBattle;

    std::string asciiArt;         // faces right (player side)
    std::string mirroredArt;      // faces left  (enemy side)

public:
    Servant();
    Servant(const std::string& name,
            const std::string& masterName,
            const std::string& description,
            Series series,
            int maxHP,
            int strength,
            int durability,
            int agility,
            const std::vector<NoblePhantasm>& nps,
            const std::string& art,
            const std::string& mirroredArt);

    const std::string& getName()        const { return name; }
    const std::string& getMasterName()  const { return masterName; }
    const std::string& getDescription() const { return description; }
    Series             getSeries()      const { return series; }
    int                getMaxHP()       const { return maxHP; }
    int                getCurrentHP()   const { return currentHP; }
    int                getStrength()    const { return strength; }
    int                getDurability()  const { return durability; }
    int                getAgility()     const { return agility; }
    const std::string& getAsciiArt()    const { return asciiArt; }
    const std::string& getMirroredArt() const { return mirroredArt; }

    void setCurrentHP(int hp);
    bool isAlive() const { return currentHP > 0; }

    const std::vector<NoblePhantasm>& getNPs() const { return noblePhantasms; }
    bool isNPAvailable(std::size_t idx) const;
    void resetForNewBattle();

    int  basicAttackDamage(int attackerStrBonus, int defenderDurEff) const;
    int  noblePhantasmDamage(std::size_t idx,
                             int attackerStrBonus,
                             int defenderDurEff) const;
    void markNPUsed(std::size_t idx);

    void renderAscii() const;

    // Returns servant whose masterName matches (case-insensitive); or random
    static Servant selectServantForMaster(const std::string& masterInput, Series s);
    static Servant randomServantForSeries(Series s);
    static Servant randomServant();
    static std::vector<Servant> allServants();
    static std::vector<Servant> enemyPoolForSeries(Series s);
};

#endif // SERVANT_H
