#pragma once
#include<string>
#include"Texture.h"
#include <comutil.h>
#include "LocalizationManager.h"
#include<fstream>

#define LEVEL_SIZE	32
#define TILE_MIDDLE	32
#define EXTENSION	".XDD"
#define MAX_LEVEL	50
#define MONSTER_SPAWN_RANDOMIZER	6

enum ItemTypes {
	Empty,
	SwordItem,
	BowItem,
};

enum Architecture {
	FloorTile,
	WallTile,
	StairsDownTile,
	StairsUpTile,
	Visited, //for generating maps only
};


enum EntityTypes {
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
};

enum TileTypes {
	Floor,
	Wall,
	Sword,
	Bow,
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
	Highlight,
	StairsDown,
	StairsUp,

	Num
};

inline BSTR ToString(Architecture arch) {
	switch (arch) {
	case Architecture::FloorTile:
		return LOCALIZED_TEXT("floor");
	case Architecture::WallTile:
		return LOCALIZED_TEXT("wall");
	case Architecture::StairsDownTile:
		return LOCALIZED_TEXT("stairs_down");
	case Architecture::StairsUpTile:
		return LOCALIZED_TEXT("stairs_up");
	default:
		return LOCALIZED_TEXT("invalid");
	}
}

inline TileTypes ToTiletype(EntityTypes ent) {
	switch (ent) {
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

inline TileTypes ToTileType(Architecture arch) {
	switch (arch) {
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

struct Sprite {
	TileTypes Type;
	Texture* SpriteTexture;
	Sprite() = default;
	Sprite(TileTypes NewType) : Type(NewType) {
		SpriteTexture = new Texture(GetPath());
		SpriteTexture->bind();
	}
	void Bind() {
		SpriteTexture->bind();
	}
	std::string GetPath()
	{
		switch (Type) {
		case TileTypes::Floor:
			return "Resources/Textures/FloorTile.png";
		case TileTypes::Wall:
			return "Resources/Textures/WallTile.png";
		case TileTypes::Player:
			return "Resources/Textures/PlayerSprite.png";
		case TileTypes::Bow:
			return "Resources/Textures/ItemBow.png";
		case TileTypes::Sword:
			return "Resources/Textures/ItemSword.png";
		case TileTypes::Goblin:
			return "Resources/Textures/GoblinSprite.png";
		case TileTypes::Orc:
			return "Resources/Textures/OrcSprite.png";
		case TileTypes::Drake:
			return "Resources/Textures/DrakeSprite.png";
		case TileTypes::Griffin:
			return "Resources/Textures/GriffinSprite.png";
		case TileTypes::Angel:
			return "Resources/Textures/AngelSprite.png";
		case TileTypes::Dwarf:
			return "Resources/Textures/DwarfSprite.png";
		case TileTypes::Dendroid:
			return "Resources/Textures/DendroidSprite.png";
		case TileTypes::GoldenDragon:
			return "Resources/Textures/GoldenDragonSprite.png";
		case TileTypes::Imp:
			return "Resources/Textures/ImpSprite.png";
		case TileTypes::Skeleton:
			return "Resources/Textures/SkeletonSprite.png";
		case TileTypes::Gnoll:
			return "Resources/Textures/GnollSprite.png";
		case TileTypes::Highlight:
			return "Resources/Textures/Highlight.png";
		case TileTypes::StairsDown:
			return "Resources/Textures/StairsDown.png";
		case TileTypes::StairsUp:
			return "Resources/Textures/StairsUp.png";
		default:
			MessageBox(nullptr, L"Tried to load invalid sprite", L"Error", MB_OK | MB_ICONERROR);
			return "";
		}
	}

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
	bool operator!=(const Position& New) const {
		return this->x != New.x || this->y != New.y;
	}
};



struct HoverInfo {
	BSTR Name;
	int CurrentHP;
	int MaxHP;

	HoverInfo(BSTR NewName, int NewCurrentHP, int NewMaxHP) : CurrentHP(NewCurrentHP), MaxHP(NewMaxHP), Name(NewName) { ; };
};


