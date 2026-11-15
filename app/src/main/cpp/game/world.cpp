#include "world.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <random>

namespace nbapp {

Player::Player() : id(0), name("Player"), classId(CLASS_WARRIOR), x(WORLD_W / 2), y(WORLD_H / 2),
    hp(100), maxHp(100), mana(40), maxMana(40), level(1), xp(0), gold(0), potions(2), alive(true),
    inputDx(0), inputDy(0), facingDx(0), facingDy(1), moveTimer(0.0f), skillCooldown(0.0f), manaRegenTimer(0.0f) {}
Monster::Monster() : id(0), x(0), y(0), hp(10), maxHp(10), type(MOB_SLIME), alive(true) {}
Loot::Loot() : id(0), x(0), y(0), type(LOOT_GOLD), amount(1), itemType(ITEM_NONE), rarity(0), power(0), active(true) {}

const char* className(int id) {
    switch (id) {
        case CLASS_ARCHER: return "Лучник";
        case CLASS_MAGE: return "Маг";
        case CLASS_NECROMANCER: return "Некромант";
        default: return "Воин";
    }
}

const char* monsterName(int id) {
    switch (id) {
        case MOB_GOBLIN: return "Гоблин";
        case MOB_SKELETON: return "Скелет";
        case MOB_BAT: return "Летучая мышь";
        case MOB_ORC: return "Орк";
        case MOB_WOLF: return "Волк";
        default: return "Слизень";
    }
}

const char* itemName(int id) {
    switch (id) {
        case ITEM_SWORD: return "Меч";
        case ITEM_BOW: return "Лук";
        case ITEM_STAFF: return "Посох";
        case ITEM_TOTEM: return "Тотем";
        case ITEM_LEATHER: return "Кожаная броня";
        case ITEM_CHAIN: return "Кольчуга";
        case ITEM_ROBE: return "Мантия";
        case ITEM_AMULET: return "Амулет";
        case ITEM_RING: return "Кольцо";
        default: return "Пусто";
    }
}

const char* rarityName(int rarity) {
    switch (rarity) {
        case RARITY_UNCOMMON: return "Необычный";
        case RARITY_RARE: return "Редкий";
        case RARITY_EPIC: return "Эпический";
        case RARITY_LEGENDARY: return "Легендарный";
        default: return "Обычный";
    }
}

int itemSlot(int type) {
    if (type >= ITEM_SWORD && type <= ITEM_TOTEM) return SLOT_WEAPON;
    if (type >= ITEM_LEATHER && type <= ITEM_ROBE) return SLOT_ARMOR;
    if (type == ITEM_AMULET || type == ITEM_RING) return SLOT_CHARM;
    return -1;
}

World::World() : seed_(1), tick_(0), nextMonsterId_(1), nextLootId_(1), monsterAccumulator_(0.0f) {
    for (int y = 0; y < WORLD_H; ++y)
        for (int x = 0; x < WORLD_W; ++x) tiles_[y][x] = TILE_GRASS;
}

void World::generate(uint32_t seed) {
    seed_ = seed ? seed : 1;
    tick_ = 0;
    nextMonsterId_ = 1;
    nextLootId_ = 1;
    monsterAccumulator_ = 0.0f;
    players_.clear(); monsters_.clear(); loot_.clear();
    std::mt19937 rng(seed_);

    // Base terrain: coherent patches rather than isolated random obstacles.
    for (int y = 0; y < WORLD_H; ++y) {
        for (int x = 0; x < WORLD_W; ++x) {
            if (x == 0 || y == 0 || x == WORLD_W - 1 || y == WORLD_H - 1) {
                tiles_[y][x] = TILE_WALL;
                continue;
            }
            const int dx = x - WORLD_W / 2, dy = y - WORLD_H / 2;
            const int d = std::abs(dx) + std::abs(dy);
            const int roll = static_cast<int>(rng() % 100);
            if (d < 9) tiles_[y][x] = TILE_GRASS;
            else if (roll < 7) tiles_[y][x] = TILE_WATER;
            else if (roll < 18) tiles_[y][x] = TILE_TREE;
            else if (roll < 23) tiles_[y][x] = TILE_SAND;
            else if (roll < 28) tiles_[y][x] = TILE_RUINS;
            else tiles_[y][x] = TILE_GRASS;
        }
    }

    // A small safe clearing and paths leading out of it.
    const int cx = WORLD_W / 2, cy = WORLD_H / 2;
    for (int y = cy - 7; y <= cy + 7; ++y)
        for (int x = cx - 7; x <= cx + 7; ++x) tiles_[y][x] = TILE_GRASS;
    for (int x = 2; x < WORLD_W - 2; ++x) tiles_[cy][x] = TILE_PATH;
    for (int y = 2; y < WORLD_H - 2; ++y) tiles_[y][cx] = TILE_PATH;
    for (int y = cy - 3; y <= cy + 3; ++y)
        for (int x = cx - 3; x <= cx + 3; ++x) tiles_[y][x] = TILE_GRASS;

    // Make terrain patches readable and impassable edges deterministic.
    for (int i = 0; i < 18; ++i) {
        int px = 5 + static_cast<int>(rng() % (WORLD_W - 10));
        int py = 5 + static_cast<int>(rng() % (WORLD_H - 10));
        int radius = 2 + static_cast<int>(rng() % 4);
        int type = (rng() % 3 == 0) ? TILE_WATER : TILE_TREE;
        for (int y = py - radius; y <= py + radius; ++y)
            for (int x = px - radius; x <= px + radius; ++x) {
                if (x < 2 || y < 2 || x >= WORLD_W - 2 || y >= WORLD_H - 2) continue;
                if (std::abs(x - cx) + std::abs(y - cy) < 12) continue;
                const int ax = x - px, ay = y - py;
                if (ax * ax + ay * ay <= radius * radius) tiles_[y][x] = type;
            }
    }

    // Scatter ruins in compact groups.
    for (int i = 0; i < 9; ++i) {
        int px = 4 + static_cast<int>(rng() % (WORLD_W - 8));
        int py = 4 + static_cast<int>(rng() % (WORLD_H - 8));
        if (std::abs(px - cx) + std::abs(py - cy) < 16) continue;
        for (int oy = -1; oy <= 1; ++oy)
            for (int ox = -1; ox <= 1; ++ox)
                if ((rng() % 4) != 0 && px + ox > 1 && py + oy > 1)
                    tiles_[py + oy][px + ox] = TILE_RUINS;
    }

    int attempts = 0;
    while (monsters_.size() < 72 && attempts++ < 5000) {
        int x = 2 + static_cast<int>(rng() % (WORLD_W - 4));
        int y = 2 + static_cast<int>(rng() % (WORLD_H - 4));
        if (!isWalkable(x, y)) continue;
        const int distance = std::abs(x - cx) + std::abs(y - cy);
        if (distance < 13 || monsterAt(x, y)) continue;
        Monster m;
        m.id = nextMonsterId_++;
        m.x = x; m.y = y;
        int roll = static_cast<int>(rng() % 100);
        if (distance > 38 && roll < 16) m.type = MOB_ORC;
        else if (distance > 26 && roll < 30) m.type = MOB_SKELETON;
        else if (roll < 16) m.type = MOB_WOLF;
        else if (roll < 34) m.type = MOB_GOBLIN;
        else if (roll < 52) m.type = MOB_BAT;
        else m.type = MOB_SLIME;
        int danger = std::max(0, (distance - 12) / 18);
        m.maxHp = 9 + danger * 5;
        if (m.type == MOB_ORC) m.maxHp += 15;
        if (m.type == MOB_SKELETON) m.maxHp += 6;
        if (m.type == MOB_WOLF) m.maxHp += 3;
        m.hp = m.maxHp;
        monsters_.push_back(m);
    }
    for (int i = 0; i < 42; ++i) {
        int x = 3 + static_cast<int>(rng() % (WORLD_W - 6));
        int y = 3 + static_cast<int>(rng() % (WORLD_H - 6));
        if (!isWalkable(x, y) || std::abs(x - cx) + std::abs(y - cy) < 10 || lootAt(x, y)) continue;
        spawnLoot(x, y, (rng() % 5 == 0) ? LOOT_POTION : LOOT_GOLD,
                  (rng() % 5 == 0) ? 1 : 2 + static_cast<int>(rng() % 9));
    }
}

bool World::addPlayer(int id, const std::string& rawName, int classId) {
    if (findPlayer(id) || players_.size() >= MAX_PLAYERS || id < 0) return false;
    Player p;
    p.id = id;
    p.name.clear();
    for (std::size_t i = 0; i < rawName.size() && p.name.size() < 14; ++i) {
        unsigned char c = static_cast<unsigned char>(rawName[i]);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') p.name.push_back(static_cast<char>(c));
    }
    if (p.name.empty()) p.name = "Player";
    p.classId = std::max(0, std::min(3, classId));
    p.x = WORLD_W / 2; p.y = WORLD_H / 2;
    static const int offsets[8][2] = {{0,0},{1,0},{0,1},{-1,0},{0,-1},{1,1},{-1,1},{1,-1}};
    for (int i = 0; i < 8; ++i) {
        int x = WORLD_W / 2 + offsets[i][0], y = WORLD_H / 2 + offsets[i][1];
        if (isWalkable(x, y) && !isOccupiedByPlayer(x, y) && !monsterAt(x, y)) { p.x = x; p.y = y; break; }
    }
    if (p.classId == CLASS_WARRIOR) {
        p.equipment[SLOT_WEAPON] = Item(ITEM_SWORD, RARITY_COMMON, 2);
        p.equipment[SLOT_ARMOR] = Item(ITEM_CHAIN, RARITY_COMMON, 1);
    } else if (p.classId == CLASS_ARCHER) {
        p.equipment[SLOT_WEAPON] = Item(ITEM_BOW, RARITY_COMMON, 2);
        p.equipment[SLOT_ARMOR] = Item(ITEM_LEATHER, RARITY_COMMON, 1);
    } else if (p.classId == CLASS_MAGE) {
        p.equipment[SLOT_WEAPON] = Item(ITEM_STAFF, RARITY_COMMON, 2);
        p.equipment[SLOT_ARMOR] = Item(ITEM_ROBE, RARITY_COMMON, 1);
    } else {
        p.equipment[SLOT_WEAPON] = Item(ITEM_TOTEM, RARITY_COMMON, 2);
        p.equipment[SLOT_ARMOR] = Item(ITEM_LEATHER, RARITY_COMMON, 1);
    }
    p.maxHp = baseMaxHp(p);
    p.hp = p.maxHp;
    p.maxMana = baseMaxMana(p);
    p.mana = p.maxMana;
    players_.push_back(p);
    return true;
}

void World::removePlayer(int id) {
    players_.erase(std::remove_if(players_.begin(), players_.end(), [id](const Player& p) { return p.id == id; }), players_.end());
}

const Player* World::findPlayer(int id) const {
    for (std::size_t i = 0; i < players_.size(); ++i) if (players_[i].id == id) return &players_[i];
    return 0;
}
Player* World::findPlayer(int id) {
    for (std::size_t i = 0; i < players_.size(); ++i) if (players_[i].id == id) return &players_[i];
    return 0;
}

int World::tileAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= WORLD_W || y >= WORLD_H) return TILE_WALL;
    return tiles_[y][x];
}
bool World::isWalkable(int x, int y) const {
    const int t = tileAt(x, y);
    return t != TILE_WALL && t != TILE_TREE && t != TILE_WATER;
}
bool World::isOccupiedByPlayer(int x, int y, int exceptId) const {
    for (std::size_t i = 0; i < players_.size(); ++i)
        if (players_[i].alive && players_[i].id != exceptId && players_[i].x == x && players_[i].y == y) return true;
    return false;
}
bool World::monsterAt(int x, int y, int* index) const {
    for (std::size_t i = 0; i < monsters_.size(); ++i)
        if (monsters_[i].alive && monsters_[i].x == x && monsters_[i].y == y) { if (index) *index = static_cast<int>(i); return true; }
    return false;
}
bool World::lootAt(int x, int y, int* index) const {
    for (std::size_t i = 0; i < loot_.size(); ++i)
        if (loot_[i].active && loot_[i].x == x && loot_[i].y == y) { if (index) *index = static_cast<int>(i); return true; }
    return false;
}

