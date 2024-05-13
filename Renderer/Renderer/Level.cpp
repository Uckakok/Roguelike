#include "pch.h"
#include "Level.h"
#include <queue>
#include <cmath>
#include "GameEngine.h"

void DungeonLevel::UseCurrentObject()
{
    Position PlayerPos = GetPlayerPosition();
    if (LevelMap[PlayerPos.x][PlayerPos.y].Arch == Architecture::StairsDownTile) {
        GameEngine::GetInstance()->LoadLevel(LevelIndex + 1);
    }
    if (LevelMap[PlayerPos.x][PlayerPos.y].Arch == Architecture::StairsUpTile) {
        GameEngine::GetInstance()->LoadLevel(LevelIndex - 1);
    }
    //todo: use other things
}

void DungeonLevel::SpawnPlayer(bool bFromUp, Entity* Player)
{
    if (GetPlayer())
    {
        MessageBox(nullptr, L"Player is already present on this level!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (bFromUp) {
        for (int i = 0; i < DeclaredBoardSize; ++i) {
            for (int j = 0; j < DeclaredBoardSize; ++j) {
                if (LevelMap[i][j].Arch == Architecture::StairsUpTile) {
                    EntitiesOnLevel.push_back(new Entity(*Player));
                    GetPlayer()->Location = Position(i, j);
                    LevelMap[i][j].Entity = GetPlayer();
                    MonsterQueue[0].insert(MonsterQueue[0].begin(), GetPlayer());
                    return;
                }
            }
        }
    }
    else {
        for (int i = 0; i < DeclaredBoardSize; ++i) {
            for (int j = 0; j < DeclaredBoardSize; ++j) {
                if (LevelMap[i][j].Arch == Architecture::StairsDownTile) {
                    EntitiesOnLevel.push_back(new Entity(*Player));
                    GetPlayer()->Location = Position(i, j);
                    LevelMap[i][j].Entity = GetPlayer();
                    MonsterQueue[0].insert(MonsterQueue[0].begin(), GetPlayer());
                    return;
                }
            }
        }
    }
    MessageBox(nullptr, L"didn't find a valid localization to spawn a player!", L"Error", MB_OK | MB_ICONERROR);
}

bool DungeonLevel::LoadMapFromSave(std::string& SaveName)
{
    std::ifstream file(SaveName);
    if (!file.is_open()) {
        //std::string errorMessage = "Failed to load dungeon save: " + SaveName;
        //MessageBox(nullptr, std::wstring(errorMessage.begin(), errorMessage.end()).c_str(), L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    //save current level

    for (auto* ent : EntitiesOnLevel) {
        delete ent;
    }

    EntitiesOnLevel.clear();
    ItemsOnLevel.clear();

    std::vector<std::vector<LevelTile>> newLevelMap;

    std::string line;
    int BoardSize = 0;
    int CurrentSize = -1;
    while (std::getline(file, line) && !line.empty()) {
        int LineSize = 0;
        std::vector<LevelTile> row;
        std::istringstream iss(line);
        int value;
        while (iss >> value) {
            LevelTile tile;
            tile.Arch = static_cast<Architecture>(value);
            row.push_back(tile);
            LineSize++;
        }
        if (CurrentSize == -1) {
            CurrentSize = LineSize;
        }
        else if (CurrentSize != LineSize){
            MessageBox(nullptr, L"Corrupted level data!", L"Error", MB_OK | MB_ICONERROR);
            return false;
        }
        BoardSize++;
        newLevelMap.push_back(row);
    }

    if (BoardSize != CurrentSize) {
        MessageBox(nullptr, L"Corrupted level data!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    DeclaredBoardSize = BoardSize;

    int entityID, x, y, HP, Damage;
    while (std::getline(file, line)) {
        if (line.empty()) {
            break;
        }

        std::istringstream iss(line);
        iss >> entityID >> x >> y >> HP >> Damage;

        if (entityID != 0) {
            EntitiesOnLevel.push_back(new Entity(static_cast<EntityTypes>(entityID), Position(x, y), HP, Damage));
        }
    }

    int itemID, BonusHP, BonusDamage;
    while (file >> itemID >> x >> y >> BonusHP >> BonusDamage) {
        if (itemID != 0) {
            ItemsOnLevel.push_back(Item(static_cast<ItemTypes>(itemID), Position(x, y), BonusHP, BonusDamage));
        }
    }

    for (auto& ent : EntitiesOnLevel) {
        newLevelMap[ent->Location.x][ent->Location.y].Entity = ent;
    }

    for (auto& it : ItemsOnLevel) {
        newLevelMap[it.Location.x][it.Location.y].Items.push_back(&it);
    }

    std::vector<Entity*> AllEnts;

    for (auto& ent : EntitiesOnLevel) {
        if (ent->IsPlayer()) continue;
        AllEnts.push_back(ent);
    }
    AllEnts.insert(AllEnts.begin(), GetPlayer());

    //std::stringstream ss;
    //ss << "AllEnts Contents:\n";

    //// Iterate through AllEnts
    //for (const auto& ent : AllEnts) {
    //    ss << "Entity " << ToString(ent->Type) << ", IsPlayer: " << (ent->IsPlayer() ? "true" : "false") << "\n";
    //}

    //// Convert stringstream to string
    //std::string message = ss.str();

    //// Display the message box
    //MessageBox(NULL, std::wstring(message.begin(), message.end()).c_str(), L"AllEnts Contents", MB_OK);

    MonsterQueue.push_back(AllEnts);

    LevelMap = std::move(newLevelMap);
    return true;
}

void DungeonLevel::PutInQueue(int PositionOffset, Entity* ent)
{
    while (MonsterQueue.size() <= PositionOffset) {
        std::vector<Entity*> NewTurn;
        MonsterQueue.push_back(NewTurn);
    }
    if (ent->IsPlayer()) {
        MonsterQueue[PositionOffset].insert(MonsterQueue[PositionOffset].begin(), ent);
    }
    else {
        MonsterQueue[PositionOffset].push_back(ent);
    }
}

void DungeonLevel::ConnectAreas(std::vector<std::vector<Position>>& Areas) {
    // Iterate over all pairs of areas
    std::vector<Position> Tunnels;
    for (size_t i = 0; i < Areas.size(); ++i) {
        for (size_t j = i + 1; j < Areas.size(); ++j) {
            // Find the shortest path between the current pair of areas
            std::vector<Position> ShortestPath = GetPath(Areas[i][0], Areas[j][0], true);
            for (int i = 0; i < ShortestPath.size(); ++i) {
                Tunnels.push_back(ShortestPath[i]);
            }
        }
    }

    for (auto& pos : Tunnels) {
        LevelMap[pos.x][pos.y].Arch = Architecture::FloorTile;
    }
}

void DungeonLevel::GenerateMap()
{
    std::vector<std::vector<LevelTile>> GeneratedMap;

    //generate full wall map
    for (int i = 0; i < LEVEL_SIZE; ++i) {
        std::vector<LevelTile> Row;
        for (int j = 0; j < LEVEL_SIZE; ++j) {
            LevelTile NewTile;
            NewTile.Arch = Architecture::WallTile;
            Row.push_back(NewTile);
        }
        GeneratedMap.push_back(Row);
    }

    int NumRectangles = 24; //generate them somehow
    int MaxSize = 6; //generate this somehow
    int MinSize = 3;
    srand(time(0));

    //add random rectangles on the map
    for (int r = 0; r < NumRectangles; ++r) {
        // Random position for the top-left corner of the rectangle
        int startX = rand() % (LEVEL_SIZE - MaxSize);
        int startY = rand() % (LEVEL_SIZE - MaxSize);

        // Random size for the rectangle
        int width = MinSize + rand() % (MaxSize - MinSize + 1);
        int height = MinSize + rand() % (MaxSize - MinSize + 1);

        // Fill the rectangle with Architecture::Floor
        for (int i = startX; i < startX + width; ++i) {
            for (int j = startY; j < startY + height; ++j) {
                GeneratedMap[i][j].Arch = Architecture::FloorTile;
            }
        }
    }

    DeclaredBoardSize = LEVEL_SIZE;

    std::vector<std::vector<Position>> Areas;

    for (int i = 0; i < LEVEL_SIZE; ++i) {
        for (int j = 0; j < LEVEL_SIZE; ++j) {
            if (GeneratedMap[i][j].Arch == Architecture::FloorTile) {
                Areas.push_back(FindAllConnected(Position(i, j), &GeneratedMap));
            }
        }
    }

    LevelMap = std::move(GeneratedMap);
    ConnectAreas(Areas);

    //fix up the board after calculating areas
    for (int i = 0; i < LEVEL_SIZE; ++i) {
        for (int j = 0; j < LEVEL_SIZE; ++j) {
            if (LevelMap[i][j].Arch == Architecture::Visited) {
                LevelMap[i][j].Arch = Architecture::FloorTile;
            }
        }
    }
    
    //spawn 'things'
    //gather all tiles
   std::vector<LevelTile*> ValidTilesForSpawning;
    for (int i = 0; i < LEVEL_SIZE; ++i) {
        for (int j = 0; j < LEVEL_SIZE; ++j) {
            if (LevelMap[i][j].Arch == Architecture::FloorTile) {
                LevelMap[i][j].Coordinates = Position(i, j);
                ValidTilesForSpawning.push_back(&LevelMap[i][j]);
            }
        }
    }

    //generate stairs
    ValidTilesForSpawning[rand() % ValidTilesForSpawning.size()]->Arch = Architecture::StairsDownTile;
    int Index = rand() % ValidTilesForSpawning.size();
    while (ValidTilesForSpawning[Index]->Arch == StairsDownTile) {
        Index = rand() % ValidTilesForSpawning.size();
    }
    ValidTilesForSpawning[Index]->Arch = Architecture::StairsUpTile;

    for (auto* ent : EntitiesOnLevel) {
        delete ent;
    }
    EntitiesOnLevel.clear();
    ItemsOnLevel.clear();

    int GoblinCount = rand() % 10 + 5;

    for (int i = 0; i < GoblinCount; ++i) {
        Index = rand() % ValidTilesForSpawning.size();
        if (ValidTilesForSpawning[Index]->Arch == Architecture::StairsUpTile || ValidTilesForSpawning[Index]->Entity) continue;
        EntitiesOnLevel.push_back(new Entity(EntityTypes::GoblinEntity, ValidTilesForSpawning[Index]->Coordinates, 100, 10));
        ValidTilesForSpawning[Index]->Entity = EntitiesOnLevel.back();
    }

    
    std::vector<Entity*> AllEnts;

    for (auto& ent : EntitiesOnLevel) {
        AllEnts.push_back(ent);
    }

    MonsterQueue.push_back(AllEnts);

}


std::vector<Position> DungeonLevel::FindAllConnected(Position StartPos, std::vector<std::vector<LevelTile>>* NewMap)
{
    std::vector<Position> Connected;

    // Check if the starting position is a floor tile
    if (NewMap->at(StartPos.x)[StartPos.y].Arch != Architecture::FloorTile) {
        MessageBox(nullptr, L"Error when generating map!", L"Error", MB_OK | MB_ICONERROR);
        return Connected;
    }

    // Define the possible movement directions (including diagonals)
    std::vector<Position> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, // Straight movement
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // Diagonal movement
    };

    // Create a queue for BFS
    std::queue<Position> queue;
    queue.push(StartPos);

    // Mark the starting position as visited
    NewMap->at(StartPos.x)[StartPos.y].Arch = Visited;

    // Perform BFS
    while (!queue.empty()) {
        // Get the front position from the queue
        Position currentPos = queue.front();
        queue.pop();

        // Add the current position to the connected vector
        Connected.push_back(currentPos);

        // Explore all neighboring tiles
        for (const auto& dir : directions) {
            int nx = currentPos.x + dir.x;
            int ny = currentPos.y + dir.y;
            // Check if the neighboring position is within bounds and is a floor tile
            if (nx >= 0 && nx < NewMap->size() && ny >= 0 && ny < NewMap->at(0).size() && NewMap->at(nx)[ny].Arch == Architecture::FloorTile) {
                // Mark the neighboring position as visited
                NewMap->at(nx)[ny].Arch = Visited;
                // Add the neighboring position to the queue for further exploration
                queue.push({ nx, ny });
            }
        }
    }

    return Connected;
}

void DungeonLevel::RemovePlayer()
{
    Entity* Player = GetPlayer();
    LevelMap[Player->Location.x][Player->Location.y].Entity = nullptr;
    KillEntityOnPosition(Player->Location);
}

bool DungeonLevel::SaveMapToSave()
{
    std::string SaveName = GameEngine::GetInstance()->GetPlayerName();
    SaveName.append(std::to_string(LevelIndex));
    SaveName.append(EXTENSION);


    std::ofstream file(SaveName);
    if (!file.is_open()) {
        std::wstring errorMessage = L"Failed to save dungeon data to file: " + std::wstring(SaveName.begin(), SaveName.end());
        MessageBox(nullptr, errorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Save the level map
    for (const auto& row : LevelMap) {
        for (const auto& tile : row) {
            file << static_cast<int>(tile.Arch) << ' ';
        }
        file << std::endl;
    }

    file << std::endl;

    // Save the entities
    for (auto& ent : EntitiesOnLevel) {
        file << static_cast<int>(ent->Type) << ' ' << ent->Location.x << ' ' << ent->Location.y << ' ' << ent->GetHP() << ' ' << ent->GetDamage() << std::endl;
    }

    file << std::endl;

    // Save the items
    for (const auto& item : ItemsOnLevel) {
        file << static_cast<int>(item.Type) << ' ' << item.Location.x << ' ' << item.Location.y << ' ' << item.BonusHP << ' ' << item.BonusDamage << std::endl;
    }
    file.close();

    return true;
}

int DungeonLevel::GetDeclaredBoardSize()
{
    return DeclaredBoardSize;
}

std::vector<TileToDraw> DungeonLevel::GatherTilesForRender()
{
    std::vector<TileToDraw> GatheredTiles;

    if (DeclaredBoardSize != LevelMap.size() || LevelMap.size() <= 0 || DeclaredBoardSize != LevelMap[0].size()) {
        std::wstring message = L"DeclaredBoardSize does not match the actual size of LevelMap.\n";
        message += L"DeclaredBoardSize: " + std::to_wstring(DeclaredBoardSize) + L"\n";
        message += L"LevelMap size: " + std::to_wstring(LevelMap.size()) + L"\n";
        if (LevelMap.size() > 0) {
            message += L"LevelMap row size: " + std::to_wstring(LevelMap[0].size()) + L"\n";
        }
        else {
            message += L"LevelMap is empty\n";
        }

        MessageBox(nullptr, message.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return GatheredTiles;
    }

    for (int i = 0; i < DeclaredBoardSize; ++i) {
        for (int j = 0; j < DeclaredBoardSize; ++j) {
            GatheredTiles.push_back(TileToDraw(i, j, ToTileType(LevelMap[i][j].Arch)));
        }
    }

    for (auto& elem : ItemsOnLevel)
    {
        if (elem.Type == ItemTypes::Empty) {
            continue;
        }
        if (elem.Type == ItemTypes::BowItem) {
            GatheredTiles.push_back(TileToDraw(elem.Location.x, elem.Location.y, TileTypes::Bow));
        }
        if (elem.Type == ItemTypes::SwordItem) {
            GatheredTiles.push_back(TileToDraw(elem.Location.x, elem.Location.y, TileTypes::Sword));
        }
    }

    for (auto& mon : EntitiesOnLevel)
    {
        if (mon->Type == EntityTypes::None) {
            MessageBox(nullptr, L"Invalid entity present on level", L"Error", MB_OK | MB_ICONERROR);
            continue;
        }
        GatheredTiles.push_back(TileToDraw(mon->Location.x, mon->Location.y, ToTiletype(mon->Type)));
    }

    //player died
    if (!GetPlayer()) return GatheredTiles;

    Position playerPos = GetPlayerPosition();

    // Add highlighted tiles around the player
    const int radius = 1;
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            int nx = playerPos.x + dx;
            int ny = playerPos.y + dy;
            if (nx == playerPos.x && ny == playerPos.y) continue;
            // Check if the tile is within bounds and not a wall
            if (nx >= 0 && nx < DeclaredBoardSize && ny >= 0 && ny < DeclaredBoardSize && LevelMap[nx][ny].Arch != Architecture::WallTile) {
                GatheredTiles.push_back(TileToDraw(nx, ny, TileTypes::Highlight));
            }
        }
    }

    return GatheredTiles;
}

Position DungeonLevel::GetPlayerPosition()
{
    if (GetPlayer()) {
        return GetPlayer()->Location;
    }
    else {
        MessageBox(nullptr, L"Player was not found.", L"Error", MB_OK | MB_ICONERROR);
    }
}

bool DungeonLevel::PerformAction(Position PlayerMove)
{
    if (!IsMoveLegal(PlayerMove)) {
        return false;
    }

    Entity* PlayerEntity = GetPlayer();
    Entity* EntityOnTile = GetEntityOnTile(PlayerMove);
    if (EntityOnTile) {
        //perform attack
        EntityOnTile->ReceiveDamage(PlayerEntity->GetDamage());
        if (EntityOnTile->IsDead()) {
            KillEntityOnPosition(PlayerMove);
        }
        PutInQueue(5, PlayerEntity);
        for (auto it = MonsterQueue.front().begin(); it != MonsterQueue.front().end(); ++it) {
            if (*it == GetPlayer()) {
                MonsterQueue.front().erase(it);
                break;
            }
        }
        return true;
    }

    LevelMap[PlayerEntity->Location.x][PlayerEntity->Location.y].Entity = nullptr;
    PlayerEntity->Location = PlayerMove;
    LevelMap[PlayerEntity->Location.x][PlayerEntity->Location.y].Entity = PlayerEntity;

    PutInQueue(4, PlayerEntity);
    for (auto it = MonsterQueue.front().begin(); it != MonsterQueue.front().end(); ++it) {
        if (*it == GetPlayer()) {
            MonsterQueue.front().erase(it);
            break;
        }
    }
    return true;
}

bool DungeonLevel::MoveEntity(Entity* EntityToMove)
{
    if (!EntityToMove) {
        MessageBox(nullptr, L"Tried to move invalid entity", L"Error", MB_OK | MB_ICONERROR);
        //don't try to move it ever again
        return false;
    }

    std::vector<Position> PathToTake = GetPath(EntityToMove->Location, GetPlayerPosition());

    if (PathToTake.size() < 2) {
        // invalid path. No way to reach the goal
        //MessageBox(nullptr, L"No valid path to move the entity", L"Error", MB_OK | MB_ICONERROR);
        PutInQueue(4, EntityToMove);
        return false;
    }

    Position NextMove = PathToTake[1];

    Entity* EntityOnTile = GetEntityOnTile(NextMove);

    if (EntityOnTile) {
        //perform attack
        EntityOnTile->ReceiveDamage(EntityToMove->GetDamage());
        if (EntityOnTile->IsDead()) {
            if (EntityOnTile->IsPlayer()) {
                KillEntityOnPosition(NextMove);
                MessageBox(nullptr, L"You Died!", L"Game over", MB_OK | MB_ICONEXCLAMATION);
                bIsGameEnded = true;
            }
        }

        PutInQueue(8, EntityToMove);
        return true;
    }

    LevelMap[EntityToMove->Location.x][EntityToMove->Location.y].Entity = nullptr;
    EntityToMove->Location = NextMove;
    LevelMap[EntityToMove->Location.x][EntityToMove->Location.y].Entity = EntityToMove;

    PutInQueue(6, EntityToMove);
    return true;
}

Entity* DungeonLevel::GetPlayer()
{
    for (auto& mon : EntitiesOnLevel)
    {
        if (mon->Type == EntityTypes::PlayerEntity)
        {
            return mon;
        }
    }

    return nullptr;
}

Entity* DungeonLevel::GetEntityOnTile(Position Location)
{
    return LevelMap[Location.x][Location.y].Entity;
}


double CalculateDistance(const Position& a, const Position& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

bool IsValid(const Position& pos, const std::vector<std::vector<LevelTile>>& levelMap, bool bIgnoreAll) {
    if (pos.x < 0 || pos.x >= static_cast<int>(levelMap.size()) || pos.y < 0 || pos.y >= static_cast<int>(levelMap[0].size())) {
        return false; // Position is out of bounds
    }
    if (bIgnoreAll) return true;
    return levelMap[pos.x][pos.y].Arch != Architecture::WallTile && (!levelMap[pos.x][pos.y].Entity || levelMap[pos.x][pos.y].Entity->IsPlayer()); // Position is not a wall and doesn't have entity
}

std::vector<Position> ReconstructPath(Node* current) {
    std::vector<Position> path;
    while (current != nullptr) {
        path.push_back(current->pos);
        current = current->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Position> DungeonLevel::GetPath(Position Start, Position Goal, bool bIgnoreAll)
{

    // Validate Goal
    if (!IsValid(Goal, LevelMap, bIgnoreAll)) {
        return {}; // Goal position is not valid
    }

    if (CalculateDistance(Start, Goal) > 11 && !bIgnoreAll)
    {
        return {}; //too far away. No reason to waste resources
    }

    std::vector<Position> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, // Straight movement
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // Diagonal movement
    };

    auto compareNodes = [](Node* a, Node* b) { return a->GetFCost() > b->GetFCost(); };
    std::priority_queue<Node*, std::vector<Node*>, decltype(compareNodes)> openSet(compareNodes);

    std::vector<std::vector<bool>> visited(LevelMap.size(), std::vector<bool>(LevelMap[0].size(), false));

    Node* startNode = new Node(Start, 0.0, CalculateDistance(Start, Goal), nullptr);
    openSet.push(startNode);

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();

        if (current->pos == Goal) {
            std::vector<Position> path = ReconstructPath(current);
            // Clean up memory
            while (!openSet.empty()) {
                delete openSet.top();
                openSet.pop();
            }
            return path;
        }

        if (current->gCost > 12 && !bIgnoreAll) {
            // Clean up memory
            while (!openSet.empty()) {
                delete openSet.top();
                openSet.pop();
            }
            return {}; // Path length exceeds limit
        }

        visited[current->pos.x][current->pos.y] = true;

        for (const auto& dir : directions) {
            Position neighborPos = { current->pos.x + dir.x, current->pos.y + dir.y };
            if (!IsValid(neighborPos, LevelMap, bIgnoreAll) || visited[neighborPos.x][neighborPos.y]) {
                continue;
            }

            double neighborGCost = current->gCost + (dir.x == 0 || dir.y == 0 ? 1.0 : 1.4); // Cost of movement
            Node* neighborNode = new Node(neighborPos, neighborGCost, CalculateDistance(neighborPos, Goal), current);
            openSet.push(neighborNode);
        }
    }

    // Clean up memory
    while (!openSet.empty()) {
        delete openSet.top();
        openSet.pop();
    }

    return {}; // Path not found
}

void DungeonLevel::PerformEntitiesTurn()
{
    //std::stringstream ss;
    //ss << "MonsterQueue Contents:\n";

    //// Iterate through the MonsterQueue
    //for (size_t i = 0; i < MonsterQueue.size(); ++i) {
    //    ss << "Queue " << i << ": ";
    //    for (size_t j = 0; j < MonsterQueue[i].size(); ++j) {
    //        ss << "Entity " << ToString(MonsterQueue[i][j]->Type);
    //        if (j != MonsterQueue[i].size() - 1) {
    //            ss << ", ";
    //        }
    //    }
    //    ss << "\n";
    //}

    //// Convert stringstream to string
    //std::string message = ss.str();

    //// Display the message box
    //MessageBox(NULL, std::wstring(message.begin(), message.end()).c_str(), L"MonsterQueue Contents", MB_OK);

    while (MonsterQueue.front().empty() || !MonsterQueue.front().front()->IsPlayer()) {
        for (auto& ent : MonsterQueue.front()) {
            if (ent->IsPlayer()) continue; //or just crash. Shouldn't happen
            if (bIsGameEnded) break;

            MoveEntity(ent);
        }
        MonsterQueue.erase(MonsterQueue.begin());
    }
}

bool DungeonLevel::GetGameEnded()
{
    return bIsGameEnded;
}

bool DungeonLevel::IsUseAvailable()
{
    Position PlayerPos = GetPlayerPosition();
    if (LevelMap[PlayerPos.x][PlayerPos.y].Arch == Architecture::StairsDownTile || LevelMap[PlayerPos.x][PlayerPos.y].Arch == Architecture::StairsUpTile) {
        return true;
    }
    if (!LevelMap[PlayerPos.x][PlayerPos.y].Items.empty()) {
        return true;
    }
    return false;
}

HoverInfo DungeonLevel::ConstructHoverInfo(Position HoverPosition)
{
    if (HoverPosition.x < 0 || HoverPosition.x >= static_cast<int>(LevelMap.size()) ||
        HoverPosition.y < 0 || HoverPosition.y >= static_cast<int>(LevelMap[0].size())) {
        return HoverInfo("Out of Bounds", 0, 0);
    }

    if (LevelMap[HoverPosition.x][HoverPosition.y].Entity == nullptr) {
        return HoverInfo(ToString(LevelMap[HoverPosition.x][HoverPosition.y].Arch), 0, 1);
    }
    else {
        return HoverInfo(ToString(LevelMap[HoverPosition.x][HoverPosition.y].Entity->Type), LevelMap[HoverPosition.x][HoverPosition.y].Entity->GetHP(), 100);
    }

    return HoverInfo("Error", 0, 0);
}


void DungeonLevel::KillEntityOnPosition(Position Location)
{
    // Set the entity pointer in the LevelMap to nullptr
    LevelMap[Location.x][Location.y].Entity = nullptr;

    // Erase the entity from the EntitiesOnLevel vector
    for (auto it = EntitiesOnLevel.begin(); it != EntitiesOnLevel.end(); ++it) {
        if ((*it)->Location == Location) {
            EntitiesOnLevel.erase(it);
            break;
        }
    }

    //remove this entity from queue
    for (auto& queue : MonsterQueue) {
        for (auto it = queue.begin(); it != queue.end();) {
            if ((*it)->Location == Location) {
                delete* it;
                it = queue.erase(it); // Update iterator after erasing
            }
            else {
                ++it;
            }
        }
    }
}

bool DungeonLevel::IsMoveLegal(Position PlayerMove)
{
    //ignore ranged attacks for now. handle later

    if (PlayerMove == GetPlayer()->Location) {
        return false;
    }

    if (PlayerMove.x < 0 || PlayerMove.x >= DeclaredBoardSize || PlayerMove.y < 0 || PlayerMove.y >= DeclaredBoardSize) {
        return false; // Out of bounds
    }

    if (LevelMap[PlayerMove.x][PlayerMove.y].Arch == Architecture::WallTile) {
        return false; // Tile is a wall
    }

    Position currentPlayerPosition = GetPlayerPosition();
    if (abs(PlayerMove.x - currentPlayerPosition.x) > 1 || abs(PlayerMove.y - currentPlayerPosition.y) > 1) {
        return false; // Move is more than one tile away from the player
    }

    return true;
}
