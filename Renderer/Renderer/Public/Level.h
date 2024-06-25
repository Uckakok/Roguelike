#pragma once
#include "Defines.h"
#include<vector>
#include<string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GraphicHandler.h"
#include "Entity.h"


struct Node 
{
	Position Pos;
	double GCost; // Cost from start to current node
	double HCost; // Heuristic (estimated) cost from current node to goal
	Node* Parent; // Parent node in the path

	Node(Position NewPosition, double NewGCost, double NewHCost, Node* NewParent) : Pos(NewPosition), GCost(NewGCost), HCost(NewHCost), Parent(NewParent) {}

	double GetFCost() const 
	{
		return GCost + HCost;
	}
};

struct Item 
{
	ItemTypes Type = ItemTypes::Empty;
	Position Location = Position();
	int BonusHP = 0;
	int BonusDamage = 0;
	int BonusPermaHP = 0;

	Item(ItemTypes NewType, Position NewPosition, int NewBonusHP, int NewBonusDamage, int NewBonusPermaHP) : Type(NewType), Location(NewPosition), BonusHP(NewBonusHP), BonusDamage(NewBonusDamage), BonusPermaHP(NewBonusPermaHP) { ; };
	Item() = default;
};

inline BSTR ToString(ItemTypes Type) 
{
	switch (Type) 
	{
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

struct LevelTile 
{
	Position Coordinates;
	Architecture Arch;
	Entity* Entity = nullptr;
	Item* CurrentItem = nullptr;
};

class DungeonLevel 
{
public:
	void RemovePlayer();
	bool SaveMapToSave();
	void UseCurrentObject();
	void SpawnPlayer(bool bFromUp, Entity* Player);
	bool LoadMapFromSave(const std::string& SaveName);
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
	bool GetGameWon();
	HoverInfo ConstructHoverInfo(Position HoverPosition);

	int LevelIndex;
private:
	void UseItem(Item* UsedItem);
	void PutInQueue(size_t PositionOffset, Entity* EntityToAdd);
	void ConnectAreas(std::vector<std::vector<Position>>& Areas);
	std::vector<Position> FindAllConnected(Position StartPos, std::vector<std::vector<LevelTile>>* NewMap);
	void KillEntityOnPosition(Position Location, bool bUpdateQueue = true);
	bool IsMoveLegal(Position PlayerMove);

	std::vector<std::vector<LevelTile>> m_levelMap;
	std::vector<std::vector<Entity*>> m_monsterQueue;

	int m_declaredBoardSize = 0;

	bool m_bIsGameWon = false;
	bool m_bIsGameEnded = false;
	std::vector<Item*> m_itemsOnLevel;
	std::vector<Entity*> m_entitiesOnLevel;
};