void World::setInput(int playerId, const InputState& input) {
    Player* p = findPlayer(playerId);
    if (!p || !p->alive) return;
    p->inputDx = std::max(-1, std::min(1, input.dx));
    p->inputDy = std::max(-1, std::min(1, input.dy));
    if (p->inputDx != 0 && p->inputDy != 0) p->inputDy = 0;
    if (p->inputDx != 0 || p->inputDy != 0) {
        p->facingDx = p->inputDx;
        p->facingDy = p->inputDy;
    }
    if (input.usePotion) usePotion(*p);
    if (input.useSkill) useClassSkill(playerId);
    if (input.equipIndex >= 0) equipInventoryItem(playerId, input.equipIndex);
}

void World::usePotion(Player& p) {
    if (p.alive && p.potions > 0 && p.hp < p.maxHp) {
        --p.potions;
        p.hp = std::min(p.maxHp, p.hp + 42);
    }
}

void World::spawnLoot(int x, int y, int type, int amount, int itemType, int rarity, int power) {
    if (loot_.size() >= 256) {
        loot_.erase(std::remove_if(loot_.begin(), loot_.end(), [](const Loot& item) { return !item.active; }), loot_.end());
        if (loot_.size() >= 512) return;
    }
    Loot item; item.id = nextLootId_++; item.x = x; item.y = y; item.type = type; item.amount = amount;
    item.itemType = itemType; item.rarity = rarity; item.power = power;
    loot_.push_back(item);
}

