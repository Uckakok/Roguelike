#pragma once
#include"glew.h"
#include"VertexArray.h"
#include"IndexBuffer.h"
#include"Shader.h"

#define GLAD_GET_ERROR_MAX 25
#define ASSERT(x) if (!(x)) __debugbreak();
//#ifdef DEBUG
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))
//#else
//#define GLCall(x) x
//#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
	void clear() const;
	void draw(const vertexArray& va, const indexBuffer& ib, const Shader& shader) const;
};