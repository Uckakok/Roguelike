#pragma once
#include<string>
#include"Texture.h"

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
	Highlight,

	Num
};

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
};

struct HoverInfo {
	std::string Name;
	int CurrentHP;
	int MaxHP;
	HoverInfo(std::string NewName, int NewCurrentHP, int NewMaxHP) : Name(NewName), CurrentHP(NewCurrentHP), MaxHP(NewMaxHP) { ; };
};

