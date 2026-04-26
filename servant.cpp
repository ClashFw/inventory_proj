#include "servant.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

// ─────────────────────────────────────────────
// ANSI colors
// ─────────────────────────────────────────────

#define RESET          "\033[0m"

#define BLACK          "\033[30m"
#define RED            "\033[31m"
#define GREEN          "\033[32m"
#define YELLOW         "\033[33m"
#define BLUE           "\033[34m"
#define MAGENTA        "\033[35m"
#define CYAN           "\033[36m"
#define WHITE          "\033[37m"

#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"

// 256-color shades
#define GOLD           "\033[38;5;220m"
#define DARK_GOLD      "\033[38;5;178m"
#define PALE_GOLD      "\033[38;5;229m"
#define SILVER         "\033[38;5;250m"
#define STEEL          "\033[38;5;245m"
#define GUNMETAL       "\033[38;5;240m"
#define NAVY           "\033[38;5;18m"
#define ROYAL_BLUE     "\033[38;5;27m"
#define SKY_BLUE       "\033[38;5;39m"
#define CRIMSON        "\033[38;5;160m"
#define BLOOD_RED      "\033[38;5;124m"
#define MAROON         "\033[38;5;88m"
#define VIOLET         "\033[38;5;141m"
#define PURPLE         "\033[38;5;129m"
#define DEEP_PURPLE    "\033[38;5;54m"
#define LAVENDER       "\033[38;5;183m"
#define IVORY          "\033[38;5;230m"
#define TAN            "\033[38;5;180m"
#define BROWN          "\033[38;5;94m"
#define DARK_BROWN     "\033[38;5;52m"
#define ASH            "\033[38;5;248m"
#define BONE           "\033[38;5;252m"

// ─────────────────────────────────────────────
// Constructors
// ─────────────────────────────────────────────

Servant::Servant()
    : name("Unnamed"),
      masterName("Unknown"),
      description(""),
      series(Series::StayNight),
      maxHP(100),
      currentHP(100),
      strength(10),
      durability(5),
      agility(10),
      noblePhantasms(),
      npUsedThisBattle(),
      asciiArt("[Servant]"),
      mirroredArt("[Servant]")
{}

Servant::Servant(const std::string& name,
                 const std::string& masterName,
                 const std::string& description,
                 Series series,
                 int maxHP,
                 int strength,
                 int durability,
                 int agility,
                 const std::vector<NoblePhantasm>& nps,
                 const std::string& art,
                 const std::string& mirroredArt)
    : name(name),
      masterName(masterName),
      description(description),
      series(series),
      maxHP(maxHP),
      currentHP(maxHP),
      strength(strength),
      durability(durability),
      agility(agility),
      noblePhantasms(nps),
      npUsedThisBattle(nps.size(), false),
      asciiArt(art),
      mirroredArt(mirroredArt)
{}

// ─────────────────────────────────────────────
// Battle helpers
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
// Helpers
// ─────────────────────────────────────────────

static std::string mirrorAscii(const std::string& art) {
    std::string result;
    std::string line;
    for (char c : art) {
        if (c == '\n') {
            std::reverse(line.begin(), line.end());
            result += line;
            result += '\n';
            line.clear();
        } else {
            line += c;
        }
    }
    if (!line.empty()) {
        std::reverse(line.begin(), line.end());
        result += line;
    }
    return result;
}

// ─────────────────────────────────────────────
// Fate/stay night servants (Fifth War)
// ─────────────────────────────────────────────

