#pragma once

#include"Renderer.h"

class Texture 
{
private:
	unsigned int m_rendererId;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_BPP;
public:
	Texture(const std::string& Path);
	~Texture();
	void Bind(unsigned int Slot = 0) const;
	void Unbind() const;
	inline int GetWidth() const { return m_width; };
	inline int GetHeight() const { return m_height; };
};