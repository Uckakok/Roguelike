#pragma once
#include<string>
#include"Texture.h"

#define LEVEL_SIZE	32
#define TILE_MIDDLE	32
#define EXTENSION	".XDD"


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
};

enum TileTypes {
	Floor,
	Wall,
	Sword,
	Bow,
	Player,
	Goblin,
	Highlight,
	StairsDown,
	StairsUp,

	Num
};

inline std::string ToString(Architecture arch) {
	switch (arch) {
	case Architecture::FloorTile:
		return "floor";
	case Architecture::WallTile:
		return "wall";
	case Architecture::StairsDownTile:
		return "stairs down";
	case Architecture::StairsUpTile:
		return "stairs up";
	default:
		return "invalid";
	}
}

inline std::string ToString(EntityTypes ent) {
	switch (ent) {
	case EntityTypes::PlayerEntity:
		return "Player";
	case EntityTypes::GoblinEntity:
		return "Goblin";
	default:
		return "invalid";
	}
}

inline TileTypes ToTiletype(EntityTypes ent) {
	switch (ent) {
	case EntityTypes::PlayerEntity:
		return TileTypes::Player;
	case EntityTypes::GoblinEntity:
		return TileTypes::Goblin;
	default:
		MessageBox(nullptr, L"trying to convert invalid entity to tile type!", L"Error", MB_OK | MB_ICONERROR);
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
	std::string Name;
	int CurrentHP;
	int MaxHP;
	HoverInfo(std::string NewName, int NewCurrentHP, int NewMaxHP) : Name(NewName), CurrentHP(NewCurrentHP), MaxHP(NewMaxHP) { ; };
};

