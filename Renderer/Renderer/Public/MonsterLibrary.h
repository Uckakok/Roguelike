#pragma once
#include "Defines.h"

#define MONSTERFILE "Resources/monsters.mon"
#define GETMONSTERKEY(x) MonsterManager::GetInstance()->GetData(x).LocalizationKey

struct MonsterData 
{
    EntityTypes Type;
    std::string LocalizationKey;
    int Damage;
    int MaxHP;
    int Weight;
    int Speed;
};

inline Entity ToEntity(MonsterData Mon) 
{
    return Entity(Mon.Type, Position(), Mon.MaxHP, Mon.MaxHP, Mon.Damage, Mon.Speed);
}

class MonsterManager
{
private:
    MonsterManager() = default;
    ~MonsterManager() = default;
    MonsterManager(const MonsterManager&) = delete;
    MonsterManager& operator=(const MonsterManager&) = delete;

    std::unordered_map<int, MonsterData> m_monsterLibrary;
    static MonsterManager* m_instance;

public:
    void ReadMonsterDataFromFile();
    static MonsterManager* GetInstance();
    Entity GetMonster(EntityTypes Key) const;
    MonsterData GetData(EntityTypes Key) const;
    MonsterData GetRandMonsterData();
};