#include "servant.h"
#include <iostream>
#include <cstdlib>

// ─────────────────────────────────────────────
// Constructors
// ─────────────────────────────────────────────

Servant::Servant()
    : name("Unnamed"),
      series(Series::StayNight),
      maxHP(100),
      currentHP(100),
      strength(10),
      durability(5),
      agility(10),
      noblePhantasms(),
      npUsedThisBattle(),
      asciiArt("[Servant]")
{}

Servant::Servant(const std::string& name,
                 Series series,
                 int maxHP,
                 int strength,
                 int durability,
                 int agility,
                 const std::vector<NoblePhantasm>& nps,
                 const std::string& art)
    : name(name),
      series(series),
      maxHP(maxHP),
      currentHP(maxHP),
      strength(strength),
      durability(durability),
      agility(agility),
      noblePhantasms(nps),
      npUsedThisBattle(nps.size(), false),
      asciiArt(art)
{}

// ─────────────────────────────────────────────
// State / battle helpers
// ─────────────────────────────────────────────

void Servant::setCurrentHP(int hp) {
    if (hp < 0) hp = 0;
    if (hp > maxHP) hp = maxHP;
    currentHP = hp;
}

bool Servant::isNPAvailable(size_t idx) const {
    return idx < npUsedThisBattle.size() && !npUsedThisBattle[idx];
}

void Servant::resetForNewBattle() {
    currentHP = maxHP;
    for (std::size_t i = 0; i < npUsedThisBattle.size(); ++i) {
        npUsedThisBattle[i] = false;
    }
}


int Servant::basicAttackDamage(int attackerStrBonus, int defenderDurEff) const {
    int raw = strength + attackerStrBonus;
    int reduction = defenderDurEff;
    int dmg = raw - reduction;
    if (dmg < 1) dmg = 1;
    return dmg;
}

int Servant::noblePhantasmDamage(size_t idx,
                                 int attackerStrBonus,
                                 int defenderDurEff) const {
    if (idx >= noblePhantasms.size()) return 0;
    const NoblePhantasm& np = noblePhantasms[idx];
    int raw = np.baseDamage +
              static_cast<int>((strength + attackerStrBonus) * np.damageScale);
    int reduction = defenderDurEff;
    int dmg = raw - reduction;
    if (dmg < 1) dmg = 1;
    return dmg;
}

void Servant::markNPUsed(size_t idx) {
    if (idx < npUsedThisBattle.size()) npUsedThisBattle[idx] = true;
}

void Servant::renderAscii() const {
    std::cout << asciiArt << std::endl;
}

// ─────────────────────────────────────────────
// Example servants and factories
// (expand with real Fate characters later)
// ─────────────────────────────────────────────

static Servant makeArtoriaStayNight() {
    std::vector<NoblePhantasm> nps = {
        { "Excalibur", "Massive anti-fortress beam.", 40, 2.0f },
        { "Invisible Air", "Wind-cloaked sword strike.", 20, 1.2f }
    };
    std::string art =
        "  [SABER]\n"
        "    /\\\n"
        "   /__\\\n"
        "   |  |\n"
        "   |  |\n";
    return Servant("Artoria Pendragon", Series::StayNight,
                   140, 25, 15, 18, nps, art);
}

// TODO: add more servants from Stay Night and Zero.

Servant Servant::randomServant() {
    std::vector<Servant> all;
    all.push_back(makeArtoriaStayNight());
    // push_back more servants here

    if (all.empty()) return Servant();
    int idx = std::rand() % all.size();
    return all[idx];
}

std::vector<Servant> Servant::enemyPoolForSeries(Series s) {
    std::vector<Servant> pool;
    Servant a = makeArtoriaStayNight();
    if (a.getSeries() == s) pool.push_back(a);

    // Add other servants of the same series here

    return pool;
}
