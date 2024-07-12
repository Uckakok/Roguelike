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


const float g_disBetweenSquares = 50;

void GraphicalInterface::MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{
	if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS)
	{
		m_clickCoordinates = GetCursorHoverPosition();
	}
}

void GraphicalInterface::MouseButtonCallbackWrapper(GLFWwindow* Window, int Button, int Action, int Mods) 
{
	GraphicalInterface* Instance = static_cast<GraphicalInterface*>(glfwGetWindowUserPointer(Window));
	if (Instance) 
	{
		Instance->MouseButtonCallback(Window, Button, Action, Mods);
	}
}

GraphicalInterface::GraphicalInterface() :m_va(NULL), m_ib(NULL) 
{
	if (!glfwInit()) 
	{
		MessageBox(NULL, L"Can't open the window. Critical error", L"Error", MB_OK | MB_ICONERROR);
		__debugbreak();
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(640, 480, "Roguelike", NULL, NULL);
	if (!m_window)
	{
		MessageBox(NULL, L"Can't open the window. Critical error", L"Error", MB_OK | MB_ICONERROR);
		glfwTerminate();
		__debugbreak();
		return;
	}
	
}

void GraphicalInterface::MakeContextCurrent()
{
	GLCall(glfwMakeContextCurrent(m_window));

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) 
	{
		MessageBox(NULL, L"Can't open the window. Critical error", L"Error", MB_OK | MB_ICONERROR);
		glfwTerminate();
		__debugbreak();
		return;
	}
}

void GraphicalInterface::EnableBlending() const
{
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void GraphicalInterface::PrepareVertexArray()
{
	GLCall(glGenVertexArrays(1, &m_vao));
	GLCall(glBindVertexArray(m_vao));
}

void GraphicalInterface::PrepareVertexBuffer() 
{
	m_vb = new VertexBuffer(m_squareVertex, 4 * 4 * sizeof(float));
	m_layout.Push(2);
	m_layout.Push(2);
	m_va = new VertexArray();
	m_va->AddBuffer(*m_vb, m_layout);
	m_ib = new IndexBuffer(m_squareIndices, 6);
}

void GraphicalInterface::PrepareShaders()
{
	m_shader = new Shader("Resources/Shaders/basic.shader");
	m_shader->Bind();
	m_shader->SetUniform1i("u_Texture", 0);
	m_background = new Texture("Resources/Textures/Background.png");
	m_background->Bind();
	
	//load architecture and other
	for (int i = 0; i < static_cast<int>(TileTypes::Player); ++i) 
	{
		m_sprites.emplace(static_cast<TileTypes>(i), Sprite(static_cast<TileTypes>(i)));
	}
	//load monsters
	for (int i = 1; i < static_cast<int>(EntityTypes::Num); ++i)
	{
		m_sprites.emplace(static_cast<TileTypes>(static_cast<int>(TileTypes::Player) + i - 1), Sprite(static_cast<EntityTypes>(i)));
	}
}

void GraphicalInterface::UnbindObjects() const
{
	GLCall(glBindVertexArray(0));
	m_va->Unbind();
	m_shader->Unbind();
	m_vb->Unbind();
	m_ib->Unbind();
}

void GraphicalInterface::SetupCallbacks() const
{
	glfwSetWindowUserPointer(m_window, const_cast<GraphicalInterface*>(this));
	glfwSetMouseButtonCallback(m_window, MouseButtonCallbackWrapper);
}

void GraphicalInterface::SetupMatrices()
{
	m_translationA = new glm::vec3(320/7, 240/5.2, 0);
	m_translationB = new glm::vec3(100, 100, 0);
	m_scaleA = new glm::vec3(7, 5.2, 0);
	m_scaleB = new glm::vec3(0.5f, 0.5f, 0);
	m_proj = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
}

void GraphicalInterface::DrawTile(TileToDraw NewTile)
{
	m_sprites[NewTile.Type].Bind();
	float X = (NewTile.X - m_playerPosition.X + 5) * g_disBetweenSquares + TILE_MIDDLE;
	float Y = (NewTile.Y - m_playerPosition.Y + 6) * g_disBetweenSquares + TILE_MIDDLE;
	m_translationB = new glm::vec3(Y * 2, (480 - X) * 2, 0);
	glm::mat4 Model = glm::translate(glm::mat4(1.0f), (const glm::vec3)*m_translationB);
	glm::mat4 Scale = glm::scale(glm::mat4(1.0f), (const glm::vec3)*m_scaleB);
	glm::mat4 Mvp = m_proj * Scale * Model;
	m_shader->SetUniformMat4f("u_MVP", Mvp);
	m_renderer.Draw(*m_va, *m_ib, *m_shader);
	delete m_translationB;
}

void GraphicalInterface::WindowUpdate()
{
	m_renderer.Clear();

	m_shader->Bind();
	{
		m_background->Bind();
		glm::mat4 Model = glm::translate(glm::mat4(1.0f), (const glm::vec3)*m_translationA);
		glm::mat4 Scale = glm::scale(glm::mat4(1.0f), (const glm::vec3)*m_scaleA);
		glm::mat4 Mvp = m_proj * Scale * Model;
		m_shader->SetUniformMat4f("u_MVP", Mvp);
		m_renderer.Draw(*m_va, *m_ib, *m_shader);
	}
	
	for (auto& MapTile : m_tiles) 
	{
		DrawTile(MapTile);
	}

	for (auto& MapTile : m_entitiesToDraw)
	{
		DrawTile(MapTile);
	}

	GLCall(glfwSwapBuffers(m_window));
	GLCall(glfwPollEvents());
}

void GraphicalInterface::NewEntitiesToDraw(const std::vector<TileToDraw>& NewEntities)
{
	m_entitiesToDraw = NewEntities;
}

void GraphicalInterface::NewTilesToDraw(const std::vector<TileToDraw>& NewTiles)
{
	m_tiles = NewTiles;
}

void GraphicalInterface::NewPlayerCoords(Position NewPosition)
{
	m_playerPosition = NewPosition;
}

GLFWwindow* GraphicalInterface::GetWindow() const
{
	return m_window;
}

Position GraphicalInterface::GetClickPosition()
{
	Position Temp = m_clickCoordinates;
	m_clickCoordinates = Position(-1, -1);
	return Temp;
}

Position GraphicalInterface::GetCursorHoverPosition()
{
	double XPos, YPos;
	glfwGetCursorPos(m_window, &XPos, &YPos);
	Position InGameCursorPos;
	InGameCursorPos = Position((int)round((YPos - TILE_MIDDLE) / g_disBetweenSquares) - 5 + m_playerPosition.X, (int)round((XPos - TILE_MIDDLE) / g_disBetweenSquares) - 6 + m_playerPosition.Y);

	return InGameCursorPos;
}
