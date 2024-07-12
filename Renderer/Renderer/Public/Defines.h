#pragma once
#include <string>
#include "Texture.h"
#include <comutil.h>
#include "LocalizationManager.h"
#include <fstream>

#define LEVEL_SIZE	32
#define TILE_MIDDLE	32
#define EXTENSION	".Rog"
#define MONSTER_SPAWN_RANDOMIZER	6
#define STRENGTH_DEPTH_MULTIPLIER	1
#define HEALING_DEPTH_MULTIPLIER	8
#define MAX_HP_DEPTH_MULTIPLIER		4

#define MAX(x, y) (x > y ? x : y);

enum ItemTypes 
{
	Empty,
	HealingPotionItem,
	VitalityRuneItem,
	StrengthRuneItem,
};

enum Architecture 
{
	FloorTile,
	WallTile,
	StairsDownTile,
	StairsUpTile,
	StairsAirTile,
	StairsFireTile,
	StairsWaterTile,
	StairsEarthTile,
	Visited, //for generating maps only
};


enum class EntityTypes 
{
	None,
	PlayerEntity,
	GoblinEntity,
	OrcEntity,
	DrakeEntity,
	GriffinEntity,
	AngelEntity,
	DwarfEntity,
	DendroidEntity,
	GoldenDragonEntity,
	ImpEntity,
	SkeletonEntity,
	GnollEntity,
	Num
};

enum class TileTypes 
{
	Floor,
	Wall,
	Highlight,
	StairsDown,
	StairsUp,
	HealingPotion,
	VitalityRune,
	StrengthRune,
	Player,
	Goblin,
	Orc,
	Drake,
	Griffin,
	Angel,
	Dwarf,
	Dendroid,
	GoldenDragon,
	Imp,
	Skeleton,
	Gnoll,

	Num
};

inline BSTR ToString(Architecture arch) 
{
	switch (arch) 
	{
	case Architecture::FloorTile:
		return LOCALIZED_TEXT("floor");
	case Architecture::WallTile:
		return LOCALIZED_TEXT("wall");
	case Architecture::StairsDownTile:
		return LOCALIZED_TEXT("stairs_down");
	case Architecture::StairsUpTile:
		return LOCALIZED_TEXT("stairs_up");
	case Architecture::StairsAirTile:
		return LOCALIZED_TEXT("stairs_air");
	case Architecture::StairsFireTile:
		return LOCALIZED_TEXT("stairs_fire");
	case Architecture::StairsEarthTile:
		return LOCALIZED_TEXT("stairs_earth");
	case Architecture::StairsWaterTile:
		return LOCALIZED_TEXT("stairs_water");
	default:
		return LOCALIZED_TEXT("invalid");
	}
}

inline TileTypes ToTiletype(EntityTypes ent)
{
	switch (ent) 
	{
	case EntityTypes::PlayerEntity:
		return TileTypes::Player;
	case EntityTypes::GoblinEntity:
		return TileTypes::Goblin;
	case EntityTypes::OrcEntity:
		return TileTypes::Orc;
	case EntityTypes::DrakeEntity:
		return TileTypes::Drake;
	case EntityTypes::GriffinEntity:
		return TileTypes::Griffin;
	case EntityTypes::AngelEntity:
		return TileTypes::Angel;
	case EntityTypes::DwarfEntity:
		return TileTypes::Dwarf;
	case EntityTypes::DendroidEntity:
		return TileTypes::Dendroid;
	case EntityTypes::GoldenDragonEntity:
		return TileTypes::GoldenDragon;
	case EntityTypes::ImpEntity:
		return TileTypes::Imp;
	case EntityTypes::SkeletonEntity:
		return TileTypes::Skeleton;
	case EntityTypes::GnollEntity:
		return TileTypes::Gnoll;
	default:
		MessageBox(nullptr, L"Trying to convert invalid entity to tile type!", L"Error", MB_OK | MB_ICONERROR);
		return TileTypes::Floor;
	}
}

inline TileTypes ToTileType(Architecture arch) 
{
	switch (arch) 
	{
	case Architecture::FloorTile:
		return TileTypes::Floor;
	case Architecture::WallTile:
		return TileTypes::Wall;
	case Architecture::StairsDownTile:
		return TileTypes::StairsDown;
	case Architecture::StairsUpTile:
		return TileTypes::StairsUp;
	default:
		MessageBox(nullptr, L"trying to convert invalid architecture to tile type!", L"Error", MB_OK | MB_ICONERROR);
		return TileTypes::Floor;
	}
}

struct TileToDraw 
{
	int X;
	int Y;
	TileTypes Type;

	TileToDraw() = default;
	TileToDraw(int NewX, int NewY, TileTypes NewType) : X(NewX), Y(NewY), Type(NewType) { ; };
};

struct Position 
{
	int X;
	int Y;

	Position() = default;
	Position(int NewX, int NewY) : X(NewX), Y(NewY) { ; };

	bool operator==(const Position& New) const 
	{
		return this->X == New.X && this->Y == New.Y;
	}

	bool operator!=(const Position& New) const 
	{
		return this->X != New.X || this->Y != New.Y;
	}
};



struct HoverInfo 
{
	BSTR Name;
	int CurrentHp;
	int MaxHp;

	HoverInfo(BSTR NewName, int NewCurrentHP, int NewMaxHP) : CurrentHp(NewCurrentHP), MaxHp(NewMaxHP), Name(NewName) { ; };
};


