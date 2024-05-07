#pragma once
#include<string>

#define LEVEL_SIZE	64
#define MAGICAL_NUMBER	30	//I literally barelly know meaning of this number, but works nice for 30!


enum ItemTypes {
	Empty,
	SwordItem,
	BowItem,
};


enum EntityTypes {
	None,
	PlayerEntity,
	GoblinEntity,
};

enum TileTypes {
	Floor,
	Wall,
	Sword,
	Bow,
	Player,
	Goblin,
	Highlight
};

struct TileToDraw {
	int x;
	int y;
	TileTypes Type;

	TileToDraw() = default;
	TileToDraw(int newx, int newy, TileTypes newType) : x(newx), y(newy), Type(newType) { ; };
};

struct Position {
	int x;
	int y;
	Position() = default;
	Position(int NewX, int NewY) : x(NewX), y(NewY) { ; };
	bool operator==(const Position& New) const {
		return this->x == New.x && this->y == New.y;
	}
};

struct HoverInfo {
	std::string Name;
	int CurrentHP;
	int MaxHP;
	HoverInfo(std::string NewName, int NewCurrentHP, int NewMaxHP) : Name(NewName), CurrentHP(NewCurrentHP), MaxHP(NewMaxHP) { ; };
};