static Servant makeSaberArtoriaStayNight() {
    std::vector<NoblePhantasm> nps = {
        { "Excalibur", "Holy sword beam that obliterates foes.", 50, 2.3f },
        { "Invisible Air", "Wind-cloaked sword strike.", 20, 1.3f }
    };

    std::string art =
        PALE_GOLD     "                  /\\\n"
        BRIGHT_YELLOW "                 /  \\\n"
        PALE_GOLD     "            ____/ /\\ \\____\n"
        BRIGHT_YELLOW "           / __  /  \\  __ \\\n"
        SILVER        "          /_/ /_/ /\\ \\_\\ \\_\\\n"
        BRIGHT_BLUE   "             /  / /==\\ \\  \\\n"
        SILVER        "            /  / /" NAVY "[] " SILVER "\\ \\  \\\n"
        NAVY          "           /  / / " SILVER "||  " NAVY "\\ \\  \\\n"
        SILVER        "          /__/ /__" NAVY "||" SILVER "__\\ \\__\\\n"
        BRIGHT_BLUE   "             / /   ||   \\ \\\n"
        BRIGHT_BLUE   "            / /    ||    \\ \\\n"
        SILVER        "           /_/_____/\\_____\\_\\\n"
        GOLD          "                " BRIGHT_YELLOW "||\n"
        GOLD          "                " BRIGHT_YELLOW "||\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Shirou Emiya → Saber (Artoria) in FSN
    return Servant(
        "Saber (Artoria)",
        "Shirou Emiya",
        "Knightly king wielding the holy sword Excalibur.",
        Series::StayNight,
        160, 26, 20, 18, nps, art, mirrored
    );
}

static Servant makeArcherEmiya() {
    std::vector<NoblePhantasm> nps = {
        { "Unlimited Blade Works", "Reality marble of countless swords.", 40, 2.0f },
        { "Kanshou & Bakuya", "Twin blade combination assault.", 28, 1.5f }
    };

    std::string art =
        BRIGHT_WHITE "                /^^^^\\\n"
        BRIGHT_WHITE "               / " CRIMSON "__" BRIGHT_WHITE "  \\\n"
        CRIMSON      "              / /" BRIGHT_WHITE "[]" CRIMSON "\\  \\\n"
        BRIGHT_RED   "             / /____\\  \\\n"
        CRIMSON      "            / /|    |\\  \\\n"
        BLACK        "           / / |" CRIMSON "====" BLACK "| \\  \\\n"
        CRIMSON      "          /_/  | " SILVER "|| " CRIMSON "|  \\__\\\n"
        BLACK        "          | |  | " SILVER "|| " BLACK "|  | |\n"
        BRIGHT_RED   "          | |  | " SILVER "|| " BRIGHT_RED "|  | |\n"
        BLACK        "          | |  |_" SILVER "||" BLACK "_|  | |\n"
        SILVER       "          |_| /_/  \\_\\ |_| |\n"
        BRIGHT_RED   "             /_/    \\_\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Rin Tohsaka → Archer (EMIYA)
    return Servant(
        "Archer (Emiya)",
        "Rin Tohsaka",
        "Counter guardian of infinite steel.",
        Series::StayNight,
        145, 24, 17, 23, nps, art, mirrored
    );
}

static Servant makeLancerCu() {
    std::vector<NoblePhantasm> nps = {
        { "Gae Bolg", "Cursed spear that strikes the heart.", 42, 1.9f },
        { "Soaring Spear", "Long-range spear throw.", 30, 1.4f }
    };

    std::string art =
        BLOOD_RED    "                        /\\\n"
        BLOOD_RED    "                       /  \\\n"
        BRIGHT_RED   "======================" BLOOD_RED "/ /\\ \\====================>\n"
        ROYAL_BLUE   "                    /_/  \\_\\\n"
        SKY_BLUE     "                   /  /" SILVER "[] " SKY_BLUE "\\  \\\n"
        ROYAL_BLUE   "                  /  / " SILVER "|| " ROYAL_BLUE "\\  \\\n"
        ROYAL_BLUE   "                 /  /  " SILVER "|| " ROYAL_BLUE " \\  \\\n"
        ROYAL_BLUE   "                /__/___" SILVER "||" ROYAL_BLUE "__\\__\\\n"
        BRIGHT_BLUE  "                   /    ||    \\\n"
        BRIGHT_BLUE  "                  /     ||     \\\n"
        BLOOD_RED    "                 /______" BRIGHT_RED "||" BLOOD_RED "______\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Kirei Kotomine → Lancer (Cu Chulainn) in FSN
    return Servant(
        "Lancer (Cu Chulainn)",
        "Kirei Kotomine",
        "Hound of Ulster bearing the cursed spear Gae Bolg.",
        Series::StayNight,
        150, 23, 16, 25, nps, art, mirrored
    );
}

