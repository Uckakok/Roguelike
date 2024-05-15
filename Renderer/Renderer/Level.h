#pragma once
#include "Defines.h"
#include<vector>
#include<string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GraphicHandler.h"
#include "Entity.h"


struct Node {
	Position pos;
	double gCost; // Cost from start to current node
	double hCost; // Heuristic (estimated) cost from current node to goal
	Node* parent; // Parent node in the path

	Node(Position p, double g, double h, Node* par) : pos(p), gCost(g), hCost(h), parent(par) {}

	double GetFCost() const {
		return gCost + hCost;
	}
};

struct Item {
	ItemTypes Type = ItemTypes::Empty;
	Position Location = Position();
	int BonusHP = 0;
	int BonusDamage = 0;
	int BonusPermaHP = 0;
	//list of buffs or sth

	Item(ItemTypes NewType, Position NewPosition, int NewBonusHP, int NewBonusDamage, int NewBonusPermaHP) : Type(NewType), Location(NewPosition), BonusHP(NewBonusHP), BonusDamage(NewBonusDamage), BonusPermaHP(NewBonusPermaHP) { ; };
	Item() = default;
};

inline BSTR ToString(ItemTypes Type) {
	switch (Type) {
	case ItemTypes::HealingPotionItem:
		return LOCALIZED_TEXT("healing_potion");
	case ItemTypes::StrengthRuneItem:
		return LOCALIZED_TEXT("strength_rune");
	case ItemTypes::VitalityRuneItem:
		return LOCALIZED_TEXT("vitality_rune");
	default:
		return LOCALIZED_TEXT("error");
	}
}

struct LevelTile {
	Position Coordinates;
	Architecture Arch;
	Entity* Entity = nullptr;
	Item* CurrentItem = nullptr;
};

class DungeonLevel {
public:
	void RemovePlayer();
	bool SaveMapToSave();
	int LevelIndex;
	void UseCurrentObject();
	void SpawnPlayer(bool bFromUp, Entity* Player);
	bool LoadMapFromSave(std::string& SaveName);
	void GenerateMap();

	int GetDeclaredBoardSize();
	std::vector<TileToDraw> GatherTilesForRender();
	Position GetPlayerPosition();
	bool PerformAction(Position PlayerMove);
	bool MoveEntity(Entity* EntityToMove);
	Entity* GetPlayer();
	Entity* GetEntityOnTile(Position Location);

	std::vector<Position> GetPath(Position Start, Position Goal, bool bIgnoreAll = false);
	void PerformEntitiesTurn();
	bool GetGameEnded();
	bool IsUseAvailable();

	HoverInfo ConstructHoverInfo(Position HoverPosition);
private:
	void UseItem(Item* UsedItem);
	void PutInQueue(int PositionOffset, Entity* ent);
	void ConnectAreas(std::vector<std::vector<Position>>& Areas);
	std::vector<Position> FindAllConnected(Position StartPos, std::vector<std::vector<LevelTile>>* NewMap);
	bool bIsGameEnded = false;
	void KillEntityOnPosition(Position Location, bool bUpdateQueue = true);
	bool IsMoveLegal(Position PlayerMove);
	std::vector<std::vector<LevelTile>> LevelMap;
	std::vector<std::vector<Entity*>> MonsterQueue;

	int DeclaredBoardSize = 0;

	std::vector<Item*> ItemsOnLevel;
	std::vector<Entity*> EntitiesOnLevel;
};