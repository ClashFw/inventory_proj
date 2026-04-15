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

bool Servant::isNPAvailable(std::size_t idx) const {
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

int Servant::noblePhantasmDamage(std::size_t idx,
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

void Servant::markNPUsed(std::size_t idx) {
    if (idx < npUsedThisBattle.size()) npUsedThisBattle[idx] = true;
}

void Servant::renderAscii() const {
    std::cout << asciiArt << std::endl;
}

// ─────────────────────────────────────────────
// Individual servant factories
// (ASCII art: taller and closer to references)
// ─────────────────────────────────────────────

// ---- Fate/stay night ----

// Saber – Artoria Pendragon (blue armor dress)
static Servant makeSaberArtoriaStayNight() {
    std::vector<NoblePhantasm> nps = {
        { "Excalibur", "Holy sword beam that obliterates foes.", 50, 2.3f },
        { "Invisible Air", "Wind-cloaked sword and boosted speed.", 20, 1.3f }
    };
    std::string art = R"(
           ________
          /  __   \
         /  /  \   \
        /__/ /\ \___\
          / /  \ \
         /_/====\_\
        /  |[][]|  \
       /   | || |   \
      /____|_||_|____\
          /  ||  \
         /   ||   \
        /    ||    \
       /____/  \____\
          /      \
         /________\
)";
    return Servant("Saber (Artoria)", Series::StayNight,
                   160, 26, 20, 18, nps, art);
}

// Archer – Emiya
static Servant makeArcherEmiya() {
    std::vector<NoblePhantasm> nps = {
        { "Unlimited Blade Works", "Reality marble of countless swords.", 40, 2.0f },
        { "Kanshou & Bakuya", "Twin blade combination assault.", 28, 1.5f }
    };
    std::string art = R"(
          ________
         / ____  \
        / / __|  |
       / / /  |  |
      /_/ /   |  |
      |__/ [] |  |
      |  \____|  |
      |  /||||\  |
      | /_||||_\ |
      |   ||||   |
      |   ||||   |
      |___||||___|
          /  \
         /____\
)";
    return Servant("Archer (Emiya)", Series::StayNight,
                   145, 24, 17, 23, nps, art);
}

// Lancer – Cú Chulainn (blue armor, red spear)
static Servant makeLancerCu() {
    std::vector<NoblePhantasm> nps = {
        { "Gae Bolg", "Cursed spear that strikes the heart.", 42, 1.9f },
        { "Soaring Spear", "Long-range spear throw.", 30, 1.4f }
    };
    std::string art = R"(
             /\
            /  \
       ____/ /\ \____
      /  _  /  \  _  \
     /__/ \/ /\ \/ \__\
         /  /  \  \
        /__/____\__\
          / || \
         /  ||  \
        /   ||   \
   ======   ||   =====>
)";
    return Servant("Lancer (Cu Chulainn)", Series::StayNight,
                   150, 23, 16, 25, nps, art);
}

// Rider – Medusa (blindfold, long hair, black dress)
static Servant makeRiderMedusa() {
    std::vector<NoblePhantasm> nps = {
        { "Bellerophon", "Pegasus charge that tramples the field.", 45, 1.8f },
        { "Blood Fort Andromeda", "Barrier that drains life.", 25, 1.2f }
    };
    std::string art = R"(
          ~~~~~~~~~~~~~
        ~~  ////////  ~~
       ~~  /  ----  \  ~
      ~~  /  |----|  \ ~
          |  | [] |  |
          |  |____|  |
          |  /||||\  |
          | / |||| \ |
          |/  ||||  \|
          /   ||||   \
         /    ||||    \
        /____/||||\____\
             / || \
            /  ||  \
)";
    return Servant("Rider (Medusa)", Series::StayNight,
                   140, 22, 15, 24, nps, art);
}

