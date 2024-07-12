#include "pch.h"
#include "Level.h"
#include <queue>
#include <cmath>
#include "LocalizationManager.h"
#include "GameEngine.h"
#include "MonsterLibrary.h"

void DungeonLevel::UseCurrentObject()
{
    Position PlayerPos = GetPlayerPosition();
    if (m_levelMap[PlayerPos.X][PlayerPos.Y].CurrentItem) 
    {
        UseItem(m_levelMap[PlayerPos.X][PlayerPos.Y].CurrentItem);
        return;
    }
    if (m_levelMap[PlayerPos.X][PlayerPos.Y].Arch == Architecture::StairsDownTile) 
    {
        GameEngine::GetInstance()->AppendLogger(LOCALIZED_TEXT("climb_downwards"));
        GameEngine::GetInstance()->LoadLevel(LevelIndex + 1);
        return;
    }
    if (m_levelMap[PlayerPos.X][PlayerPos.Y].Arch == Architecture::StairsUpTile && LevelIndex != 1) 
    {
        GameEngine::GetInstance()->AppendLogger(LOCALIZED_TEXT("climb_upwards"));
        GameEngine::GetInstance()->LoadLevel(LevelIndex - 1);
        return;
    }
}

void DungeonLevel::SpawnPlayer(bool bFromUp, Entity* Player)
{
    if (GetPlayer())
    {
        MessageBox(nullptr, L"Player is already present on this level!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    auto NewPlayer = std::make_unique<Entity>(*Player);

    if (bFromUp) 
    {
        for (int i = 0; i < m_declaredBoardSize; ++i) 
        {
            for (int j = 0; j < m_declaredBoardSize; ++j) 
            {
                if (m_levelMap[i][j].Arch == Architecture::StairsUpTile) 
                {
                    NewPlayer->Location = Position(i, j);
                    m_entitiesOnLevel.push_back(std::move(NewPlayer));
                    m_levelMap[i][j].Entity = m_entitiesOnLevel.back().get(); 
                    m_monsterQueue[0].insert(m_monsterQueue[0].begin(), m_entitiesOnLevel.back().get());
                    return;
                }
            }
        }
    }
    else 
    {
        for (int i = 0; i < m_declaredBoardSize; ++i)
        {
            for (int j = 0; j < m_declaredBoardSize; ++j) 
            {
                if (m_levelMap[i][j].Arch == Architecture::StairsDownTile) 
                {
                    NewPlayer->Location = Position(i, j);
                    m_entitiesOnLevel.push_back(std::move(NewPlayer));
                    m_levelMap[i][j].Entity = m_entitiesOnLevel.back().get();
                    m_monsterQueue[0].insert(m_monsterQueue[0].begin(), m_entitiesOnLevel.back().get());
                    return;
                }
            }
        }
    }
    MessageBox(nullptr, L"didn't find a valid localization to spawn a player!", L"Error", MB_OK | MB_ICONERROR);
}

bool DungeonLevel::LoadMapFromSave(const std::string& SaveName)
{

    std::ifstream File;
    if (LevelIndex == 25) 
    {
        File = std::ifstream("Resources/Level25.Rog");
        if (!File.is_open()) 
        {
            MessageBox(nullptr, L"Couldn't load Level25.Rog blueprint. Verify your installation", L"Error", MB_OK | MB_ICONERROR);
            return false;
        }
        MessageBox(nullptr, LOCALIZED_TEXT("win_dialogue"), LOCALIZED_TEXT("won"), MB_OK | MB_ICONEXCLAMATION);
        m_bIsGameWon = true;
    }
    else 
    {
        File = std::ifstream(SaveName);
        if (!File.is_open()) 
        {
            return false;
        }
    }

    m_monsterQueue.clear();
    m_entitiesOnLevel.clear();
    m_itemsOnLevel.clear();

    std::vector<std::vector<LevelTile>> NewLevelMap;

    std::string Line;
    int BoardSize = 0;
    int CurrentSize = -1;
    while (std::getline(File, Line) && !Line.empty()) 
    {
        int LineSize = 0;
        std::vector<LevelTile> Row;
        std::istringstream Iss(Line);
        int Value;
        while (Iss >> Value) 
        {
            LevelTile Tile;
            Tile.Arch = static_cast<Architecture>(Value);
            Row.push_back(Tile);
            LineSize++;
        }
        if (CurrentSize == -1) 
        {
            CurrentSize = LineSize;
        }
        else if (CurrentSize != LineSize)
        {
            MessageBox(nullptr, L"Corrupted level data!", L"Error", MB_OK | MB_ICONERROR);
            return false;
        }
        BoardSize++;
        NewLevelMap.push_back(Row);
    }

    if (BoardSize != CurrentSize) {
        MessageBox(nullptr, L"Corrupted level data!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_declaredBoardSize = BoardSize;

    int EntityId, X, Y, Hp, MaxHP, Damage, Speed;
    while (std::getline(File, Line)) 
    {
        if (Line.empty()) 
        {
            break;
        }

        std::istringstream Iss(Line);
        Iss >> EntityId >> X >> Y >> Hp >> MaxHP >> Damage >> Speed;

        if (EntityId != 0) 
        {
            auto newEntity = std::make_unique<Entity>(static_cast<EntityTypes>(EntityId), Position(X, Y), Hp, MaxHP, Damage, Speed);
            m_entitiesOnLevel.push_back(std::move(newEntity));
        }
    }

    int ItemId, BonusHP, BonusDamage, BonusPermaHP;
    while (File >> ItemId >> X >> Y >> BonusHP >> BonusDamage >> BonusPermaHP) 
    {
        if (ItemId != 0) 
        {
            auto newItem = std::make_unique<Item>(static_cast<ItemTypes>(ItemId), Position(X, Y), BonusHP, BonusDamage, BonusPermaHP);
            m_itemsOnLevel.push_back(std::move(newItem));
        }
    }

    for (auto& EntityOnLevel : m_entitiesOnLevel) 
    {
        NewLevelMap[EntityOnLevel->Location.X][EntityOnLevel->Location.Y].Entity = EntityOnLevel.get();
    }

    for (auto& ItemOnLevel : m_itemsOnLevel) 
    {
        NewLevelMap[ItemOnLevel->Location.X][ItemOnLevel->Location.Y].CurrentItem = ItemOnLevel.get();
    }

    std::vector<Entity*> AllEnts;

    for (auto& EntityOnLevel : m_entitiesOnLevel) 
    {
        if (EntityOnLevel->IsPlayer()) continue;
        AllEnts.push_back(EntityOnLevel.get());
    }
    if (GetPlayer()) 
    {
        AllEnts.insert(AllEnts.begin(), GetPlayer());
    }

    m_monsterQueue.push_back(AllEnts);
    for (const auto& Turn : m_monsterQueue) 
    {
        for (const auto& EntityPtr : Turn) 
        {
            if (EntityPtr == nullptr) 
            {
                MessageBox(nullptr, L"Null reference found in MonsterQueue!", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
        }
    }

    m_levelMap = std::move(NewLevelMap);

    return true;
}

void DungeonLevel::UseItem(Item* UsedItem)
{
    //apply buffs
    GameEngine::GetInstance()->AppendLogger(FORMAT(LOCALIZED_WSTRING("used_item"), ToString(UsedItem->Type)));
    Entity* Player = GetPlayer();
    if (UsedItem->BonusPermaHP != 0)
    {
        Player->AddMaxHP(UsedItem->BonusPermaHP);
        GameEngine::GetInstance()->AppendLogger(FORMAT(LOCALIZED_WSTRING("gained_max_hp"), UsedItem->BonusPermaHP));
    }
    if (UsedItem->BonusHP != 0) 
    {
        Player->Heal(UsedItem->BonusHP);
        GameEngine::GetInstance()->AppendLogger(FORMAT(LOCALIZED_WSTRING("healed"), UsedItem->BonusHP));
    }
    if (UsedItem->BonusDamage != 0) 
    {
        Player->AddDamage(UsedItem->BonusDamage);
        GameEngine::GetInstance()->AppendLogger(FORMAT(LOCALIZED_WSTRING("gained_damage"), UsedItem->BonusDamage));
    }

    //delete this item
    //set the item pointer in the LevelMap to nullptr
    m_levelMap[UsedItem->Location.X][UsedItem->Location.Y].CurrentItem = nullptr;

    for (size_t i = 0; i < m_itemsOnLevel.size(); ++i)
    {
        if (m_itemsOnLevel[i]->Location == UsedItem->Location)
        {
            m_itemsOnLevel.erase(m_itemsOnLevel.begin() + i);
            break;
        }
    }
}

void DungeonLevel::PutInQueue(size_t PositionOffset, Entity* EntityToAdd)
{
    if (!EntityToAdd) 
    {
        MessageBox(nullptr, L"Entity is null", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    while (m_monsterQueue.size() <= PositionOffset) 
    {
        std::vector<Entity*> NewTurn;
        m_monsterQueue.push_back(NewTurn);
    }
    if (EntityToAdd->IsPlayer()) 
    {
        m_monsterQueue[PositionOffset].insert(m_monsterQueue[PositionOffset].begin(), EntityToAdd);
    }
    else 
    {
        m_monsterQueue[PositionOffset].push_back(EntityToAdd);
    }
}

void DungeonLevel::ConnectAreas(std::vector<std::vector<Position>>& Areas) 
{
    // Iterate over all pairs of areas
    std::vector<Position> Tunnels;
    for (size_t i = 0; i < Areas.size(); ++i) 
    {
        for (size_t j = i + 1; j < Areas.size(); ++j)
        {
            // Find the shortest path between the current pair of areas
            std::vector<Position> ShortestPath = GetPath(Areas[i][0], Areas[j][0], true);
            for (size_t i = 0; i < ShortestPath.size(); ++i) 
            {
                Tunnels.push_back(ShortestPath[i]);
            }
        }
    }

    for (auto& Pos : Tunnels) {
        m_levelMap[Pos.X][Pos.Y].Arch = Architecture::FloorTile;
    }
}

void DungeonLevel::GenerateMap()
{
    std::vector<std::vector<LevelTile>> GeneratedMap;

    //generate full wall map
    for (int i = 0; i < LEVEL_SIZE; ++i) 
    {
        std::vector<LevelTile> Row;
        for (int j = 0; j < LEVEL_SIZE; ++j) 
        {
            LevelTile NewTile;
            NewTile.Arch = Architecture::WallTile;
            Row.push_back(NewTile);
        }
        GeneratedMap.push_back(Row);
    }

    int NumRectangles = 24;
    int MaxSize = 6;
    int MinSize = 3;
    srand(time(0));

    //add random rectangles on the map
    for (int r = 0; r < NumRectangles; ++r) 
    {
        int StartX = rand() % (LEVEL_SIZE - MaxSize);
        int StartY = rand() % (LEVEL_SIZE - MaxSize);

        int Width = MinSize + rand() % (MaxSize - MinSize + 1);
        int Height = MinSize + rand() % (MaxSize - MinSize + 1);

        for (int i = StartX; i < StartX + Width; ++i) 
        {
            for (int j = StartY; j < StartY + Height; ++j) 
            {
                GeneratedMap[i][j].Arch = Architecture::FloorTile;
            }
        }
    }

    m_declaredBoardSize = LEVEL_SIZE;
    std::vector<std::vector<Position>> Areas;

    for (int i = 0; i < LEVEL_SIZE; ++i) 
    {
        for (int j = 0; j < LEVEL_SIZE; ++j) 
        {
            if (GeneratedMap[i][j].Arch == Architecture::FloorTile) 
            {
                Areas.push_back(FindAllConnected(Position(i, j), &GeneratedMap));
            }
        }
    }

    m_levelMap = std::move(GeneratedMap);
    ConnectAreas(Areas);

    //fix up the board after calculating areas
    for (int i = 0; i < LEVEL_SIZE; ++i) 
    {
        for (int j = 0; j < LEVEL_SIZE; ++j) 
        {
            if (m_levelMap[i][j].Arch == Architecture::Visited) 
            {
                m_levelMap[i][j].Arch = Architecture::FloorTile;
            }
        }
    }
    
    //gather all tiles
   std::vector<LevelTile*> ValidTilesForSpawning;
    for (int i = 0; i < LEVEL_SIZE; ++i) 
    {
        for (int j = 0; j < LEVEL_SIZE; ++j) 
        {
            if (m_levelMap[i][j].Arch == Architecture::FloorTile) 
            {
                m_levelMap[i][j].Coordinates = Position(i, j);
                ValidTilesForSpawning.push_back(&m_levelMap[i][j]);
            }
        }
    }

    //generate stairs
    ValidTilesForSpawning[rand() % ValidTilesForSpawning.size()]->Arch = Architecture::StairsDownTile;
    int Index = rand() % ValidTilesForSpawning.size();
    while (ValidTilesForSpawning[Index]->Arch == StairsDownTile) 
    {
        Index = rand() % ValidTilesForSpawning.size();
    }
    ValidTilesForSpawning[Index]->Arch = Architecture::StairsUpTile;

    m_monsterQueue.clear();
    m_entitiesOnLevel.clear();
    m_itemsOnLevel.clear();

    int EntityCount = rand() % 10 + 5;

    for (int i = 0; i < EntityCount; ++i) 
    {
        Index = rand() % ValidTilesForSpawning.size();
        if (ValidTilesForSpawning[Index]->Arch == Architecture::StairsUpTile || ValidTilesForSpawning[Index]->Entity) continue;
        //get random entity
        MonsterData NewMonster;
        NewMonster.Weight = 0;

        while (NewMonster.Weight == 0 || NewMonster.Weight > (LevelIndex * MONSTER_SPAWN_RANDOMIZER) + (rand() % MONSTER_SPAWN_RANDOMIZER)) 
        {
            NewMonster = MonsterManager::GetInstance()->GetRandMonsterData();
        }
        auto NewEntity = std::make_unique<Entity>(ToEntity(NewMonster));

        NewEntity->Location = ValidTilesForSpawning[Index]->Coordinates;
        m_entitiesOnLevel.push_back(std::move(NewEntity));
        ValidTilesForSpawning[Index]->Entity = m_entitiesOnLevel.back().get();
    }

    int ItemsCount = (rand() % 8) + 2;

    for (int i = 0; i < ItemsCount; ++i) 
    {
        Index = rand() % ValidTilesForSpawning.size();
        if (ValidTilesForSpawning[Index]->Arch == Architecture::StairsUpTile || ValidTilesForSpawning[Index]->CurrentItem) continue;
        //get random entity
        int ItemType = rand() % 4;
        auto NewItem = std::make_unique<Item>();
        switch (ItemType) 
        {
        case 0:
            NewItem->Type = ItemTypes::StrengthRuneItem;
            NewItem->BonusDamage = MAX(LevelIndex * STRENGTH_DEPTH_MULTIPLIER, 1);
            break;
        case 1:
            NewItem->Type = ItemTypes::VitalityRuneItem;
            NewItem->BonusPermaHP = MAX(LevelIndex * MAX_HP_DEPTH_MULTIPLIER, 10);
            break;
        default:
            NewItem->Type = ItemTypes::HealingPotionItem;
            NewItem->BonusHP = MAX(LevelIndex * HEALING_DEPTH_MULTIPLIER, 25);
        }
        NewItem->Location = ValidTilesForSpawning[Index]->Coordinates;

        m_itemsOnLevel.push_back(std::move(NewItem));
        ValidTilesForSpawning[Index]->CurrentItem = m_itemsOnLevel.back().get();
    }
    
    std::vector<Entity*> AllEnts;

    for (auto& EntityOnLevel : m_entitiesOnLevel) 
    {
        AllEnts.push_back(EntityOnLevel.get());
    }

    m_monsterQueue.push_back(AllEnts);
}


std::vector<Position> DungeonLevel::FindAllConnected(Position StartPos, std::vector<std::vector<LevelTile>>* NewMap)
{
    std::vector<Position> Connected;

    if (NewMap->at(StartPos.X)[StartPos.Y].Arch != Architecture::FloorTile) 
    {
        MessageBox(nullptr, L"Error when generating map!", L"Error", MB_OK | MB_ICONERROR);
        return Connected;
    }

    std::vector<Position> Directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    //perform BFS
    std::queue<Position> Queue;
    Queue.push(StartPos);

    NewMap->at(StartPos.X)[StartPos.Y].Arch = Visited;

    while (!Queue.empty()) 
    {
        Position CurrentPos = Queue.front();
        Queue.pop();

        Connected.push_back(CurrentPos);

        for (const auto& Dir : Directions) 
        {
            int NX = CurrentPos.X + Dir.X;
            int NY = CurrentPos.Y + Dir.Y;
            if (NX >= 0 && NX < NewMap->size() && NY >= 0 && NY < NewMap->at(0).size() && NewMap->at(NX)[NY].Arch == Architecture::FloorTile) 
            {
                NewMap->at(NX)[NY].Arch = Visited;
                Queue.push({ NX, NY });
            }
        }
    }

    return Connected;
}

void DungeonLevel::RemovePlayer()
{
    Entity* Player = GetPlayer();
    KillEntityOnPosition(Player->Location, false);
}

bool DungeonLevel::SaveMapToSave()
{
    std::string SaveName = GameEngine::GetInstance()->GetPlayerName();
    SaveName.append(std::to_string(LevelIndex));
    SaveName.append(EXTENSION);

    std::ofstream File(SaveName);
    if (!File.is_open()) 
    {
        std::wstring ErrorMessage = L"Failed to save dungeon data to file: " + std::wstring(SaveName.begin(), SaveName.end());
        MessageBox(nullptr, ErrorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Save the level map
    for (const auto& Row : m_levelMap) 
    {
        for (const auto& Tile : Row) 
        {
            File << static_cast<int>(Tile.Arch) << ' ';
        }
        File << std::endl;
    }

    File << std::endl;

    // Save the entities
    for (auto& EntityOnLevel : m_entitiesOnLevel) 
    {
        File << static_cast<int>(EntityOnLevel->Type) << ' ' << EntityOnLevel->Location.X << ' ' << EntityOnLevel->Location.Y << ' ' << EntityOnLevel->GetHP() << ' ' << EntityOnLevel->GetMaxHP() << ' ' << EntityOnLevel->GetDamage() << ' ' << EntityOnLevel->GetSpeed() << std::endl;
    }

    File << std::endl;

    // Save the items
    for (const auto& ItemOnLevel : m_itemsOnLevel) 
    {
        File << static_cast<int>(ItemOnLevel->Type) << ' ' << ItemOnLevel->Location.X << ' ' << ItemOnLevel->Location.Y << ' ' << ItemOnLevel->BonusHP << ' ' << ItemOnLevel->BonusDamage << ' ' << ItemOnLevel->BonusPermaHP << std::endl;
    }
    File.close();

    return true;
}

int DungeonLevel::GetDeclaredBoardSize() const
{
    return m_declaredBoardSize;
}

std::vector<TileToDraw> DungeonLevel::GatherTilesForRender()
{
    std::vector<TileToDraw> GatheredTiles;

    for (int i = 0; i < m_declaredBoardSize; ++i) 
    {
        for (int j = 0; j < m_declaredBoardSize; ++j) 
        {
            GatheredTiles.push_back(TileToDraw(i, j, ToTileType(m_levelMap[i][j].Arch)));
        }
    }

    return GatheredTiles;
}

std::vector<TileToDraw> DungeonLevel::GatherEntitiesForRender()
{
    std::vector<TileToDraw> GatheredTiles;

    for (auto& Element : m_itemsOnLevel)
    {
        if (Element->Type == ItemTypes::Empty)
        {
            continue;
        }
        if (Element->Type == ItemTypes::HealingPotionItem)
        {
            GatheredTiles.push_back(TileToDraw(Element->Location.X, Element->Location.Y, TileTypes::HealingPotion));
        }
        if (Element->Type == ItemTypes::VitalityRuneItem)
        {
            GatheredTiles.push_back(TileToDraw(Element->Location.X, Element->Location.Y, TileTypes::VitalityRune));
        }
        if (Element->Type == ItemTypes::StrengthRuneItem)
        {
            GatheredTiles.push_back(TileToDraw(Element->Location.X, Element->Location.Y, TileTypes::StrengthRune));
        }
    }

    for (auto& EntityOnLevel : m_entitiesOnLevel)
    {
        if (EntityOnLevel->Type == EntityTypes::None)
        {
            MessageBox(nullptr, L"Invalid entity present on level", L"Error", MB_OK | MB_ICONERROR);
            continue;
        }
        GatheredTiles.push_back(TileToDraw(EntityOnLevel->Location.X, EntityOnLevel->Location.Y, ToTiletype(EntityOnLevel->Type)));
    }

    //player died
    if (!GetPlayer()) return GatheredTiles;

    Position PlayerPos = GetPlayerPosition();

    //add highlighted tiles around the player
    const int Radius = 1;
    for (int DX = -Radius; DX <= Radius; ++DX)
    {
        for (int DY = -Radius; DY <= Radius; ++DY)
        {
            int NX = PlayerPos.X + DX;
            int NY = PlayerPos.Y + DY;
            if (NX == PlayerPos.X && NY == PlayerPos.Y) continue;
            if (NX >= 0 && NX < m_declaredBoardSize && NY >= 0 && NY < m_declaredBoardSize && m_levelMap[NX][NY].Arch != Architecture::WallTile)
            {
                GatheredTiles.push_back(TileToDraw(NX, NY, TileTypes::Highlight));
            }
        }
    }

    return GatheredTiles;
}

Position DungeonLevel::GetPlayerPosition() const
{
    if (GetPlayer()) 
    {
        return GetPlayer()->Location;
    }
    else
    {
        MessageBox(nullptr, L"Player was not found.", L"Error", MB_OK | MB_ICONERROR);
    }
    return Position(-1, -1);
}

bool DungeonLevel::PerformAction(Position PlayerMove)
{
    if (!IsMoveLegal(PlayerMove)) 
    {
        return false;
    }

    Entity* PlayerEntity = GetPlayer();

    if (PlayerEntity->Location == PlayerMove) 
    {
        //player waited
        PutInQueue(1, PlayerEntity);
        for (auto Iterator = m_monsterQueue.front().begin(); Iterator != m_monsterQueue.front().end(); ++Iterator) 
        {
            if (*Iterator == GetPlayer()) 
            {
                m_monsterQueue.front().erase(Iterator);
                break;
            }
        }
        return true;
    }

    Entity* EntityOnTile = GetEntityOnTile(PlayerMove);
    if (EntityOnTile) 
    {
        //perform attack
        EntityOnTile->ReceiveDamage(PlayerEntity->GetDamage());
        GameEngine::GetInstance()->AppendLogger(FORMAT(LOCALIZED_WSTRING("deal_damage_to").c_str(), PlayerEntity->GetDamage(), LOCALIZED_TEXT(GETMONSTERKEY(EntityOnTile->Type))));
        if (EntityOnTile->IsDead())
        {
            GameEngine::GetInstance()->AppendLogger(FORMAT(LOCALIZED_WSTRING("kill_who").c_str(), LOCALIZED_TEXT(GETMONSTERKEY(EntityOnTile->Type))));
            KillEntityOnPosition(PlayerMove);
        }
        PutInQueue(PlayerEntity->GetSpeed(), PlayerEntity);
        for (auto Iterator = m_monsterQueue.front().begin(); Iterator != m_monsterQueue.front().end(); ++Iterator) 
        {
            if (*Iterator == GetPlayer()) 
            {
                m_monsterQueue.front().erase(Iterator);
                break;
            }
        }
        return true;
    }

    m_levelMap[PlayerEntity->Location.X][PlayerEntity->Location.Y].Entity = nullptr;
    PlayerEntity->Location = PlayerMove;
    m_levelMap[PlayerEntity->Location.X][PlayerEntity->Location.Y].Entity = PlayerEntity;

    PutInQueue(PlayerEntity->GetSpeed(), PlayerEntity);
    for (auto Iterator = m_monsterQueue.front().begin(); Iterator != m_monsterQueue.front().end(); ++Iterator) 
    {
        if (*Iterator == GetPlayer()) 
        {
            m_monsterQueue.front().erase(Iterator);
            break;
        }
    }
    return true;
}

bool DungeonLevel::MoveEntity(Entity* EntityToMove)
{
    if (!EntityToMove) 
    {
        MessageBox(nullptr, L"Tried to move invalid entity", L"Error", MB_OK | MB_ICONERROR);
        //don't try to move it ever again
        return false;
    }

    std::vector<Position> PathToTake = GetPath(EntityToMove->Location, GetPlayerPosition());

    if (PathToTake.size() < 2)
    {
        // invalid path. No way to reach the goal=
        PutInQueue(EntityToMove->GetSpeed(), EntityToMove);
        return false;
    }

    Position NextMove = PathToTake[1];
    Entity* EntityOnTile = GetEntityOnTile(NextMove);

    if (EntityOnTile) 
    {
        //perform attack
        EntityOnTile->ReceiveDamage(EntityToMove->GetDamage());
        GameEngine::GetInstance()->AppendLogger(FORMAT(LOCALIZED_WSTRING("dealt_damage_to_you").c_str(), LOCALIZED_TEXT(GETMONSTERKEY(EntityToMove->Type)), EntityToMove->GetDamage()));
        if (EntityOnTile->IsDead()) 
        {
            if (EntityOnTile->IsPlayer()) 
            {
                KillEntityOnPosition(NextMove);
                MessageBox(nullptr, LOCALIZED_TEXT("you_died"), LOCALIZED_TEXT("game_over"), MB_OK | MB_ICONEXCLAMATION);
                m_bIsGameEnded = true;
                return true;
            }
        }

        PutInQueue(EntityToMove->GetSpeed(), EntityToMove);
        return true;
    }

    m_levelMap[EntityToMove->Location.X][EntityToMove->Location.Y].Entity = nullptr;
    EntityToMove->Location = NextMove;
    m_levelMap[EntityToMove->Location.X][EntityToMove->Location.Y].Entity = EntityToMove;

    PutInQueue(EntityToMove->GetSpeed(), EntityToMove);
    return true;
}

Entity* DungeonLevel::GetPlayer() const
{
    for (auto& EntityOnLevel : m_entitiesOnLevel)
    {
        if (EntityOnLevel->Type == EntityTypes::PlayerEntity)
        {
            return EntityOnLevel.get();
        }
    }

    return nullptr;
}

Entity* DungeonLevel::GetEntityOnTile(Position Location) const
{
    return m_levelMap[Location.X][Location.Y].Entity;
}


inline double CalculateDistance(const Position& PositionA, const Position& PositionB) 
{
    return std::abs(PositionA.X - PositionB.X) + std::abs(PositionA.Y - PositionB.Y);
}

inline bool IsValid(const Position& Pos, const std::vector<std::vector<LevelTile>>& LevelMap, bool bIgnoreAll) 
{
    if (Pos.X < 0 || Pos.X >= static_cast<int>(LevelMap.size()) || Pos.Y < 0 || Pos.Y >= static_cast<int>(LevelMap[0].size())) 
    {
        return false; //position is out of bounds
    }
    if (bIgnoreAll) return true;
    return LevelMap[Pos.X][Pos.Y].Arch != Architecture::WallTile && (!LevelMap[Pos.X][Pos.Y].Entity || LevelMap[Pos.X][Pos.Y].Entity->IsPlayer()); //position is not a wall and doesn't have entity
}

std::vector<Position> ReconstructPath(Node* Current) 
{
    std::vector<Position> Path;
    while (Current != nullptr) 
    {
        Path.push_back(Current->Pos);
        Current = Current->Parent;
    }
    std::reverse(Path.begin(), Path.end());
    return Path;
}

std::vector<Position> DungeonLevel::GetPath(Position Start, Position Goal, bool bIgnoreAll) const
{
    // Validate Goal
    if (!IsValid(Goal, m_levelMap, bIgnoreAll)) 
    {
        return {}; // Goal position is not valid
    }

    if (CalculateDistance(Start, Goal) > 11 && !bIgnoreAll)
    {
        return {}; //too far away. Don't waste resources
    }

    std::vector<Position> Directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    auto CompareNodes = [](Node* NodeA, Node* NodeB) { return NodeA->GetFCost() > NodeB->GetFCost(); };
    std::priority_queue<Node*, std::vector<Node*>, decltype(CompareNodes)> OpenSet(CompareNodes);

    std::vector<std::vector<bool>> Visited(m_levelMap.size(), std::vector<bool>(m_levelMap[0].size(), false));

    Node* StartNode = new Node(Start, 0.0, CalculateDistance(Start, Goal), nullptr);
    OpenSet.push(StartNode);

    while (!OpenSet.empty()) 
    {
        Node* Current = OpenSet.top();
        OpenSet.pop();

        if (Current->Pos == Goal) 
        {
            std::vector<Position> Path = ReconstructPath(Current);
            while (!OpenSet.empty()) 
            {
                delete OpenSet.top();
                OpenSet.pop();
            }
            return Path;
        }

        if (Current->GCost > 12 && !bIgnoreAll) //12 as maximum cost for a path. If it's farther away don't search
        {
            while (!OpenSet.empty()) 
            {
                delete OpenSet.top();
                OpenSet.pop();
            }
            return {}; //path length exceeds limit
        }

        Visited[Current->Pos.X][Current->Pos.Y] = true;

        for (const auto& Dir : Directions)
        {
            Position NeighborPos = { Current->Pos.X + Dir.X, Current->Pos.Y + Dir.Y };
            if (!IsValid(NeighborPos, m_levelMap, bIgnoreAll) || Visited[NeighborPos.X][NeighborPos.Y]) 
            {
                continue;
            }

            double NeighborGCost = Current->GCost + (Dir.X == 0 || Dir.Y == 0 ? 1.0 : 1.4); //give higher cost to diagonals so when tunnels are generated it will prefer to go in straight lines
            Node* NeighborNode = new Node(NeighborPos, NeighborGCost, CalculateDistance(NeighborPos, Goal), Current);
            OpenSet.push(NeighborNode);
        }
    }

    while (!OpenSet.empty()) 
    {
        delete OpenSet.top();
        OpenSet.pop();
    }

    return {};
}

void DungeonLevel::PerformEntitiesTurn()
{
    while (m_monsterQueue.front().empty() || !m_monsterQueue.front().front()->IsPlayer())
    {
        for (auto& EntityInQueue : m_monsterQueue.front()) 
        {
            if (EntityInQueue->IsPlayer()) continue; //or just crash. Shouldn't happen
            if (m_bIsGameEnded) break;

            MoveEntity(EntityInQueue);
        }
        if (m_bIsGameEnded) break;
        m_monsterQueue.erase(m_monsterQueue.begin());
    }
}

bool DungeonLevel::GetGameEnded() const
{
    return m_bIsGameEnded;
}

bool DungeonLevel::IsUseAvailable() const
{
    Position PlayerPos = GetPlayerPosition();
    if (m_levelMap[PlayerPos.X][PlayerPos.Y].Arch == Architecture::StairsDownTile || (m_levelMap[PlayerPos.X][PlayerPos.Y].Arch == Architecture::StairsUpTile && LevelIndex != 1)) 
    {
        return true;
    }
    if (m_levelMap[PlayerPos.X][PlayerPos.Y].CurrentItem)
    {
        return true;
    }
    return false;
}

bool DungeonLevel::GetGameWon() const
{
    return m_bIsGameWon;
}

HoverInfo DungeonLevel::ConstructHoverInfo(Position HoverPosition) const
{
    if (HoverPosition.X < 0 || HoverPosition.X >= static_cast<int>(m_levelMap.size()) ||
        HoverPosition.Y < 0 || HoverPosition.Y >= static_cast<int>(m_levelMap[0].size())) 
    {
        return HoverInfo(LOCALIZED_TEXT("out_of_bounds"), 0, 1);
    }

    if (m_levelMap[HoverPosition.X][HoverPosition.Y].Entity) 
    {
        return HoverInfo(LOCALIZED_TEXT(GETMONSTERKEY(m_levelMap[HoverPosition.X][HoverPosition.Y].Entity->Type)), m_levelMap[HoverPosition.X][HoverPosition.Y].Entity->GetHP(), m_levelMap[HoverPosition.X][HoverPosition.Y].Entity->GetMaxHP());
    }
    else if (m_levelMap[HoverPosition.X][HoverPosition.Y].CurrentItem)
    {
        return HoverInfo(ToString(m_levelMap[HoverPosition.X][HoverPosition.Y].CurrentItem->Type), 0, 1);
    }
    else 
    {
        return HoverInfo(ToString(m_levelMap[HoverPosition.X][HoverPosition.Y].Arch), 0, 1);
    }

    return HoverInfo(LOCALIZED_TEXT("error"), 0, 0);
}


void DungeonLevel::KillEntityOnPosition(Position Location, bool bUpdateQueue)
{
    //remove this entity from queue
    if (bUpdateQueue)
    {
        for (auto& Turn : m_monsterQueue)
        {
            for (auto Iterator = Turn.begin(); Iterator != Turn.end();)
            {
                if ((*Iterator)->Location == Location)
                {
                    Iterator = Turn.erase(Iterator);
                }
                else
                {
                    ++Iterator;
                }
            }
        }
    }

    m_levelMap[Location.X][Location.Y].Entity = nullptr;

    for (auto Iterator = m_entitiesOnLevel.begin(); Iterator != m_entitiesOnLevel.end(); ++Iterator)
    {
        if ((*Iterator)->Location == Location)
        {
            m_entitiesOnLevel.erase(Iterator);
            break;
        }
    }
}

bool DungeonLevel::IsMoveLegal(Position PlayerMove) const
{
    if (PlayerMove.X < 0 || PlayerMove.X >= m_declaredBoardSize || PlayerMove.Y < 0 || PlayerMove.Y >= m_declaredBoardSize) 
    {
        return false; //out of bounds
    }

    if (m_levelMap[PlayerMove.X][PlayerMove.Y].Arch == Architecture::WallTile)
    {
        return false; //tile is a wall
    }

    Position CurrentPlayerPosition = GetPlayerPosition();
    if (abs(PlayerMove.X - CurrentPlayerPosition.X) > 1 || abs(PlayerMove.Y - CurrentPlayerPosition.Y) > 1)
    {
        return false; //move is more than one tile away from the player
    }

    return true;
}
