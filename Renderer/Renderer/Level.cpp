#include "pch.h"
#include "Level.h"
#include <queue>
#include <cmath>

bool DungeonLevel::LoadMapFromSave(std::string& SaveName)
{
    std::ifstream file(SaveName);
    if (!file.is_open()) {
        MessageBox(nullptr, L"Failed to load dungeon save!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

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
            if (value == 1) {
                tile.bIsWall = true;
            }
            else {
                tile.bIsWall = false;
            }
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
            break; // Exit loop if empty line encountered
        }

        std::istringstream iss(line);
        iss >> entityID >> x >> y >> HP >> Damage;

        if (entityID != 0) {
            EntitiesOnLevel.push_back(Entity(static_cast<EntityTypes>(entityID), Position(x, y), HP, Damage));
        }
    }

    int itemID, BonusHP, BonusDamage;
    while (file >> itemID >> x >> y >> BonusHP >> BonusDamage) {
        if (itemID != 0) {
            ItemsOnLevel.push_back(Item(static_cast<ItemTypes>(itemID), Position(x, y), BonusHP, BonusDamage));
        }
    }

    for (auto& ent : EntitiesOnLevel) {
        newLevelMap[ent.Location.x][ent.Location.y].Entity = &ent;
    }

    for (auto& it : ItemsOnLevel) {
        newLevelMap[it.Location.x][it.Location.y].Items.push_back(&it);
    }

    LevelMap = std::move(newLevelMap);
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
            if (LevelMap[i][j].bIsWall) {
                GatheredTiles.push_back(TileToDraw(i, j, TileTypes::Wall));
            }
            else {
                GatheredTiles.push_back(TileToDraw(i, j, TileTypes::Floor));
            }
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
        if (mon.Type == EntityTypes::None) {
            continue;
        }
        if (mon.Type == EntityTypes::GoblinEntity) {
            GatheredTiles.push_back(TileToDraw(mon.Location.x, mon.Location.y, TileTypes::Goblin));
        }
        if (mon.Type == EntityTypes::PlayerEntity) {
            GatheredTiles.push_back(TileToDraw(mon.Location.x, mon.Location.y, TileTypes::Player));
        }
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
            if (nx >= 0 && nx < DeclaredBoardSize && ny >= 0 && ny < DeclaredBoardSize && !LevelMap[nx][ny].bIsWall) {
                GatheredTiles.push_back(TileToDraw(nx, ny, TileTypes::Highlight));
            }
        }
    }

    return GatheredTiles;
}

Position DungeonLevel::GetPlayerPosition()
{
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
        return Position();
    }

    for (auto& mon : EntitiesOnLevel)
    {
        if (mon.IsPlayer()) return mon.Location;
    }

    MessageBox(nullptr, L"Couldn't locate player on the board!", L"Error", MB_OK | MB_ICONERROR);

    return Position();
}

bool DungeonLevel::PerformAction(Position PlayerMove)
{
    if (!IsMoveLegal(PlayerMove)) {
        //do something
        return false;
    }
    
    //todo: if desired tile is occupied by a monster do not move, just remove some hp from enemy


    Entity* PlayerEntity = GetPlayer();
    Entity* EntityOnTile = GetEntityOnTile(PlayerMove);
    if (EntityOnTile) {
        //perform attack
        EntityOnTile->ReceiveDamage(PlayerEntity->GetDamage());
        if (EntityOnTile->IsDead()) {
            KillEntityOnPosition(PlayerMove);
        }
        else {
            return true;
        }
    }

    LevelMap[PlayerEntity->Location.x][PlayerEntity->Location.y].Entity = nullptr;
    PlayerEntity->Location = PlayerMove;
    LevelMap[PlayerMove.x][PlayerMove.y].Entity = PlayerEntity;

    return true;
}