static Servant makeRiderMedusa() {
    std::vector<NoblePhantasm> nps = {
        { "Bellerophon", "Pegasus charge that tramples the field.", 45, 1.8f },
        { "Blood Fort Andromeda", "Barrier that drains life.", 25, 1.2f }
    };

    std::string art =
        LAVENDER      "             ~~~~~~~~~~~~~~~~~~~~~~\n"
        LAVENDER      "         ~~~~~~~" PURPLE "///////////" LAVENDER "~~~~~~~\n"
        LAVENDER      "      ~~~~~~~~~" PURPLE "/////////////" LAVENDER "~~~~~~~~\n"
        PURPLE        "                /  " MAGENTA "------" PURPLE "  \\\n"
        PURPLE        "               /  /" BLACK "[][]" PURPLE "\\  \\\n"
        BLACK         "               |  |" MAGENTA "    " BLACK "|  |\n"
        BLACK         "               |  |" MAGENTA "____" BLACK "|  |\n"
        BLACK         "               | /" PURPLE "||||||" BLACK "\\ |\n"
        BLACK         "               |/" PURPLE "||||||||" BLACK "\\|\n"
        LAVENDER      "              / " PURPLE "||||||||||" LAVENDER " \\\n"
        LAVENDER      "             /  " PURPLE "||||||||||" LAVENDER "  \\\n"
        LAVENDER      "            /___/" PURPLE "||||||||" LAVENDER "\\___\\\n"
        LAVENDER      "               /  " PURPLE "||||" LAVENDER "  \\\n"
        LAVENDER      "              /___/" PURPLE "||" LAVENDER "\\___\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Sakura Matou → Rider (Medusa)
    return Servant(
        "Rider (Medusa)",
        "Sakura Matou",
        "Gorgon sister mounted upon the winged steed.",
        Series::StayNight,
        140, 22, 15, 24, nps, art, mirrored
    );
}

static Servant makeBerserkerHeracles() {
    std::vector<NoblePhantasm> nps = {
        { "Nine Lives", "Multi-hit onslaught of overwhelming force.", 55, 2.1f }
    };

    std::string art =
        ASH           "                  _________\n"
        DARK_BROWN    "                /'  ___  '\\\n"
        DARK_BROWN    "               /   /   \\   \\\n"
        ASH           "              /___/_____|___\\\n"
        DARK_BROWN    "           ___|   " ASH " ___ " DARK_BROWN "   |___\n"
        DARK_BROWN    "         /'   |  " ASH "|   |" DARK_BROWN "  |   '\\\n"
        DARK_BROWN    "        /     |  " ASH "|___|" DARK_BROWN "  |     \\\n"
        DARK_BROWN    "       |      |  " ASH " ___ " DARK_BROWN "  |      |\n"
        DARK_BROWN    "       |      |  " ASH "|   |" DARK_BROWN "  |      |\n"
        STEEL         "       |   [" DARK_BROWN "___" STEEL "]   [" DARK_BROWN "___" STEEL "]   |\n"
        DARK_BROWN    "       |      /_________\\      |\n"
        DARK_BROWN    "       |_____/ / /   \\ \\ \\_____|\n"
        STEEL         "           /__/ /_____\\ \\__\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Illyasviel von Einzbern → Berserker (Heracles)
    return Servant(
        "Berserker (Heracles)",
        "Illyasviel von Einzbern",
        "Mad warrior bearing countless lives.",
        Series::StayNight,
        210, 30, 22, 13, nps, art, mirrored
    );
}

static Servant makeArcherGilgameshStayNight() {
    std::vector<NoblePhantasm> nps = {
        { "Gate of Babylon", "Endless barrage of treasures.", 48, 2.0f },
        { "Enuma Elish", "Sword of rupture that tears space.", 60, 2.5f }
    };

    std::string art =
        GOLD          "                 _________\n"
        DARK_GOLD     "                /  ___   /|\n"
        GOLD          "               /  /" PALE_GOLD "[]" GOLD " \\ / |\n"
        DARK_GOLD     "              /__/_____/ / |\n"
        BLACK         "              |  " GOLD "____" BLACK "  |  |\n"
        BLACK         "              | |" IVORY "    " BLACK "| |  |\n"
        BLACK         "              | |" IVORY "____" BLACK "| |  |\n"
        BLACK         "              | |  " GOLD "||" BLACK "  | |  |\n"
        BLACK         "              | |  " GOLD "||" BLACK "  | |  |\n"
        BLACK         "              |_|__" GOLD "||" BLACK "__|_|__|\n"
        GOLD          "                 /_/  \\_\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Shinji Matou → Archer (Gilgamesh), UBW route
    return Servant(
        "Archer (Gilgamesh)",
        "Shinji Matou",
        "Golden king wielding a treasury of legends.",
        Series::StayNight,
        155, 27, 18, 20, nps, art, mirrored
    );
}

