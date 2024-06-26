#pragma once
#include"texture.h"
#include"shader.h"
#include"indexBuffer.h"
#include"vertexBuffer.h"
#include"vertexArray.h"
#include"vertexBufferLayout.h"
#include"glm/glm.hpp"
#include"glfw3.h"
#include "Defines.h"
#include <unordered_map>
#include"Renderer.h"


class GraphicalInterface 
{
private:
	void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods);
	float m_squareVertex[16] = {
		-50.0f, -50.0f, 0.0f, 0.0f,
		 50.0f, -50.0f, 1.0f, 0.0f,
		 50.0f,  50.0f, 1.0f, 1.0f,
		-50.0f,  50.0f, 0.0f, 1.0f
	};
	unsigned int m_squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	GLFWwindow* m_window;
	GLuint m_vao;
	VertexArray* m_va;
	VertexBuffer* m_vb;
	IndexBuffer* m_ib;
	VertexBufferLayout m_layout;
	Shader* m_shader;
	
	std::unordered_map<TileTypes, Sprite> m_sprites;

	Texture* m_background;
	Renderer m_renderer;
	glm::vec3* m_translationA;
	glm::vec3* m_translationB;
	glm::vec3* m_scaleA;
	glm::vec3* m_scaleB;
	glm::mat4 m_proj;
	std::vector<TileToDraw> m_tiles;
	Position m_playerPosition;
	Position m_clickCoordinates = Position(-1, -1);
public:
	static void MouseButtonCallbackWrapper(GLFWwindow* Window, int Button, int Action, int Mods);
	GraphicalInterface();
	void MakeContextCurrent();
	void EnableBlending() const;
	void PrepareVertexArray();
	void PrepareVertexBuffer();
	void PrepareShaders();
	void UnbindObjects() const;
	void SetupCallbacks() const;
	void SetupMatrices();
	void WindowUpdate();
	void NewTilesToDraw(const std::vector<TileToDraw>& NewTiles);
	void NewPlayerCoords(Position NewPosition);
	GLFWwindow* GetWindow() const;
	[[nodiscard]] Position GetClickPosition();
	Position GetCursorHoverPosition();
};