void World::collectLoot(Player& p) {
    for (std::size_t i = 0; i < loot_.size(); ++i) {
        Loot& item = loot_[i];
        if (!item.active || item.x != p.x || item.y != p.y) continue;
        if (item.type == LOOT_EQUIPMENT) {
            if (p.inventory.size() >= INVENTORY_CAPACITY) continue;
            p.inventory.push_back(Item(item.itemType, item.rarity, item.power));
        } else if (item.type == LOOT_POTION) p.potions += item.amount;
        else p.gold += item.amount;
        item.active = false;
    }
}

void World::addXp(Player& p, int amount) {
    p.xp += amount;
    while (p.xp >= p.level * 30) {
        p.xp -= p.level * 30;
        ++p.level;
        p.maxHp = baseMaxHp(p);
        p.hp = std::min(p.maxHp, p.hp + 20);
        p.maxMana = baseMaxMana(p);
        p.mana = std::min(p.maxMana, p.mana + 10);
    }
}

int World::baseMaxHp(const Player& p) const {
    int base = p.classId == CLASS_WARRIOR ? 125 : (p.classId == CLASS_ARCHER ? 95 : (p.classId == CLASS_MAGE ? 85 : 105));
    base += (p.level - 1) * 7;
    if (p.equipment[SLOT_ARMOR].valid()) base += p.equipment[SLOT_ARMOR].power * 5;
    if (p.equipment[SLOT_CHARM].valid()) base += p.equipment[SLOT_CHARM].power * 3;
    return base;
}