// Berserker – Heracles (huge body, bracers, skirt armor)
static Servant makeBerserkerHeracles() {
    std::vector<NoblePhantasm> nps = {
        { "Nine Lives", "Multi-hit onslaught of overwhelming force.", 55, 2.1f }
    };
    std::string art = R"(
            _________
           /  ___   \
          /  / _ \   \
         /__/ |_| \___\
         |  / ___ \  |
         | | |   | | |
         | | |   | | |
         | | |___| | |
         | |  ___  | |
         | | |   | | |
         | | |   | | |
         | | |___| | |
         |_|_/   \_|_|
           /_/   \_\
)";
    return Servant("Berserker (Heracles)", Series::StayNight,
                   210, 30, 22, 13, nps, art);
}

// Archer – Gilgamesh (casual FSN outfit)
static Servant makeArcherGilgameshStayNight() {
    std::vector<NoblePhantasm> nps = {
        { "Gate of Babylon", "Endless barrage of treasures.", 48, 2.0f },
        { "Enuma Elish", "Sword of rupture that tears space.", 60, 2.5f }
    };
    std::string art = R"(
          _________
         /  ___  _\
        /  /  / / /
       /__/__/ / /
       |  ____/ /
       | |  __ /
       | | |  |
       | | |[]|
       | | |__|
       | |  ||
       |_|  ||____
         \__||   /
            /__/
)";
    return Servant("Archer (Gilgamesh)", Series::StayNight,
                   155, 27, 18, 20, nps, art);
}

// Assassin – Sasaki Kojirou (purple robe, long sword)
static Servant makeAssassinSasaki() {
    std::vector<NoblePhantasm> nps = {
        { "Tsubame Gaeshi", "Triple strike that ignores space.", 38, 1.8f }
    };
    std::string art = R"(
          _________
         /  ____  \
        /  /   \  \
       /__/     \__\
       |  | --- |  |
       |  |  |  |  |
       |  |  |  |  |
       |  |__|  |  |
       |  /  \  |  |
       | /____\ |  |
       |/      \|  |
        \      /===>
         \____/
)";
    return Servant("Assassin (Sasaki)", Series::StayNight,
                   135, 22, 14, 26, nps, art);
}

// Caster – Medea (hood, layered robe)
static Servant makeCasterMedea() {
    std::vector<NoblePhantasm> nps = {
        { "Rule Breaker", "Dagger that severs contracts.", 24, 1.3f },
        { "Age of Gods Magecraft", "High ritual magic.", 40, 1.6f }
    };
    std::string art = R"(
           _______
          /  /\  \
         /__/  \__\
         |  ____  |
         | / __ \ |
         | | || | |
         | | || | |
         | | || | |
         | | || | |
         | | || | |
         | |____| |
         |/______\|
)";
    return Servant("Caster (Medea)", Series::StayNight,
                   135, 20, 15, 18, nps, art);
}

// Assassin – Hassan-i-Sabbah (True Assassin: skull mask, lean body)
static Servant makeAssassinHassanStayNight() {
    std::vector<NoblePhantasm> nps = {
        { "Zabaniya", "Heart-crushing assassination.", 34, 1.7f }
    };
    std::string art = R"(
           ______
          / ____ \
         / / __ \ \
        / / /  \ \ \
        | | |  | | |
        | | |  | | |
        | | |__| | |
        | |  __  | |
        | | /  \ | |
        |_|/____\|_|
          /_/  \_\
)";
    return Servant("Assassin (Hassan)", Series::StayNight,
                   135, 21, 15, 24, nps, art);
}

// ---- Fate/Zero ----

// Saber – Artoria (Zero armor; similar but slightly heavier)
static Servant makeSaberArtoriaZero() {
    std::vector<NoblePhantasm> nps = {
        { "Excalibur", "Holy sword beam that obliterates foes.", 50, 2.3f },
        { "Invisible Air", "Wind cloak around the blade.", 20, 1.3f }
    };
    std::string art = R"(
           _________
          /   __   \
         /   /  \   \
        /___/ /\ \___\
        |   / /  \   |
        |  / /==\ \  |
        | /_/ || \_\ |
        |/__  ||  __\|
           /  ||  \
          /   ||   \
         /    ||    \
        /____/  \____\
)";
    return Servant("Saber (Artoria Zero)", Series::Zero,
                   165, 27, 21, 19, nps, art);
}