static Servant makeAssassinSasaki() {
    std::vector<NoblePhantasm> nps = {
        { "Tsubame Gaeshi", "Triple strike that ignores space.", 38, 1.8f }
    };

    std::string art =
        LAVENDER      "                _________\n"
        PURPLE        "               /  ____  \\\n"
        VIOLET        "              /  /    \\  \\\n"
        PURPLE        "             /__/______\\__\\\n"
        VIOLET        "               |  " BLACK "--" VIOLET "  |\n"
        VIOLET        "               |  " BLACK "[]" VIOLET "  |\n"
        DEEP_PURPLE   "               |  " PURPLE "||" DEEP_PURPLE "  |\n"
        DEEP_PURPLE   "               |  " PURPLE "||" DEEP_PURPLE "  |\n"
        DEEP_PURPLE   "               |  " PURPLE "||" DEEP_PURPLE "  |\n"
        LAVENDER      "               |__" PURPLE "||" LAVENDER "__|====================>\n"
        LAVENDER      "                  /  \\\n"
        LAVENDER      "                 /____\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Caster (Medea) → Assassin (Sasaki Kojiro)
    return Servant(
        "Assassin (Sasaki)",
        "Caster (Medea)",
        "Nameless swordsman who slashes beyond space.",
        Series::StayNight,
        135, 22, 14, 26, nps, art, mirrored
    );
}

static Servant makeCasterMedea() {
    std::vector<NoblePhantasm> nps = {
        { "Rule Breaker", "Dagger that severs contracts.", 24, 1.3f },
        { "Age of Gods Magecraft", "High ritual magic.", 40, 1.6f }
    };

    std::string art =
        DEEP_PURPLE   "                 _________\n"
        BLACK         "                /  /\\ /\\  \\\n"
        DEEP_PURPLE   "               /__/  V  \\__\\\n"
        BLACK         "               |  " DEEP_PURPLE "______" BLACK "  |\n"
        PURPLE        "               | /" BLACK " ____ " PURPLE "\\ |\n"
        PURPLE        "               | |" DEEP_PURPLE " |||| " PURPLE "| |\n"
        PURPLE        "               | |" DEEP_PURPLE " |||| " PURPLE "| |\n"
        PURPLE        "               | |" DEEP_PURPLE " |||| " PURPLE "| |\n"
        PURPLE        "               | |" DEEP_PURPLE " |||| " PURPLE "| |\n"
        BLACK         "               | |______| |\n"
        DEEP_PURPLE   "               |/________\\|\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Souichirou Kuzuki → Caster (Medea)
    return Servant(
        "Caster (Medea)",
        "Souichirou Kuzuki",
        "Witch of betrayal wielding ancient magecraft.",
        Series::StayNight,
        135, 20, 15, 18, nps, art, mirrored
    );
}

static Servant makeAssassinHassanStayNight() {
    std::vector<NoblePhantasm> nps = {
        { "Zabaniya", "Heart-crushing assassination.", 34, 1.7f }
    };

    std::string art =
        BRIGHT_BLACK  "                _________\n"
        BRIGHT_BLACK  "              /'  ____  '\\\n"
        BONE          "             /  /" BLACK ".__." BONE "\\  \\\n"
        BONE          "            /__/ " BLACK "\\__/ " BONE "\\__\\\n"
        BRIGHT_BLACK  "              /  /____\\  \\\n"
        BRIGHT_BLACK  "             /  / " ASH "||" BRIGHT_BLACK " \\  \\\n"
        BRIGHT_BLACK  "            /  /  " ASH "||" BRIGHT_BLACK "  \\  \\\n"
        BRIGHT_BLACK  "       ____/  /___" ASH "||" BRIGHT_BLACK "___\\  \\____\n"
        BRIGHT_BLACK  "      /____   ____/" ASH "||" BRIGHT_BLACK "\\____   ____\\\n"
        BRIGHT_BLACK  "           /  /    " ASH "||" BRIGHT_BLACK "    \\  \\\n"
        BRIGHT_BLACK  "          /__/     " ASH "||" BRIGHT_BLACK "     \\__\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Optional: Kirei also controls Assassin in FSN
    return Servant(
        "Assassin (Hassan)",
        "Kirei Kotomine",
        "Old man of the mountain, silent killer.",
        Series::StayNight,
        135, 21, 15, 24, nps, art, mirrored
    );
}

