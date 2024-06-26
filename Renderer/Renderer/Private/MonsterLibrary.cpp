#include "pch.h"
#include "MonsterLibrary.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <unordered_map>

MonsterManager* MonsterManager::m_instance = nullptr;

void MonsterManager::ReadMonsterDataFromFile()
{
    std::ifstream File(MONSTERFILE);
    if (!File.is_open()) 
    {
        MessageBox(nullptr, L"Failed to load monster library", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    std::string Line;
    int Count = 0;
    while (std::getline(File, Line))
    {
        MonsterData NewMonsterEntry;
        std::istringstream Iss(Line);

        int Type;
        if (!(Iss >> Type >> NewMonsterEntry.LocalizationKey >> NewMonsterEntry.Damage >> NewMonsterEntry.MaxHP >> NewMonsterEntry.Speed >> NewMonsterEntry.Weight))
        {
            MessageBox(nullptr, L"Incorrectly formatten monsters.mon file", L"Error", MB_OK | MB_ICONERROR);
            continue;
        }
        Count++;
        NewMonsterEntry.Type = static_cast<EntityTypes>(Type);

        m_monsterLibrary.emplace(NewMonsterEntry.Type, NewMonsterEntry);
    }

    File.close();
    return;
}

MonsterManager* MonsterManager::GetInstance()
{
    if (!m_instance) 
    {
        m_instance = new MonsterManager();
    }
    return m_instance;
}

Entity MonsterManager::GetMonster(EntityTypes Key) const
{
    auto Iterator = m_monsterLibrary.find(Key);
    if (Iterator != m_monsterLibrary.end())
    {
        return ToEntity(Iterator->second);
    }
    else
    {
        MessageBox(nullptr, L"Requested monster not present in monsters.mon file", L"Error", MB_OK | MB_ICONERROR);
        return Entity();
    }
}

MonsterData MonsterManager::GetData(EntityTypes Key) const
{
    auto Iterator = m_monsterLibrary.find(Key);
    if (Iterator != m_monsterLibrary.end())
    {
        return Iterator->second;
    }
    else 
    {
        MessageBox(nullptr, L"Requested monster not present in monsters.mon file", L"Error", MB_OK | MB_ICONERROR);
        return MonsterData();
    }
}

MonsterData MonsterManager::GetRandMonsterData()
{
    return m_monsterLibrary[rand() % (m_monsterLibrary.size() - 1) + 1];
}
