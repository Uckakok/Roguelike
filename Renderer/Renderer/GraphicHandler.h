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
#include"Renderer.h"


class graphicalInterface {
private:
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	float squareVertex[16] = {
		-50.0f, -50.0f, 0.0f, 0.0f,
		 50.0f, -50.0f, 1.0f, 0.0f,
		 50.0f,  50.0f, 1.0f, 1.0f,
		-50.0f,  50.0f, 0.0f, 1.0f
	};
	unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	GLFWwindow* window;
	GLuint vao;
	vertexArray* va;
	vertexBuffer* vb;
	indexBuffer* ib;
	vertexBufferLayout layout;
	Shader* shader;
	Texture* floorTexture;
	Texture* wallTexture;
	Texture* background;
	Texture* PlayerSprite;
	Texture* bowSprite;
	Texture* swordSprite;
	Texture* GoblinSprite;
	Texture* Highlight;
	Renderer renderer;
	glm::vec3* translationA;
	glm::vec3* translationB;
	glm::vec3* scaleA;
	glm::vec3* scaleB;
	glm::mat4 proj;
	std::vector<TileToDraw> Tiles;
	Position PlayerPosition;
	Position ClickCoordinates = Position(-1, -1);
public:
	static void mouse_button_callback_wrapper(GLFWwindow* window, int button, int action, int mods) {
		graphicalInterface* instance = static_cast<graphicalInterface*>(glfwGetWindowUserPointer(window));
		if (instance) {
			instance->mouse_button_callback(window, button, action, mods);
		}
	}
	graphicalInterface();
	void MakeContextCurrent();
	void blendEnable() const;
	void prepareVertexArray();
	void prepareVertexBuffer();
	void prepareShaders();
	void unbindStuff() const;
	void setupCallbacks() const;
	void setupMatrices();
	void windowUpdate();
	void newTilesToDraw(const std::vector<TileToDraw>& newTiles);
	void NewPlayerCoords(Position NewPosition);
	GLFWwindow* GetWindow();
	Position GetClickPosition();
	Position GetCursorHoverPosition();
};