// ─────────────────────────────────────────────
// Fate/Zero servants (Fourth War)
// ─────────────────────────────────────────────

static Servant makeSaberArtoriaZero() {
    std::vector<NoblePhantasm> nps = {
        { "Excalibur", "Holy sword beam that obliterates foes.", 50, 2.3f },
        { "Invisible Air", "Wind cloak around the blade.", 20, 1.3f }
    };

    std::string art =
        PALE_GOLD     "                  /\\\n"
        BRIGHT_YELLOW "                 /  \\\n"
        PALE_GOLD     "            ____/ /\\ \\____\n"
        SILVER        "           / __  /  \\  __ \\\n"
        BRIGHT_YELLOW "          /_/ /_/ /\\ \\_\\ \\_\\\n"
        NAVY          "             /  / /==\\ \\  \\\n"
        SILVER        "            /  / / " BLUE "[]" SILVER " \\ \\  \\\n"
        NAVY          "           /  / /  " SILVER "||" NAVY "  \\ \\  \\\n"
        SILVER        "          /__/ /___" NAVY "||" SILVER "___\\ \\__\\\n"
        BRIGHT_BLUE   "             / /    ||    \\ \\\n"
        BRIGHT_BLUE   "            / /     ||     \\ \\\n"
        SILVER        "           /_/______/\\______\\_\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Kiritsugu Emiya → Saber (Artoria) in Zero
    return Servant(
        "Saber (Artoria Zero)",
        "Kiritsugu Emiya",
        "King in a different era of the Grail War.",
        Series::Zero,
        165, 27, 21, 19, nps, art, mirrored
    );
}

static Servant makeArcherGilgameshZero() {
    std::vector<NoblePhantasm> nps = {
        { "Gate of Babylon", "Endless barrage of treasures.", 50, 2.1f },
        { "Enuma Elish", "Sword of rupture that tears worlds.", 65, 2.6f }
    };

    std::string art =
        GOLD          "                _________\n"
        DARK_GOLD     "               /__/^^\\__\\\n"
        GOLD          "              /  /====\\  \\\n"
        DARK_GOLD     "             /__/ " BRIGHT_YELLOW "[] []" DARK_GOLD " \\__\\\n"
        GOLD          "               |  " DARK_GOLD "____" GOLD "  |\n"
        GOLD          "               | /" DARK_GOLD " |||| " GOLD "\\ |\n"
        GOLD          "               |/ " DARK_GOLD " |||| " GOLD " \\\n"
        DARK_GOLD     "               |  " GOLD " |||| " DARK_GOLD "  |\n"
        DARK_GOLD     "               |  " GOLD " |||| " DARK_GOLD "  |\n"
        GOLD          "               |__" DARK_GOLD "_||_" GOLD "__|\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Tokiomi Tohsaka → Archer (Gilgamesh) in Zero
    return Servant(
        "Archer (Gilgamesh Zero)",
        "Tokiomi Tohsaka",
        "Golden king summoned in the Fourth War.",
        Series::Zero,
        160, 29, 19, 21, nps, art, mirrored
    );
}

