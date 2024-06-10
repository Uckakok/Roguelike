#include"pch.h"
#include"Renderer.h"
#include<iostream>
#include"pch.h"

//usuwanie b³êdów openGL
void GLClearError() {
	for (int i = 0; i < GLAD_GET_ERROR_MAX; ++i) {
		while (glGetError() == GL_NO_ERROR)
			break;;
	}
}

//wypisuje wszystkie b³êdy openGL
bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		char errorMessage[512];
			sprintf_s(errorMessage, "OpenGL error in file %s at line %d: %x", file, line, error);
			MessageBoxA(NULL, errorMessage, "OpenGL Error", MB_OK | MB_ICONERROR);
			return false;
	}
	return true;
}

void Renderer::clear() const {
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::draw(const vertexArray& va, const indexBuffer& ib, const Shader& shader) const {
	shader.bind();

	va.bind();
	ib.bind();

	GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}