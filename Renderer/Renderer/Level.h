#pragma once
#include "Defines.h"
#include<vector>
#include<string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GraphicHandler.h"

struct Monster {
	EntityTypes Type;

	//stats but who cares
};


struct Item {
	ItemTypes Type;
	//list of buffs or sth
};

struct LevelTile {
	bool bIsWall;

	Monster CurrentMonster;

	std::vector<Item> Items;
};

class DungeonLevel {
public:
	bool LoadMapFromSave(std::string& SaveName);

	int GetDeclaredBoardSize();
	std::vector<TileToDraw> GatherTilesForRender();
	Position GetPlayerPosition();
	bool PerformAction(Position PlayerMove);

private:
	bool IsMoveLegal(Position PlayerMove);
	std::vector<std::vector<LevelTile>> LevelMap;

	int DeclaredBoardSize = 0;


	//cache until some change to the board happens



};