int World::baseMaxMana(const Player& p) const {
    int base = p.classId == CLASS_WARRIOR ? 40 : (p.classId == CLASS_ARCHER ? 60 : (p.classId == CLASS_MAGE ? 120 : 95));
    if (p.equipment[SLOT_CHARM].valid()) base += p.equipment[SLOT_CHARM].power * 5;
    return base;
}

int World::attackDamage(const Player& p) const {
    int damage = 5 + p.level;
    if (p.classId == CLASS_WARRIOR) damage += 7;
    else if (p.classId == CLASS_ARCHER) damage += 5;
    else if (p.classId == CLASS_MAGE) damage += 9;
    else damage += 5;
    if (p.equipment[SLOT_WEAPON].valid()) damage += p.equipment[SLOT_WEAPON].power * 2;
    return damage;
}

int World::armorDefense(const Player& p) const {
    int defense = p.equipment[SLOT_ARMOR].valid() ? p.equipment[SLOT_ARMOR].power : 0;
    if (p.equipment[SLOT_CHARM].valid()) defense += p.equipment[SLOT_CHARM].power / 2;
    return defense;
}

void World::refreshDerivedStats(Player& p) {
    int oldMaxHp = p.maxHp;
    int oldMaxMana = p.maxMana;
    p.maxHp = baseMaxHp(p);
    p.maxMana = baseMaxMana(p);
    if (p.maxHp > oldMaxHp) p.hp = std::min(p.maxHp, p.hp + (p.maxHp - oldMaxHp));
    else p.hp = std::min(p.hp, p.maxHp);
    if (p.maxMana > oldMaxMana) p.mana = std::min(p.maxMana, p.mana + (p.maxMana - oldMaxMana));
    else p.mana = std::min(p.mana, p.maxMana);
}

bool World::equipInventoryItem(int playerId, int inventoryIndex) {
    Player* p = findPlayer(playerId);
    if (!p || !p->alive || inventoryIndex < 0 || inventoryIndex >= static_cast<int>(p->inventory.size())) return false;
    Item selected = p->inventory[static_cast<std::size_t>(inventoryIndex)];
    int slot = itemSlot(selected.type);
    if (slot < 0 || slot >= SLOT_COUNT) return false;
    Item previous = p->equipment[slot];
    p->equipment[slot] = selected;
    if (previous.valid()) p->inventory[static_cast<std::size_t>(inventoryIndex)] = previous;
    else p->inventory.erase(p->inventory.begin() + inventoryIndex);
    refreshDerivedStats(*p);
    return true;
}