// Archer – Gilgamesh (Zero golden armor)
static Servant makeArcherGilgameshZero() {
    std::vector<NoblePhantasm> nps = {
        { "Gate of Babylon", "Endless barrage of treasures.", 50, 2.1f },
        { "Enuma Elish", "Sword of rupture that tears worlds.", 65, 2.6f }
    };
    std::string art = R"(
           _________
          /__/^^\__\
         /  /====\  \
        /__/  []  \__\
        |  /| || |\  |
        | / | || | \ |
        |/  | || |  \|
        |   | || |   |
        |   | || |   |
        |___|_||_|___|
)";
    return Servant("Archer (Gilgamesh Zero)", Series::Zero,
                   160, 29, 19, 21, nps, art);
}

// Rider – Iskandar (big cloak, broad build)
static Servant makeRiderIskandar() {
    std::vector<NoblePhantasm> nps = {
        { "Ionioi Hetairoi", "Army of loyal companions charges.", 55, 2.0f }
    };
    std::string art = R"(
           _________
          /  ____  \
         /  / __ \  \
        /__/ /  \ \__\
        |  / ____ \  |
        | / / __\ \ |
        |/ / /  \ \\|
        /_/ /____\ \_\
          /  ||  \
         /   ||   \
        /____||____\
)";
    return Servant("Rider (Iskandar)", Series::Zero,
                   190, 28, 21, 14, nps, art);
}

// Caster – Gilles de Rais (heavy robe, hood)
static Servant makeCasterGilles() {
    std::vector<NoblePhantasm> nps = {
        { "Prelati's Spellbook", "Sea of eldritch creatures.", 45, 1.8f }
    };
    std::string art = R"(
           _________
          /  /\   /\
         /__/  \_/  \
         |  ______  |
         | / ____ \ |
         | |/ __ \| |
         |  /____\  |
         | /::::::\ |
         |/::::::: \|
         /_/:::::\__\
)";
    return Servant("Caster (Gilles)", Series::Zero,
                   145, 21, 16, 17, nps, art);
}

// Lancer – Diarmuid Ua Duibhne (dual spears, light armor)
static Servant makeLancerDiarmuid() {
    std::vector<NoblePhantasm> nps = {
        { "Gae Dearg & Gae Buidhe", "Twin cursed spears.", 40, 1.9f }
    };
    std::string art = R"(
       <=====      =====>
          /\      /\
         /  \____/  \
        /  /|    |\  \
       /__/ | /\ | \__\
           / /  \ \
          /_/____\_\
           / || \
          /  ||  \
         /   ||   \
)";
    return Servant("Lancer (Diarmuid)", Series::Zero,
                   150, 24, 17, 24, nps, art);
}

// Berserker – Lancelot (black armor)
static Servant makeBerserkerLancelot() {
    std::vector<NoblePhantasm> nps = {
        { "Knight of Owner", "Any weapon becomes a Noble Phantasm.", 46, 2.0f }
    };
    std::string art = R"(
           _________
          /  _____  \
         /  / _ _\  \
        /__/ /_|_\__\
        |  / /_|_\  |
        | | /_|_\ | |
        | |  |_|  | |
        | |  |_|  | |
        | |  |_|  | |
        |_|__|_|__|_|
          /_/   \_\
)";
    return Servant("Berserker (Lancelot)", Series::Zero,
                   185, 27, 20, 18, nps, art);
}

