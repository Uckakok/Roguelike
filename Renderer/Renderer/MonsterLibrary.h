#pragma once
#include "Defines.h"

#define MONSTERFILE "Resources/monsters.mon"

#define GETMONSTERKEY(x) MonsterManager::GetInstance()->GetData(x).LocalizationKey

struct MonsterData {
    EntityTypes Type;
    std::string LocalizationKey;
    int Damage;
    int MaxHP;
    int Weight;
};

inline Entity ToEntity(MonsterData Mon) {
    return Entity(Mon.Type, Position(), Mon.MaxHP, Mon.MaxHP, Mon.Damage);
}

class MonsterManager
{
private:
    std::unordered_map<int, MonsterData> MonsterLibrary;
    MonsterManager() = default;
    ~MonsterManager() = default;

    MonsterManager(const MonsterManager&) = delete;

    // Private assignment operator to prevent assignment
    MonsterManager& operator=(const MonsterManager&) = delete;

    // Static instance of the class
    static MonsterManager* instance;

public:
    void ReadMonsterDataFromFile();
    static MonsterManager* GetInstance();
    Entity GetMonster(EntityTypes Key);
    MonsterData GetData(EntityTypes Key);
    MonsterData GetRandMonsterData();
};