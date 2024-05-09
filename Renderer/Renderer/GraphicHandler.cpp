#pragma once
#include "pch.h"
#include"GraphicHandler.h"
#include"glew.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include"glm/gtc/matrix_transform.hpp"
#include<vector>
#include<cmath>


const float disBetweenSquares = 50;

//funkcja wywo�ywana przy klikni�ciu myszk�
void graphicalInterface::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		ClickCoordinates = GetCursorHoverPosition();
	}
}


graphicalInterface::graphicalInterface() :va(NULL), ib(NULL) {

	if (!glfwInit()) {
		MessageBox(NULL, L"Can't open the window. Critical error", L"Error", MB_OK | MB_ICONERROR);
		__debugbreak();
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(640, 480, "Roguelike", NULL, NULL);
	if (!window)
	{
		MessageBox(NULL, L"Can't open the window. Critical error", L"Error", MB_OK | MB_ICONERROR);
		glfwTerminate();
		__debugbreak();
		return;
	}
	
}

void graphicalInterface::MakeContextCurrent()
{
	GLCall(glfwMakeContextCurrent(window));

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		MessageBox(NULL, L"Can't open the window. Critical error", L"Error", MB_OK | MB_ICONERROR);
		glfwTerminate();
		__debugbreak();
		return;
	}
}

//blendowanie kana�u alfa
void graphicalInterface::blendEnable() const
{
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void graphicalInterface::prepareVertexArray()
{
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));
}

void graphicalInterface::prepareVertexBuffer() {
	vb = new vertexBuffer(squareVertex, 4 * 4 * sizeof(float));
	layout.push(2);
	layout.push(2);
	va = new vertexArray();
	va->addBuffer(*vb, layout);
	ib = new indexBuffer(squareIndices, 6);
}

//tworzy shader i �aduje grafiki
void graphicalInterface::prepareShaders()
{
	shader = new Shader("Resources/Shaders/basic.shader");
	shader->bind();
	shader->setUniform1i("u_Texture", 0);
	background = new Texture("Resources/Textures/Background.png");
	background->bind();
	
	for (int i = 0; i < static_cast<int>(TileTypes::Num); ++i) {
		Sprites.emplace(static_cast<TileTypes>(i), Sprite(static_cast<TileTypes>(i)));
	}
}

void graphicalInterface::unbindStuff() const
{
	GLCall(glBindVertexArray(0));
	va->unbind();
	shader->unbind();
	vb->unbind();
	ib->unbind();
}

void graphicalInterface::setupCallbacks() const
{
	glfwSetWindowUserPointer(window, const_cast<graphicalInterface*>(this));
	glfwSetMouseButtonCallback(window, mouse_button_callback_wrapper);
}

//wektory do przesuwania, rzutowania i skalowania obiekt�w
void graphicalInterface::setupMatrices()
{
	translationA = new glm::vec3(320/7, 240/5.2, 0);
	translationB = new glm::vec3(100, 100, 0);
	scaleA = new glm::vec3(7, 5.2, 0);
	scaleB = new glm::vec3(0.5f, 0.5f, 0);
	proj = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
}

void graphicalInterface::windowUpdate()
{
	renderer.clear();

	shader->bind();
	{
		background->bind();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), (const glm::vec3)*translationA);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), (const glm::vec3)*scaleA);
		glm::mat4 mvp = proj * scale * model;
		shader->setUniformMat4f("u_MVP", mvp);
		renderer.draw(*va, *ib, *shader);
	}
	for (auto& tile : Tiles) {
		//todo: batch rendering for efficiency

		Sprites[tile.Type].Bind();
		float x = (tile.x - PlayerPosition.x + 5) * disBetweenSquares + MAGICAL_NUMBER;
		float y = (tile.y - PlayerPosition.y + 6) * disBetweenSquares + MAGICAL_NUMBER;
		translationB = new glm::vec3(y * 2, (480 - x) * 2, 0);
		glm::mat4 model = glm::translate(glm::mat4(1.0f), (const glm::vec3)*translationB);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), (const glm::vec3)*scaleB);
		glm::mat4 mvp = proj * scale * model;
		shader->setUniformMat4f("u_MVP", mvp);
		renderer.draw(*va, *ib, *shader); 
		delete translationB;
	}

	GLCall(glfwSwapBuffers(window));
	GLCall(glfwPollEvents());

}

void graphicalInterface::newTilesToDraw(const std::vector<TileToDraw>& newTiles)
{
	Tiles = newTiles;
}

void graphicalInterface::NewPlayerCoords(Position NewPosition)
{
	PlayerPosition = NewPosition;
}

GLFWwindow* graphicalInterface::GetWindow()
{
	return window;
}

Position graphicalInterface::GetClickPosition()
{
	Position temp = ClickCoordinates;
	ClickCoordinates = Position(-1, -1);
	return temp;
}

Position graphicalInterface::GetCursorHoverPosition()
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	Position InGameCursorPos;
	InGameCursorPos = Position((int)round((ypos - MAGICAL_NUMBER) / disBetweenSquares) - 5 + PlayerPosition.x, (int)round((xpos - MAGICAL_NUMBER) / disBetweenSquares) - 6 + PlayerPosition.y);

	return InGameCursorPos;
}