static Servant makeRiderIskandar() {
    std::vector<NoblePhantasm> nps = {
        { "Ionioi Hetairoi", "Army of loyal companions charges.", 55, 2.0f }
    };

    std::string art =
        CRIMSON       "                _________\n"
        CRIMSON       "               /  ____  \\\n"
        DARK_GOLD     "              /  / __ \\  \\\n"
        CRIMSON       "             /__/ /  \\ \\__\\\n"
        BROWN         "               | /" TAN "____" BROWN "\\ |\n"
        BROWN         "               |/" TAN " /||\\ " BROWN "\\|\n"
        CRIMSON       "               / " TAN "/ || \\ " CRIMSON "\\\n"
        CRIMSON       "              /__/ " TAN "_||_" CRIMSON "\\__\\\n"
        DARK_GOLD     "                /  ||  \\\n"
        DARK_GOLD     "               /___||___\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Waver Velvet → Rider (Iskandar)
    return Servant(
        "Rider (Iskandar)",
        "Waver Velvet",
        "Conquering king charging with his companions.",
        Series::Zero,
        190, 28, 21, 14, nps, art, mirrored
    );
}

static Servant makeCasterGilles() {
    std::vector<NoblePhantasm> nps = {
        { "Prelati's Spellbook", "Sea of eldritch creatures.", 45, 1.8f }
    };

    std::string art =
        DEEP_PURPLE   "                _________\n"
        BLACK         "               /  /\\ /\\  \\\n"
        DEEP_PURPLE   "              /__/  V  \\__\\\n"
        BLACK         "              |  " DEEP_PURPLE "______" BLACK "  |\n"
        PURPLE        "              | /" BLACK " ____ " PURPLE "\\ |\n"
        PURPLE        "              | |" DEEP_PURPLE " |||| " PURPLE "| |\n"
        PURPLE        "              | |" DEEP_PURPLE " |||| " PURPLE "| |\n"
        PURPLE        "              | |" DEEP_PURPLE " |||| " PURPLE "| |\n"
        BLACK         "              | |______| |\n"
        DEEP_PURPLE   "              |/________\\|\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Ryuunosuke Uryuu → Caster (Gilles)
    return Servant(
        "Caster (Gilles)",
        "Ryuunosuke Uryuu",
        "Mad caster invoking a sea of horrors.",
        Series::Zero,
        145, 21, 16, 17, nps, art, mirrored
    );
}

static Servant makeLancerDiarmuid() {
    std::vector<NoblePhantasm> nps = {
        { "Gae Dearg & Gae Buidhe", "Twin cursed spears.", 40, 1.9f }
    };

    std::string art =
        YELLOW        "         <=====\n"
        CYAN          "                /\\\n"
        CYAN          "               /  \\\n"
        SILVER        "          _____/ /\\ \\_____\n"
        CYAN          "         /____/_/  \\_\\____\\\n"
        CYAN          "              /  " SILVER "[] " CYAN " \\\n"
        CYAN          "             /   " SILVER "|| " CYAN "  \\\n"
        CYAN          "            /____" SILVER "||" CYAN "____\\\n"
        YELLOW        "                  ||========>\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Kayneth El-Melloi Archibald → Lancer (Diarmuid)
    return Servant(
        "Lancer (Diarmuid)",
        "Kayneth El-Melloi Archibald",
        "Knight of the love spot, bearing twin spears.",
        Series::Zero,
        150, 24, 17, 24, nps, art, mirrored
    );
}

static Servant makeBerserkerLancelot() {
    std::vector<NoblePhantasm> nps = {
        { "Knight of Owner", "Any weapon becomes a Noble Phantasm.", 46, 2.0f }
    };

    std::string art =
        BRIGHT_BLACK  "                _________\n"
        BRIGHT_BLACK  "               /  _____  \\\n"
        BLACK         "              /  /" BRIGHT_BLACK " ___ " BLACK "\\  \\\n"
        BLACK         "             /__/ /" BRIGHT_BLACK "_ _" BLACK "\\__\\\n"
        BRIGHT_BLACK  "               | /" BLACK "_|_|_" BRIGHT_BLACK "\\ |\n"
        BRIGHT_BLACK  "               | |" BLACK " ||| " BRIGHT_BLACK "| |\n"
        BRIGHT_BLACK  "               | |" BLACK " ||| " BRIGHT_BLACK "| |\n"
        BRIGHT_BLACK  "               | |" BLACK " ||| " BRIGHT_BLACK "| |\n"
        BRIGHT_BLACK  "               |_|" BLACK "_|||_" BRIGHT_BLACK "|_|\n"
        BRIGHT_BLACK  "                 /_/ \\_\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Kariya Matou → Berserker (Lancelot)
    return Servant(
        "Berserker (Lancelot)",
        "Kariya Matou",
        "Knight consumed by blackened madness.",
        Series::Zero,
        185, 27, 20, 18, nps, art, mirrored
    );
}

