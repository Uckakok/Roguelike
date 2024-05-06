#include "pch.h"
#include "Level.h"

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
    while (std::getline(file, line) && !line.empty()) {
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
        }
        BoardSize++;
        newLevelMap.push_back(row);
    }

    DeclaredBoardSize = BoardSize;

    int entityID, x, y;
    while (std::getline(file, line)) {
        if (line.empty()) {
            break; // Exit loop if empty line encountered
        }

        std::istringstream iss(line);
        iss >> entityID >> x >> y;

        if (entityID != 0) {
            newLevelMap[y][x].CurrentMonster.Type = static_cast<EntityTypes>(entityID);
        }
    }

    int itemID;
    while (file >> itemID >> x >> y) {
        if (itemID != 0) {
            Item newItem;
            newItem.Type = static_cast<ItemTypes>(itemID);
            newLevelMap[y][x].Items.push_back(newItem);
        }
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

    for (int i = 0; i < DeclaredBoardSize; ++i) {
        for (int j = 0; j < DeclaredBoardSize; ++j) {
            for (auto& Item : LevelMap[i][j].Items) {
                if (Item.Type == ItemTypes::Empty) {
                    continue;
                }
                if (Item.Type == ItemTypes::BowItem) {
                    GatheredTiles.push_back(TileToDraw(i, j, TileTypes::Bow));
                }
                if (Item.Type == ItemTypes::SwordItem) {
                    GatheredTiles.push_back(TileToDraw(i, j, TileTypes::Sword));
                }
            }
        }
    }

    for (int i = 0; i < DeclaredBoardSize; ++i) {
        for (int j = 0; j < DeclaredBoardSize; ++j) {
            if (LevelMap[i][j].bIsWall) {
                continue;
            }
            if (LevelMap[i][j].CurrentMonster.Type == EntityTypes::None) {
                continue;
            }
            if (LevelMap[i][j].CurrentMonster.Type == EntityTypes::GoblinEntity) {
                GatheredTiles.push_back(TileToDraw(i, j, TileTypes::Goblin));
            }
            if (LevelMap[i][j].CurrentMonster.Type == EntityTypes::PlayerEntity) {
                GatheredTiles.push_back(TileToDraw(i, j, TileTypes::Player));
            }

        }
    }

    Position playerPos = GetPlayerPosition();

    // Add highlighted tiles around the player
    const int radius = 1; // Adjust the radius as needed
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

    for (int i = 0; i < DeclaredBoardSize; ++i) {
        for (int j = 0; j < DeclaredBoardSize; ++j) {
            if (LevelMap[i][j].CurrentMonster.Type  == EntityTypes::PlayerEntity) {
                return Position(i, j);
            }
        }
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

    //remove player from current tile
    Position currentPlayerPosition = GetPlayerPosition();
    LevelMap[currentPlayerPosition.x][currentPlayerPosition.y].CurrentMonster.Type = EntityTypes::None;

    //put player on desired tile
    LevelMap[PlayerMove.x][PlayerMove.y].CurrentMonster.Type = EntityTypes::PlayerEntity;

    return true;
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