bool DungeonLevel::MoveEntity(Entity* EntityToMove)
{
    if (!EntityToMove) {
        MessageBox(nullptr, L"Tried to move invalid entity", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    std::vector<Position> PathToTake = GetPath(EntityToMove->Location, GetPlayerPosition());

    if (PathToTake.size() < 2) {
        // invalid path. No way to reach to goal
        return false;
    }
    Position NextMove = PathToTake[1];

    Entity* MovingEntity = GetEntityOnTile(EntityToMove->Location);
    Entity* EntityOnTile = GetEntityOnTile(NextMove);

    

    if (EntityOnTile) {
        //perform attack
        EntityOnTile->ReceiveDamage(MovingEntity->GetDamage());
        if (EntityOnTile->IsDead()) {
            if (EntityOnTile->IsPlayer()) {
                KillEntityOnPosition(NextMove);
                MessageBox(nullptr, L"You Died!", L"Game over", MB_OK | MB_ICONEXCLAMATION);
                bIsGameEnded = true;
                return true;
            }
            //handle killing of the player
        }
        else {
            return true;
        }
    }
    LevelMap[EntityToMove->Location.x][EntityToMove->Location.y].Entity = nullptr;
    MovingEntity->Location = NextMove;
    LevelMap[NextMove.x][NextMove.y].Entity = MovingEntity;

    return true;
}

Entity* DungeonLevel::GetPlayer()
{
    for (auto& mon : EntitiesOnLevel)
    {
        if (mon.Type == EntityTypes::PlayerEntity)
        {
            return &mon;
        }
    }

    //MessageBox(nullptr, L"Player couldn't be located on the level!", L"Error", MB_OK | MB_ICONERROR);

    return nullptr;
}

Entity* DungeonLevel::GetEntityOnTile(Position Location)
{
    return LevelMap[Location.x][Location.y].Entity;
}


double CalculateDistance(const Position& a, const Position& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

bool IsValid(const Position& pos, const std::vector<std::vector<LevelTile>>& levelMap) {
    if (pos.x < 0 || pos.x >= levelMap.size() || pos.y < 0 || pos.y >= levelMap[0].size()) {
        return false; // Position is out of bounds
    }
    return !levelMap[pos.x][pos.y].bIsWall && (!levelMap[pos.x][pos.y].Entity || levelMap[pos.x][pos.y].Entity->IsPlayer()); // Position is not a wall and doesn't have entity
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

std::vector<Position> DungeonLevel::GetPath(Position Start, Position Goal)
{

    // Validate Goal
    if (!IsValid(Goal, LevelMap)) {
        return {}; // Goal position is not valid
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

        visited[current->pos.x][current->pos.y] = true;

        for (const auto& dir : directions) {
            Position neighborPos = { current->pos.x + dir.x, current->pos.y + dir.y };
            if (!IsValid(neighborPos, LevelMap) || visited[neighborPos.x][neighborPos.y]) {
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
    for (auto& ent : EntitiesOnLevel) {
        if (ent.IsPlayer()) continue;
        if (bIsGameEnded) return;

        MoveEntity(&ent);
    }
}

bool DungeonLevel::GetGameEnded()
{
    return bIsGameEnded;
}

HoverInfo DungeonLevel::ConstructHoverInfo(Position HoverPosition)
{
    if (HoverPosition.x < 0 || HoverPosition.x >= LevelMap.size() ||
        HoverPosition.y < 0 || HoverPosition.y >= LevelMap[0].size()) {
        return HoverInfo("Out of Bounds", 0, 0);
    }

    if (LevelMap[HoverPosition.x][HoverPosition.y].bIsWall) {
        return HoverInfo("Wall", 0, 1);
    }
    else if (LevelMap[HoverPosition.x][HoverPosition.y].Entity == nullptr) {
        return HoverInfo("Floor", 0, 1);
    }
    else {
        std::string Name;
        switch (LevelMap[HoverPosition.x][HoverPosition.y].Entity->Type) {
        case EntityTypes::GoblinEntity:
            Name = "goblin";
            break;
        case EntityTypes::PlayerEntity:
            Name = "Player";
            break;
        default:
            Name = "Unknown";
        }
        return HoverInfo(Name, LevelMap[HoverPosition.x][HoverPosition.y].Entity->GetHP(), 100);
    }

    return HoverInfo("Error", 0, 0);
}

void DungeonLevel::KillEntityOnPosition(Position Location)
{
    EntitiesOnLevel.erase(std::remove_if(EntitiesOnLevel.begin(), EntitiesOnLevel.end(),
        [&](const auto& ent) {
        return ent.Location == Location;
    }), EntitiesOnLevel.end());
}

bool DungeonLevel::IsMoveLegal(Position PlayerMove)
{
    //ignore ranged attacks for now. handle later

    if (PlayerMove.x < 0 || PlayerMove.x >= DeclaredBoardSize || PlayerMove.y < 0 || PlayerMove.y >= DeclaredBoardSize) {
        return false; // Out of bounds
    }

    if (LevelMap[PlayerMove.x][PlayerMove.y].bIsWall) {
        return false; // Tile is a wall
    }

    Position currentPlayerPosition = GetPlayerPosition();
    if (abs(PlayerMove.x - currentPlayerPosition.x) > 1 || abs(PlayerMove.y - currentPlayerPosition.y) > 1) {
        return false; // Move is more than one tile away from the player
    }

    return true;
}
