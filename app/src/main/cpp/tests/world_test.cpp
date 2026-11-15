#include "../game/world.h"
#include <cassert>
#include <iostream>

int main() {
    nbapp::World world;
    world.generate(0x12345678u);
    assert(world.seed() == 0x12345678u);
    assert(world.players().empty());
    assert(world.monsters().size() > 20);
    int biomeMask = 0;
    for (int by = 12; by < nbapp::WORLD_H; by += 24)
        for (int bx = 12; bx < nbapp::WORLD_W; bx += 24) {
            const int biome = world.biomeAt(bx, by);
            assert(biome >= nbapp::BIOME_MEADOW && biome <= nbapp::BIOME_RUINS);
            biomeMask |= 1 << biome;
        }
    int biomeCount = 0;
    for (int bit = 0; bit < 6; ++bit) if (biomeMask & (1 << bit)) ++biomeCount;
    assert(biomeCount >= 4);
    bool hasBoss = false;
    bool hasNewMonster = false;
    for (std::size_t i = 0; i < world.monsters().size(); ++i) {
        const nbapp::Monster& monster = world.monsters()[i];
        if (monster.type == nbapp::MOB_BOSS) {
            hasBoss = true;
            assert(monster.maxHp == 180);
        }
        if (monster.type >= nbapp::MOB_SPIDER && monster.type <= nbapp::MOB_WRAITH)
            hasNewMonster = true;
    }
    assert(hasBoss);
    assert(hasNewMonster);

    assert(world.addPlayer(1, "Player", nbapp::CLASS_WARRIOR));
    const nbapp::Player* original = world.findPlayer(1);
    assert(original != 0);
    assert(original->equipment[nbapp::SLOT_WEAPON].type == nbapp::ITEM_SWORD);
    assert(original->equipment[nbapp::SLOT_ARMOR].type == nbapp::ITEM_CHAIN);
    const int oldX = original->x, oldY = original->y;
    world.setInput(1, nbapp::InputState(1, 0, false));
    world.update(0.20f);
    const nbapp::Player* moved = world.findPlayer(1);
    assert(moved != 0 && moved->x == oldX + 1 && moved->y == oldY);

    world.setInput(1, nbapp::InputState(0, 0, false));
    nbapp::Player* player = world.findPlayer(1);
    const int hpBeforeCharm = player->maxHp;
    const int manaBeforeCharm = player->maxMana;
    player->inventory.push_back(nbapp::Item(nbapp::ITEM_AMULET, nbapp::RARITY_RARE, 3));
    assert(world.equipInventoryItem(1, 0));
    assert(player->equipment[nbapp::SLOT_CHARM].type == nbapp::ITEM_AMULET);
    assert(player->maxHp == hpBeforeCharm + 9);
    assert(player->maxMana == manaBeforeCharm + 15);
    assert(player->inventory.empty());
    player->inventory.push_back(nbapp::Item(nbapp::ITEM_BOW, nbapp::RARITY_UNCOMMON, 3));
    assert(world.equipInventoryItem(1, 0));
    assert(player->equipment[nbapp::SLOT_WEAPON].type == nbapp::ITEM_BOW);
    assert(player->inventory.size() == 1);
    assert(player->inventory[0].type == nbapp::ITEM_SWORD);

    // A class skill consumes mana and damages a target in its facing line.
    nbapp::World skillWorld;
    skillWorld.generate(9191);
    assert(skillWorld.addPlayer(77, "Mage", nbapp::CLASS_MAGE));
    nbapp::Player* mage = skillWorld.findPlayer(77);
    bool positioned = false;
    for (std::size_t i = 0; i < skillWorld.monsters().size(); ++i) {
        const nbapp::Monster& monster = skillWorld.monsters()[i];
        if (monster.x > 1 && skillWorld.isWalkable(monster.x - 1, monster.y)) {
            mage->x = monster.x - 1; mage->y = monster.y; mage->facingDx = 1; mage->facingDy = 0;
            positioned = true; break;
        }
    }
    assert(positioned);
    const int manaBeforeSkill = mage->mana;
    int monsterHpBefore = 0;
    for (std::size_t i = 0; i < skillWorld.monsters().size(); ++i) monsterHpBefore += skillWorld.monsters()[i].hp;
    assert(skillWorld.useClassSkill(77));
    int monsterHpAfter = 0;
    for (std::size_t i = 0; i < skillWorld.monsters().size(); ++i) monsterHpAfter += skillWorld.monsters()[i].hp;
    assert(mage->mana < manaBeforeSkill);
    assert(mage->skillCooldown > 0.0f);
    assert(monsterHpAfter < monsterHpBefore);

    // The map guardian drops a guaranteed epic item, gold and potions.
    nbapp::World bossWorld;
    bossWorld.generate(0x7654321u);
    assert(bossWorld.addPlayer(90, "BossTest", nbapp::CLASS_WARRIOR));
    nbapp::Monster* boss = 0;
    std::vector<nbapp::Monster>& mutableBosses = const_cast<std::vector<nbapp::Monster>&>(bossWorld.monsters());
    for (std::size_t i = 0; i < mutableBosses.size(); ++i)
        if (mutableBosses[i].type == nbapp::MOB_BOSS) { boss = &mutableBosses[i]; break; }
    assert(boss != 0);
    nbapp::Player* bossTester = bossWorld.findPlayer(90);
    bool adjacentFound = false;
    const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    for (int d = 0; d < 4 && !adjacentFound; ++d) {
        int px = boss->x + dirs[d][0], py = boss->y + dirs[d][1];
        if (!bossWorld.isWalkable(px, py)) continue;
        bool occupied = false;
        for (std::size_t j = 0; j < mutableBosses.size(); ++j)
            if (mutableBosses[j].alive && mutableBosses[j].x == px && mutableBosses[j].y == py) occupied = true;
        if (!occupied) {
            bossTester->x = px; bossTester->y = py;
            bossTester->facingDx = -dirs[d][0]; bossTester->facingDy = -dirs[d][1];
            adjacentFound = true;
        }
    }
    assert(adjacentFound);
    boss->hp = 1;
    assert(bossWorld.useClassSkill(90));
    bool epicReward = false, goldReward = false, potionReward = false;
    for (std::size_t i = 0; i < bossWorld.loot().size(); ++i) {
        const nbapp::Loot& item = bossWorld.loot()[i];
        if (!item.active) continue;
        if (item.type == nbapp::LOOT_EQUIPMENT && item.rarity == nbapp::RARITY_EPIC) epicReward = true;
        if (item.type == nbapp::LOOT_GOLD && item.amount >= 80) goldReward = true;
        if (item.type == nbapp::LOOT_POTION && item.amount >= 3) potionReward = true;
    }
    assert(epicReward && goldReward && potionReward);

    // A shaman should attack a player from range when the row is unobstructed.
    nbapp::World rangedWorld;
    rangedWorld.generate(0x12345678u);
    assert(rangedWorld.addPlayer(91, "RangedTest", nbapp::CLASS_WARRIOR));
    std::vector<nbapp::Monster>& mutableRanged = const_cast<std::vector<nbapp::Monster>&>(rangedWorld.monsters());
    nbapp::Monster shaman;
    bool shamanFound = false;
    for (std::size_t i = 0; i < mutableRanged.size() && !shamanFound; ++i) {
        if (mutableRanged[i].type != nbapp::MOB_SHAMAN) continue;
        const int candidates[2] = {1, -1};
        for (int c = 0; c < 2 && !shamanFound; ++c) {
            const int px = mutableRanged[i].x + candidates[c] * 3;
            const int py = mutableRanged[i].y;
            if (px < 1 || px >= nbapp::WORLD_W - 1 || !rangedWorld.isWalkable(px, py)) continue;
            bool blocked = false;
            for (int step = 1; step <= 3; ++step)
                if (!rangedWorld.isWalkable(mutableRanged[i].x + candidates[c] * step, py)) blocked = true;
            if (!blocked) {
                shaman = mutableRanged[i];
                nbapp::Player* rangedPlayer = rangedWorld.findPlayer(91);
                rangedPlayer->x = px; rangedPlayer->y = py;
                shamanFound = true;
            }
        }
    }
    assert(shamanFound);
    mutableRanged.clear();
    shaman.moveTimer = 0.0f; shaman.attackTimer = 0.0f;
    mutableRanged.push_back(shaman);
    const int hpBeforeShaman = rangedWorld.findPlayer(91)->hp;
    for (int i = 0; i < 12; ++i) rangedWorld.update(0.20f);
    assert(rangedWorld.findPlayer(91)->hp < hpBeforeShaman);

    for (int i = 2; i <= nbapp::MAX_PLAYERS; ++i)
        assert(world.addPlayer(i, "Player", i % 4));
    assert(world.players().size() == 8);
    assert(!world.addPlayer(9, "Overflow", 0));

    std::string snapshot;
    assert(world.serializeSnapshot(snapshot));
    nbapp::World restored;
    assert(restored.deserializeSnapshot(snapshot));
    assert(restored.seed() == world.seed());
    assert(restored.players().size() == world.players().size());
    assert(restored.monsters().size() == world.monsters().size());
    assert(restored.loot().size() == world.loot().size());
    assert(restored.findPlayer(1)->name == world.findPlayer(1)->name);
    assert(restored.findPlayer(1)->x == world.findPlayer(1)->x);
    assert(restored.findPlayer(1)->equipment[nbapp::SLOT_CHARM].type == nbapp::ITEM_AMULET);
    assert(restored.findPlayer(1)->inventory.size() == world.findPlayer(1)->inventory.size());
    assert(restored.biomeAt(12, 12) == world.biomeAt(12, 12));
    bool restoredBoss = false;
    for (std::size_t i = 0; i < restored.monsters().size(); ++i)
        if (restored.monsters()[i].type == nbapp::MOB_BOSS) restoredBoss = true;
    assert(restoredBoss);
    const uint32_t stableSeed = restored.seed();
    const std::size_t stablePlayers = restored.players().size();
    assert(!restored.deserializeSnapshot("SNAP|2|0|999|not-a-player|||"));
    assert(restored.seed() == stableSeed);
    assert(restored.players().size() == stablePlayers);

    std::cout << "world_test passed: 8 players, 6 biomes, 12 monster types, boss, "
              << "equipment, class skill, snapshot v2, " << restored.monsters().size() << " monsters\n";
    return 0;
}