void World::damageMonster(Player& p, Monster& m, int damage) {
    if (!m.alive) return;
    m.hp -= std::max(1, damage);
    if (m.hp > 0) return;
    m.hp = 0; m.alive = false;
    addXp(p, 9 + m.maxHp / 2);
    p.gold += 1 + (m.type == MOB_ORC ? 5 : 0);
    if (p.classId == CLASS_NECROMANCER) p.hp = std::min(p.maxHp, p.hp + 6);
    const int roll = static_cast<int>((m.id * 17u + static_cast<unsigned>(tick_) * 31u + seed_) % 20u);
    if (roll == 0 && loot_.size() < 256) {
        const int itemRoll = static_cast<int>((m.id + tick_) % 9);
        const int type = itemRoll == 0 ? ITEM_AMULET : (itemRoll == 1 ? ITEM_RING :
            (itemRoll == 2 ? ITEM_CHAIN : (itemRoll == 3 ? ITEM_ROBE :
            (itemRoll == 4 ? ITEM_LEATHER : (p.classId == CLASS_ARCHER ? ITEM_BOW :
            (p.classId == CLASS_MAGE ? ITEM_STAFF : (p.classId == CLASS_NECROMANCER ? ITEM_TOTEM : ITEM_SWORD)))))));
        const int danger = std::max(0, (m.maxHp - 9) / 10);
        const int qualityRoll = static_cast<int>((m.id * 13u + static_cast<unsigned>(tick_) * 7u + seed_) % 100u);
        int rarity = RARITY_COMMON;
        if (danger >= 4 && qualityRoll < 2) rarity = RARITY_LEGENDARY;
        else if (danger >= 3 && qualityRoll < 7) rarity = RARITY_EPIC;
        else if (danger >= 2 && qualityRoll < 18) rarity = RARITY_RARE;
        else if (danger >= 1 && qualityRoll < 38) rarity = RARITY_UNCOMMON;
        const int power = std::max(1, 1 + danger / 2 + rarity * 2);
        spawnLoot(m.x, m.y, LOOT_EQUIPMENT, 1, type, rarity, power);
    } else if ((m.id + tick_) % 4 == 0) spawnLoot(m.x, m.y, LOOT_POTION, 1);
    else if ((m.id + tick_) % 3 == 0) spawnLoot(m.x, m.y, LOOT_GOLD, 3 + m.maxHp / 4);
}

void World::attack(Player& p, int dx, int dy, int multiplier) {
    const int range = p.classId == CLASS_ARCHER ? 4 : (p.classId == CLASS_MAGE ? 3 : 1);
    int targetIndex = -1;
    for (int step = 1; step <= range; ++step) {
        int x = p.x + dx * step, y = p.y + dy * step;
        if (!isWalkable(x, y)) break;
        if (monsterAt(x, y, &targetIndex)) break;
    }
    if (targetIndex < 0) return;
    damageMonster(p, monsters_[static_cast<std::size_t>(targetIndex)], attackDamage(p) * std::max(1, multiplier));
}

bool World::useClassSkill(int playerId) {
    Player* p = findPlayer(playerId);
    if (!p || !p->alive || p->skillCooldown > 0.0f) return false;
    const int cost = p->classId == CLASS_WARRIOR ? 18 : (p->classId == CLASS_ARCHER ? 20 : (p->classId == CLASS_MAGE ? 28 : 24));
    if (p->mana < cost) return false;
    p->mana -= cost;
    p->skillCooldown = p->classId == CLASS_WARRIOR ? 4.0f : (p->classId == CLASS_ARCHER ? 3.5f : 5.0f);
    const int dx = p->facingDx == 0 && p->facingDy == 0 ? 0 : p->facingDx;
    const int dy = p->facingDx == 0 && p->facingDy == 0 ? 1 : p->facingDy;
    const int range = p->classId == CLASS_WARRIOR ? 1 : (p->classId == CLASS_ARCHER ? 6 : 5);
    int targetIndex = -1;
    int targetX = p->x, targetY = p->y;
    for (int step = 1; step <= range; ++step) {
        int x = p->x + dx * step, y = p->y + dy * step;
        if (!isWalkable(x, y)) break;
        targetX = x; targetY = y;
        if (monsterAt(x, y, &targetIndex)) break;
    }
    if (targetIndex < 0) { p->mana = std::min(p->maxMana, p->mana + cost / 2); p->skillCooldown = 0.75f; return false; }
    const int base = attackDamage(*p);
    if (p->classId == CLASS_WARRIOR) {
        damageMonster(*p, monsters_[static_cast<std::size_t>(targetIndex)], base * 3);
    } else if (p->classId == CLASS_ARCHER) {
        damageMonster(*p, monsters_[static_cast<std::size_t>(targetIndex)], base * 2);
    } else if (p->classId == CLASS_MAGE) {
        for (std::size_t i = 0; i < monsters_.size(); ++i) {
            Monster& m = monsters_[i];
            if (m.alive && std::abs(m.x - targetX) <= 1 && std::abs(m.y - targetY) <= 1)
                damageMonster(*p, m, base * 2);
        }
    } else {
        Monster& target = monsters_[static_cast<std::size_t>(targetIndex)];
        int dealt = std::min(target.hp, base + base / 2);
        damageMonster(*p, target, dealt);
        p->hp = std::min(p->maxHp, p->hp + std::max(1, dealt / 2));
    }
    return true;
}

