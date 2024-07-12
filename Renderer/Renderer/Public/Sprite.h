#pragma once

#include "Defines.h"

class Sprite
{
private:

	const std::string m_textureDirectory = "Resources/Textures/";
public:

	TileTypes Type;
	Texture* SpriteTexture;
	EntityTypes EntityIndex = EntityTypes::None;

	Sprite() = default;
	Sprite(EntityTypes NewType);
	void Bind();
	Sprite(TileTypes NewType);
	[[nodiscard]] std::string GetPath() const;
};