#include "pch.h"
#include "Sprite.h"
#include "MonsterLibrary.h"


Sprite::Sprite(EntityTypes NewType)
{
	Type = ToTiletype(NewType);
	EntityIndex = NewType;
	SpriteTexture = new Texture(GetPath());
	SpriteTexture->Bind();
}

void Sprite::Bind()
{
	SpriteTexture->Bind();
}

Sprite::Sprite(TileTypes NewType) : Type(NewType)
{
	SpriteTexture = new Texture(GetPath());
	SpriteTexture->Bind();
}

std::string Sprite::GetPath() const
{
	if (EntityIndex != EntityTypes::None)
	{
		return m_textureDirectory + MonsterManager::GetInstance()->GetData(EntityIndex).TexturePath;
	}

	switch (Type)
	{
	case TileTypes::Floor:
		return m_textureDirectory + "FloorTile.png";
	case TileTypes::Wall:
		return m_textureDirectory + "WallTile.png";
	case TileTypes::HealingPotion:
		return m_textureDirectory + "HealingPotion.png";
	case TileTypes::VitalityRune:
		return m_textureDirectory + "VitalityRune.png";
	case TileTypes::StrengthRune:
		return m_textureDirectory + "StrengthRune.png";
	case TileTypes::Highlight:
		return m_textureDirectory + "Highlight.png";
	case TileTypes::StairsDown:
		return m_textureDirectory + "StairsDown.png";
	case TileTypes::StairsUp:
		return m_textureDirectory + "StairsUp.png";
	default:
		return "";
	}
}