void World::stepPlayer(Player& p) {
    if (!p.alive || (p.inputDx == 0 && p.inputDy == 0)) return;
    int dx = p.inputDx, dy = p.inputDy;
    attack(p, dx, dy);
    int nx = p.x + dx, ny = p.y + dy;
    if (monsterAt(nx, ny)) return;
    if (!isWalkable(nx, ny) || isOccupiedByPlayer(nx, ny, p.id)) return;
    p.x = nx; p.y = ny;
    collectLoot(p);
}

void World::moveMonster(Monster& m, const Player& target) {
    int dx = target.x - m.x, dy = target.y - m.y;
    int sx = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
    int sy = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
    int candidates[4][2] = {{sx,0},{0,sy},{-sx,0},{0,-sy}};
    for (int i = 0; i < 4; ++i) {
        if (candidates[i][0] == 0 && candidates[i][1] == 0) continue;
        int nx = m.x + candidates[i][0], ny = m.y + candidates[i][1];
        if (!isWalkable(nx, ny) || monsterAt(nx, ny) || isOccupiedByPlayer(nx, ny)) continue;
        m.x = nx; m.y = ny; return;
    }
}

void World::moveMonsters() {
    for (std::size_t i = 0; i < monsters_.size(); ++i) {
        Monster& m = monsters_[i];
        if (!m.alive) continue;
        const Player* target = 0;
        int best = 100000;
        for (std::size_t p = 0; p < players_.size(); ++p) {
            if (!players_[p].alive) continue;
            int d = std::abs(players_[p].x - m.x) + std::abs(players_[p].y - m.y);
            if (d < best) { best = d; target = &players_[p]; }
        }
        if (!target || best > 9) continue;
        if (best == 1) {
            Player* victim = 0;
            for (std::size_t p = 0; p < players_.size(); ++p) {
                Player& candidate = players_[p];
                if (!candidate.alive || std::abs(candidate.x - m.x) + std::abs(candidate.y - m.y) != 1) continue;
                if (!victim || candidate.hp < victim->hp) victim = &candidate;
            }
            if (victim) {
                int damage = 3 + (m.type == MOB_ORC ? 7 : (m.type == MOB_SKELETON ? 4 : 0));
                damage = std::max(1, damage - armorDefense(*victim));
                victim->hp -= damage;
                if (victim->hp <= 0) { victim->hp = 0; victim->alive = false; victim->inputDx = victim->inputDy = 0; }
            }
        } else moveMonster(m, *target);
    }
}

void World::update(float deltaSeconds) {
    float dt = std::max(0.0f, std::min(deltaSeconds, 0.25f));
    if (dt <= 0.0f) return;
    ++tick_;
    for (std::size_t i = 0; i < players_.size(); ++i) {
        Player& p = players_[i];
        p.moveTimer -= dt;
        p.skillCooldown = std::max(0.0f, p.skillCooldown - dt);
        if (p.mana < p.maxMana) {
            p.manaRegenTimer += dt;
            while (p.manaRegenTimer >= 1.0f) { p.manaRegenTimer -= 1.0f; ++p.mana; }
        } else p.manaRegenTimer = 0.0f;
        if (p.alive && p.moveTimer <= 0.0f && (p.inputDx != 0 || p.inputDy != 0)) {
            stepPlayer(p);
            const float speed = p.classId == CLASS_ARCHER ? 0.125f : (p.classId == CLASS_WARRIOR ? 0.16f : 0.145f);
            p.moveTimer = speed;
        }
    }
    monsterAccumulator_ += dt;
    while (monsterAccumulator_ >= 0.42f) {
        monsterAccumulator_ -= 0.42f;
        moveMonsters();
    }
}

static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> out;
    std::string part;
    std::istringstream in(s);
    while (std::getline(in, part, delimiter)) out.push_back(part);
    if (!s.empty() && s[s.size() - 1] == delimiter) out.push_back("");
    return out;
}

static std::string itemToString(const Item& item) {
    if (!item.valid()) return "-";
    std::ostringstream out;
    out << item.type << ':' << item.rarity << ':' << item.power;
    return out.str();
}