// Assassin – Hassan-i-Sabbah (Hundred Faces – hooded)
static Servant makeAssassinHassanZero() {
    std::vector<NoblePhantasm> nps = {
        { "Zabaniya (Hundred Faces)", "Assault from countless bodies.", 36, 1.8f }
    };
    std::string art = R"(
          _________
         /  _____  \
        /  /   /  \ \
       /__/___/____\_\
       |  /  /    | |
       | /__/     | |
       | |  ____  | |
       | | / __ \ | |
       | |/ /  \ \| |
       |_/ /____\ \_|
         /_/  \_\
)";
    return Servant("Assassin (Hassan Zero)", Series::Zero,
                   140, 22, 15, 25, nps, art);
}

// ─────────────────────────────────────────────
// Factories
// ─────────────────────────────────────────────

Servant Servant::randomServant() {
    std::vector<Servant> all;

    // Fate/stay night
    all.push_back(makeSaberArtoriaStayNight());
    all.push_back(makeArcherEmiya());
    all.push_back(makeLancerCu());
    all.push_back(makeRiderMedusa());
    all.push_back(makeBerserkerHeracles());
    all.push_back(makeArcherGilgameshStayNight());
    all.push_back(makeAssassinSasaki());
    all.push_back(makeCasterMedea());
    all.push_back(makeAssassinHassanStayNight());

    // Fate/Zero
    all.push_back(makeSaberArtoriaZero());
    all.push_back(makeArcherGilgameshZero());
    all.push_back(makeRiderIskandar());
    all.push_back(makeCasterGilles());
    all.push_back(makeLancerDiarmuid());
    all.push_back(makeBerserkerLancelot());
    all.push_back(makeAssassinHassanZero());

    if (all.empty()) return Servant();
    int idx = std::rand() % all.size();
    return all[idx];
}

std::vector<Servant> Servant::enemyPoolForSeries(Series s) {
    std::vector<Servant> pool;

    // stay night
    Servant sSaberSN   = makeSaberArtoriaStayNight();
    Servant sArcherE   = makeArcherEmiya();
    Servant sLancerCu  = makeLancerCu();
    Servant sRiderMed  = makeRiderMedusa();
    Servant sBersHer   = makeBerserkerHeracles();
    Servant sArcherGil = makeArcherGilgameshStayNight();
    Servant sAssSasa   = makeAssassinSasaki();
    Servant sCasterMed = makeCasterMedea();
    Servant sAssHasSN  = makeAssassinHassanStayNight();

    // zero
    Servant zSaber     = makeSaberArtoriaZero();
    Servant zArcherGil = makeArcherGilgameshZero();
    Servant zRider     = makeRiderIskandar();
    Servant zCaster    = makeCasterGilles();
    Servant zLancer    = makeLancerDiarmuid();
    Servant zBers      = makeBerserkerLancelot();
    Servant zAss       = makeAssassinHassanZero();

    if (sSaberSN.getSeries()   == s) pool.push_back(sSaberSN);
    if (sArcherE.getSeries()   == s) pool.push_back(sArcherE);
    if (sLancerCu.getSeries()  == s) pool.push_back(sLancerCu);
    if (sRiderMed.getSeries()  == s) pool.push_back(sRiderMed);
    if (sBersHer.getSeries()   == s) pool.push_back(sBersHer);
    if (sArcherGil.getSeries() == s) pool.push_back(sArcherGil);
    if (sAssSasa.getSeries()   == s) pool.push_back(sAssSasa);
    if (sCasterMed.getSeries() == s) pool.push_back(sCasterMed);
    if (sAssHasSN.getSeries()  == s) pool.push_back(sAssHasSN);

    if (zSaber.getSeries()     == s) pool.push_back(zSaber);
    if (zArcherGil.getSeries() == s) pool.push_back(zArcherGil);
    if (zRider.getSeries()     == s) pool.push_back(zRider);
    if (zCaster.getSeries()    == s) pool.push_back(zCaster);
    if (zLancer.getSeries()    == s) pool.push_back(zLancer);
    if (zBers.getSeries()      == s) pool.push_back(zBers);
    if (zAss.getSeries()       == s) pool.push_back(zAss);

    return pool;
}
