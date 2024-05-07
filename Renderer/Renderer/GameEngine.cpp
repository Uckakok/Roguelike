
#include "pch.h"
#include "GameEngine.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLEW_STATIC

#include <iostream>
#include "glew.h"
#include <gl/GL.h>
#include "glfw3.h"
#include "glfw3native.h"
#include <gl/GLU.h>


GameEngine* GameEngine::instance = nullptr;

GameEngine* GameEngine::GetInstance()
{
    if (!instance) {
        instance = new GameEngine();
    }
    return instance;
}

void GameEngine::InitializeEngine(WindowHwndCallback WindowCallback)
{
    // Store the callback function pointer
    g_WindowCallback = WindowCallback;

    windowContext = new graphicalInterface();

    if (g_WindowCallback) {
        HWND hWnd = glfwGetWin32Window(windowContext->GetWindow());
        g_WindowCallback(hWnd);
    }

    windowContext->MakeContextCurrent();
    PrepareMap();
}

void GameEngine::PrepareMap()
{
    windowContext->blendEnable();
    windowContext->prepareVertexArray();
    windowContext->prepareVertexBuffer();
    windowContext->prepareShaders();
    windowContext->unbindStuff();
    windowContext->setupCallbacks();
    windowContext->setupMatrices();

    //todo: move somewhere else, added for testing
    std::string LevelName = "test.XDD";
    if (!CurrentDungeon.LoadMapFromSave(LevelName)) {
        MessageBox(nullptr, L"Failed to load save, render thread is aborting.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
    windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());
}

void GameEngine::RunTick(HoverInfoCallback NewHoverCallback)
{
    g_HoverCallback = NewHoverCallback;
    Position PlayerMove;
    windowContext->windowUpdate();
    HoverInfo Info = CurrentDungeon.ConstructHoverInfo(windowContext->GetCursorHoverPosition());

    if (g_HoverCallback) {
        g_HoverCallback(Info.Name.c_str(), Info.CurrentHP, Info.MaxHP);
    }

    PlayerMove = windowContext->GetClickPosition();
    if (PlayerMove.x == -1) return;

    //handle action

    if (CurrentDungeon.PerformAction(PlayerMove)) {
        CurrentDungeon.PerformEntitiesTurn();
        windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
        if (!CurrentDungeon.GetGameEnded()) {
            windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());
        }
        if (CurrentDungeon.GetGameEnded()) {
            windowContext->windowUpdate();
            return;
        }
    }
}

// Function to initialize game and pass the callback function
extern "C" __declspec(dllexport) void InitializeGame(WindowHwndCallback WindowCallback)
{
    GameEngine::GetInstance()->InitializeEngine(WindowCallback);
}

extern "C" __declspec(dllexport) void GameTick(HoverInfoCallback NewHoverCallback)
{
    GameEngine::GetInstance()->RunTick(NewHoverCallback);
}
