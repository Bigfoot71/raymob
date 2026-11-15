#ifndef NBAPP_GAME_WORLD_H
#define NBAPP_GAME_WORLD_H

#include <cstdint>
#include <string>
#include <vector>

namespace nbapp {

static const int WORLD_W = 96;
static const int WORLD_H = 96;
static const int MAX_PLAYERS = 8;
static const int INVENTORY_CAPACITY = 8;

enum TileType { TILE_GRASS = 0, TILE_WALL, TILE_TREE, TILE_WATER, TILE_PATH, TILE_RUINS, TILE_SAND };
enum ClassType { CLASS_WARRIOR = 0, CLASS_ARCHER, CLASS_MAGE, CLASS_NECROMANCER };
enum MonsterType { MOB_SLIME = 0, MOB_GOBLIN, MOB_SKELETON, MOB_BAT, MOB_ORC, MOB_WOLF };
enum LootType { LOOT_GOLD = 0, LOOT_POTION, LOOT_EQUIPMENT };
enum ItemType {
    ITEM_NONE = 0,
    ITEM_SWORD, ITEM_BOW, ITEM_STAFF, ITEM_TOTEM,
    ITEM_LEATHER, ITEM_CHAIN, ITEM_ROBE,
    ITEM_AMULET, ITEM_RING
};
enum ItemRarity { RARITY_COMMON = 0, RARITY_UNCOMMON, RARITY_RARE, RARITY_EPIC, RARITY_LEGENDARY };
enum EquipmentSlot { SLOT_WEAPON = 0, SLOT_ARMOR, SLOT_CHARM, SLOT_COUNT };

struct Item {
    int type;
    int rarity;
    int power;
    Item(int t = ITEM_NONE, int r = RARITY_COMMON, int p = 0) : type(t), rarity(r), power(p) {}
    bool valid() const { return type != ITEM_NONE; }
};

struct Player {
    int id;
    std::string name;
    int classId;
    int x, y;
    int hp, maxHp;
    int mana, maxMana;
    int level, xp, gold, potions;
    bool alive;
    int inputDx, inputDy;
    int facingDx, facingDy;
    float moveTimer;
    float skillCooldown;
    float manaRegenTimer;
    Item equipment[SLOT_COUNT];
    std::vector<Item> inventory;
    Player();
};

struct Monster {
    int id;
    int x, y;
    int hp, maxHp;
    int type;
    bool alive;
    Monster();
};

struct Loot {
    int id;
    int x, y;
    int type;
    int amount;
    int itemType;
    int rarity;
    int power;
    bool active;
    Loot();
};

struct InputState {
    int dx, dy;
    bool usePotion;
    bool useSkill;
    int equipIndex;
    InputState(int x = 0, int y = 0, bool potion = false, bool skill = false, int equip = -1)
        : dx(x), dy(y), usePotion(potion), useSkill(skill), equipIndex(equip) {}
};

class World {
public:
    World();
    void generate(uint32_t seed);
    bool addPlayer(int id, const std::string& name, int classId);
    void removePlayer(int id);
    void setInput(int playerId, const InputState& input);
    void update(float deltaSeconds);

    bool isWalkable(int x, int y) const;
    bool isOccupiedByPlayer(int x, int y, int exceptId = -1) const;
    int tileAt(int x, int y) const;
    const Player* findPlayer(int id) const;
    Player* findPlayer(int id);
    bool equipInventoryItem(int playerId, int inventoryIndex);
    bool useClassSkill(int playerId);
    const std::vector<Player>& players() const { return players_; }
    const std::vector<Monster>& monsters() const { return monsters_; }
    const std::vector<Loot>& loot() const { return loot_; }
    int tick() const { return tick_; }
    uint32_t seed() const { return seed_; }
    bool serializeSnapshot(std::string& output) const;
    bool deserializeSnapshot(const std::string& input);

private:
    int tiles_[WORLD_H][WORLD_W];
    std::vector<Player> players_;
    std::vector<Monster> monsters_;
    std::vector<Loot> loot_;
    uint32_t seed_;
    int tick_;
    int nextMonsterId_;
    int nextLootId_;
    float monsterAccumulator_;

    bool monsterAt(int x, int y, int* index = 0) const;
    bool lootAt(int x, int y, int* index = 0) const;
    void stepPlayer(Player& player);
    void usePotion(Player& player);
    void attack(Player& player, int dx, int dy, int multiplier = 1);
    void damageMonster(Player& player, Monster& monster, int damage);
    void moveMonsters();
    void moveMonster(Monster& monster, const Player& target);
    void collectLoot(Player& player);
    void addXp(Player& player, int amount);
    void spawnLoot(int x, int y, int type, int amount, int itemType = ITEM_NONE, int rarity = 0, int power = 0);
    int baseMaxHp(const Player& player) const;
    int baseMaxMana(const Player& player) const;
    int attackDamage(const Player& player) const;
    int armorDefense(const Player& player) const;
    void refreshDerivedStats(Player& player);
};

const char* className(int classId);
const char* monsterName(int monsterId);
const char* itemName(int itemType);
const char* rarityName(int rarity);
int itemSlot(int itemType);

} // namespace nbapp
#endif