static bool parseNumber(const std::string& text, int& value) {
    if (text.empty()) return false;
    char* end = 0;
    long parsed = std::strtol(text.c_str(), &end, 10);
    if (!end || *end != '\0' || parsed < -2147483647L || parsed > 2147483647L) return false;
    value = static_cast<int>(parsed);
    return true;
}

static bool parseItem(const std::string& text, Item& item) {
    if (text == "-") { item = Item(); return true; }
    std::vector<std::string> v = split(text, ':');
    if (v.size() != 3) return false;
    int type, rarity, power;
    if (!parseNumber(v[0], type) || !parseNumber(v[1], rarity) || !parseNumber(v[2], power)) return false;
    if (itemSlot(type) < 0 || rarity < RARITY_COMMON || rarity > RARITY_LEGENDARY || power < 0 || power > 100) return false;
    item = Item(type, rarity, power);
    return true;
}

bool World::serializeSnapshot(std::string& output) const {
    std::ostringstream out;
    out << "SNAP|2|" << tick_ << '|' << seed_ << '|';
    for (std::size_t i = 0; i < players_.size(); ++i) {
        const Player& p = players_[i]; if (i) out << ';';
        out << p.id << ',' << p.x << ',' << p.y << ',' << p.hp << ',' << p.maxHp << ','
            << p.level << ',' << p.xp << ',' << p.gold << ',' << p.potions << ',' << p.classId << ','
            << (p.alive ? 1 : 0) << ',' << p.name << ',' << p.mana << ',' << p.maxMana << ','
            << p.facingDx << ',' << p.facingDy << ',' << itemToString(p.equipment[SLOT_WEAPON]) << ','
            << itemToString(p.equipment[SLOT_ARMOR]) << ',' << itemToString(p.equipment[SLOT_CHARM]) << ','
            << static_cast<int>(p.skillCooldown * 1000.0f) << ',';
        if (p.inventory.empty()) out << '-';
        else for (std::size_t j = 0; j < p.inventory.size(); ++j) {
            if (j) out << '/';
            out << itemToString(p.inventory[j]);
        }
    }
    out << '|';
    bool first = true;
    for (std::size_t i = 0; i < monsters_.size(); ++i) {
        const Monster& m = monsters_[i]; if (!m.alive) continue;
        if (!first) out << ';';
        first = false;
        out << m.id << ',' << m.x << ',' << m.y << ',' << m.hp << ',' << m.maxHp << ',' << m.type;
    }
    out << '|'; first = true;
    for (std::size_t i = 0; i < loot_.size(); ++i) {
        const Loot& l = loot_[i]; if (!l.active) continue;
        if (!first) out << ';';
        first = false;
        out << l.id << ',' << l.x << ',' << l.y << ',' << l.type << ',' << l.amount << ','
            << l.itemType << ',' << l.rarity << ',' << l.power;
    }
    output = out.str();
    return true;
}

