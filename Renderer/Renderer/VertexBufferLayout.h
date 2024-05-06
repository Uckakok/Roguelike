#pragma once

#include<vector>
#include"glew.h"
#include"Renderer.h"

// Struktura definiuj¹ca element bufora wierzcho³ków
struct vertexBufferElement {
	unsigned int type;

	unsigned int count;
	unsigned char normalized;

	static unsigned int getSizeOfType(unsigned int type) {
		switch (type)
		{
		case GL_FLOAT:	return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		ASSERT(false);
		return 0;
	}
};

// Klasa definiuj¹ca uk³ad bufora wierzcho³ków
class vertexBufferLayout {
private:
	std::vector<vertexBufferElement> m_Elements;
	unsigned int m_Stride; //odleg³oœæ pomiêdzy kolejnymi wierzcho³kami
public:
	vertexBufferLayout()
		: m_Stride(0) {}

	void push(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * vertexBufferElement::getSizeOfType(GL_FLOAT);
	}

	inline const std::vector<vertexBufferElement> getElements() const { return m_Elements; }
	inline unsigned int getStride() const { return m_Stride; }
};