static Servant makeAssassinHassanZero() {
    std::vector<NoblePhantasm> nps = {
        { "Zabaniya (Hundred Faces)", "Assault from countless bodies.", 36, 1.8f }
    };

    std::string art =
        BRIGHT_BLACK  "                _________\n"
        BLACK         "               /  _____  \\\n"
        BRIGHT_BLACK  "              /  /     \\  \\\n"
        BRIGHT_BLACK  "             /__/_______\\__\\\n"
        ASH           "               | /" BRIGHT_BLACK " ___ " ASH "\\ |\n"
        ASH           "               |/" BRIGHT_BLACK " /   \\ " ASH "\\|\n"
        BRIGHT_BLACK  "               |  |" ASH "___" BRIGHT_BLACK "|  |\n"
        BRIGHT_BLACK  "               |  |" ASH "||| " BRIGHT_BLACK "|  |\n"
        BRIGHT_BLACK  "               |__|" ASH "___" BRIGHT_BLACK "|__|\n"
        BRIGHT_BLACK  "                 /_/ \\_\\\n"
        RESET;

    std::string mirrored = mirrorAscii(art);

    // Kirei Kotomine → Assassin (Hassan-i Sabbah) in Zero
    return Servant(
        "Assassin (Hassan Zero)",
        "Kirei Kotomine",
        "Hundred-faced assassin unleashed in a prior war.",
        Series::Zero,
        140, 22, 15, 25, nps, art, mirrored
    );
}

// ─────────────────────────────────────────────
// Factories & selectors
// ─────────────────────────────────────────────

Servant Servant::randomServant() {
    std::vector<Servant> all;
    all.push_back(makeSaberArtoriaStayNight());
    all.push_back(makeArcherEmiya());
    all.push_back(makeLancerCu());
    all.push_back(makeRiderMedusa());
    all.push_back(makeBerserkerHeracles());
    all.push_back(makeArcherGilgameshStayNight());
    all.push_back(makeAssassinSasaki());
    all.push_back(makeCasterMedea());
    all.push_back(makeAssassinHassanStayNight());
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

std::vector<Servant> Servant::allServants() {
    std::vector<Servant> all;
    all.push_back(makeSaberArtoriaStayNight());
    all.push_back(makeArcherEmiya());
    all.push_back(makeLancerCu());
    all.push_back(makeRiderMedusa());
    all.push_back(makeBerserkerHeracles());
    all.push_back(makeArcherGilgameshStayNight());
    all.push_back(makeAssassinSasaki());
    all.push_back(makeCasterMedea());
    all.push_back(makeAssassinHassanStayNight());
    all.push_back(makeSaberArtoriaZero());
    all.push_back(makeArcherGilgameshZero());
    all.push_back(makeRiderIskandar());
    all.push_back(makeCasterGilles());
    all.push_back(makeLancerDiarmuid());
    all.push_back(makeBerserkerLancelot());
    all.push_back(makeAssassinHassanZero());
    return all;
}

Servant Servant::randomServantForSeries(Series s) {
    auto all = allServants();
    std::vector<Servant> pool;
    for (auto& sv : all) {
        if (sv.getSeries() == s) pool.push_back(sv);
    }
    if (pool.empty()) return randomServant();
    int idx = std::rand() % pool.size();
    return pool[idx];
}

Servant Servant::selectServantForMaster(const std::string& masterInput, Series s) {
    auto all = allServants();
    std::string lower;
    for (char c : masterInput) lower += std::tolower(static_cast<unsigned char>(c));

    for (auto& sv : all) {
        if (sv.getSeries() != s) continue;
        std::string m = sv.getMasterName();
        std::string mLower;
        for (char c : m) mLower += std::tolower(static_cast<unsigned char>(c));
        if (mLower == lower) return sv;
    }
    return randomServantForSeries(s);
}

std::vector<Servant> Servant::enemyPoolForSeries(Series s) {
    std::vector<Servant> pool;
    auto all = allServants();
    for (auto& sv : all) {
        if (sv.getSeries() == s) pool.push_back(sv);
    }
    return pool;
}