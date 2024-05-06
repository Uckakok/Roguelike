#pragma once

#include"VertexBuffer.h"

class vertexBufferLayout;

class vertexArray {
private:
	unsigned int m_RendererID;
public:
	vertexArray();
	~vertexArray();

	void addBuffer(const vertexBuffer& vb, const vertexBufferLayout& layout);

	void bind() const;
	void unbind() const;
};