bool World::deserializeSnapshot(const std::string& input) {
    std::string line = input;
    while (!line.empty() && (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r')) line.erase(line.size() - 1);
    if (line.size() > 512 * 1024) return false;
    std::vector<std::string> f = split(line, '|');
    if (f.size() != 7 || f[0] != "SNAP" || f[1] != "2") return false;
    int parsedTick = 0, parsedSeedInt = 0;
    if (!parseNumber(f[2], parsedTick) || parsedTick < 0 || !parseNumber(f[3], parsedSeedInt) || parsedSeedInt <= 0) return false;
    const uint32_t parsedSeed = static_cast<uint32_t>(parsedSeedInt);
    std::vector<Player> parsedPlayers;
    std::vector<Monster> parsedMonsters;
    std::vector<Loot> parsedLoot;
    std::vector<std::string> rows = split(f[4], ';');
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].empty()) continue;
        std::vector<std::string> v = split(rows[i], ',');
        if (v.size() != 21 || parsedPlayers.size() >= MAX_PLAYERS) return false;
        int values[16];
        for (int j = 0; j <= 10; ++j) if (!parseNumber(v[static_cast<std::size_t>(j)], values[j])) return false;
        for (int j = 12; j <= 15; ++j) if (!parseNumber(v[static_cast<std::size_t>(j)], values[j])) return false;
        Player p;
        p.id=values[0]; p.x=values[1]; p.y=values[2]; p.hp=values[3]; p.maxHp=values[4];
        p.level=values[5]; p.xp=values[6]; p.gold=values[7]; p.potions=values[8]; p.classId=values[9];
        p.alive=values[10] != 0; p.name=v[11]; p.mana=values[12]; p.maxMana=values[13];
        p.facingDx=values[14]; p.facingDy=values[15];
        if (p.id < 0 || p.x < 0 || p.x >= WORLD_W || p.y < 0 || p.y >= WORLD_H ||
            p.hp < 0 || p.maxHp < 1 || p.hp > p.maxHp || p.level < 1 || p.level > 10000 ||
            p.classId < 0 || p.classId > 3 || p.mana < 0 || p.maxMana < 1 || p.mana > p.maxMana ||
            (std::abs(p.facingDx) + std::abs(p.facingDy) != 1)) return false;
        if (!parseItem(v[16], p.equipment[SLOT_WEAPON]) || !parseItem(v[17], p.equipment[SLOT_ARMOR]) ||
            !parseItem(v[18], p.equipment[SLOT_CHARM])) return false;
        if ((p.equipment[SLOT_WEAPON].valid() && itemSlot(p.equipment[SLOT_WEAPON].type) != SLOT_WEAPON) ||
            (p.equipment[SLOT_ARMOR].valid() && itemSlot(p.equipment[SLOT_ARMOR].type) != SLOT_ARMOR) ||
            (p.equipment[SLOT_CHARM].valid() && itemSlot(p.equipment[SLOT_CHARM].type) != SLOT_CHARM)) return false;
        int cooldownMs = 0;
        if (!parseNumber(v[19], cooldownMs) || cooldownMs < 0 || cooldownMs > 60000) return false;
        p.skillCooldown = cooldownMs / 1000.0f;
        if (v[20] != "-") {
            std::vector<std::string> items = split(v[20], '/');
            if (items.size() > INVENTORY_CAPACITY) return false;
            for (std::size_t k = 0; k < items.size(); ++k) {
                Item item;
                if (!parseItem(items[k], item)) return false;
                p.inventory.push_back(item);
            }
        }
        p.inputDx = p.inputDy = 0; p.moveTimer = 0.0f; p.manaRegenTimer = 0.0f;
        parsedPlayers.push_back(p);
    }
    rows = split(f[5], ';');
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].empty()) continue;
        std::vector<std::string> v = split(rows[i], ','); if (v.size() != 6) return false;
        int a[6]; for (int j = 0; j < 6; ++j) if (!parseNumber(v[static_cast<std::size_t>(j)], a[j])) return false;
        Monster m; m.id=a[0]; m.x=a[1]; m.y=a[2]; m.hp=a[3]; m.maxHp=a[4]; m.type=a[5];
        if (m.id < 0 || m.x < 0 || m.x >= WORLD_W || m.y < 0 || m.y >= WORLD_H ||
            m.hp < 1 || m.maxHp < m.hp || m.type < MOB_SLIME || m.type > MOB_WOLF || parsedMonsters.size() >= 4096) return false;
        m.alive = true; parsedMonsters.push_back(m);
    }
    rows = split(f[6], ';');
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].empty()) continue;
        std::vector<std::string> v = split(rows[i], ','); if (v.size() != 8) return false;
        int a[8]; for (int j = 0; j < 8; ++j) if (!parseNumber(v[static_cast<std::size_t>(j)], a[j])) return false;
        Loot l; l.id=a[0]; l.x=a[1]; l.y=a[2]; l.type=a[3]; l.amount=a[4]; l.itemType=a[5]; l.rarity=a[6]; l.power=a[7];
        if (l.id < 0 || l.x < 0 || l.x >= WORLD_W || l.y < 0 || l.y >= WORLD_H || l.type < LOOT_GOLD ||
            l.type > LOOT_EQUIPMENT || l.amount < 1 || l.rarity < RARITY_COMMON || l.rarity > RARITY_LEGENDARY ||
            l.power < 0 || (l.type == LOOT_EQUIPMENT && itemSlot(l.itemType) < 0) || parsedLoot.size() >= 4096) return false;
        l.active = true; parsedLoot.push_back(l);
    }
    // Commit only after the entire packet has passed validation.
    if (seed_ != parsedSeed) generate(parsedSeed);
    tick_ = parsedTick;
    players_.swap(parsedPlayers); monsters_.swap(parsedMonsters); loot_.swap(parsedLoot);
    nextMonsterId_ = 1; nextLootId_ = 1;
    for (std::size_t i = 0; i < monsters_.size(); ++i) nextMonsterId_ = std::max(nextMonsterId_, monsters_[i].id + 1);
    for (std::size_t i = 0; i < loot_.size(); ++i) nextLootId_ = std::max(nextLootId_, loot_[i].id + 1);
    return true;
}

} // namespace nbapp
