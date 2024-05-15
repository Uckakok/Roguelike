#include "pch.h"
#include "MonsterLibrary.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <unordered_map>

MonsterManager* MonsterManager::instance = nullptr;

void MonsterManager::ReadMonsterDataFromFile()
{
    std::ifstream file(MONSTERFILE);
    if (!file.is_open()) {
        MessageBox(nullptr, L"Failed to load monster library", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    std::string line;
    int Count = 0;
    while (std::getline(file, line)) {
        MonsterData NewMonsterEntry;
        std::istringstream iss(line);

        int Type;

        if (!(iss >> Type >> NewMonsterEntry.LocalizationKey >> NewMonsterEntry.Damage >> NewMonsterEntry.MaxHP >> NewMonsterEntry.Weight)) {
            MessageBox(nullptr, L"Incorrectly formatten monsters.mon file", L"Error", MB_OK | MB_ICONERROR);
            continue;
        }
        Count++;
        NewMonsterEntry.Type = static_cast<EntityTypes>(Type);

        MonsterLibrary.emplace(NewMonsterEntry.Type, NewMonsterEntry);
    }

    file.close();
    return;
}

MonsterManager* MonsterManager::GetInstance()
{
    if (!instance) {
        instance = new MonsterManager();
    }
    return instance;
}

Entity MonsterManager::GetMonster(EntityTypes Key)
{
    auto it = MonsterLibrary.find(Key);
    if (it != MonsterLibrary.end()) {
        return ToEntity(it->second);
    }
    else {
        MessageBox(nullptr, L"Requested monster not present in monsters.mon file", L"Error", MB_OK | MB_ICONERROR);

        return Entity();
    }
}

MonsterData MonsterManager::GetData(EntityTypes Key)
{
    auto it = MonsterLibrary.find(Key);
    if (it != MonsterLibrary.end()) {
        return it->second;
    }
    else {
        MessageBox(nullptr, L"Requested monster not present in monsters.mon file", L"Error", MB_OK | MB_ICONERROR);

        return MonsterData();
    }
}

MonsterData MonsterManager::GetRandMonsterData()
{
    return MonsterLibrary[rand() % (MonsterLibrary.size() - 1) + 1];
}
