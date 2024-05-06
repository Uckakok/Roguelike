
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
#include <process.h>
#include "GraphicHandler.h"
#include "Level.h"

// Define a global variable to hold the callback function pointer
HotbarCallback g_HotbarCallback = nullptr;
WindowHwndCallback g_WindowCallback = nullptr;

TileToDraw createTileToDraw(int x, int y, TileTypes type) {
    TileToDraw tile;
    tile.x = x;
    tile.y = y;
    tile.Type = type;
    return tile;
}


void DrawLoop(graphicalInterface* windowContext)
{
    windowContext->MakeContextCurrent();
    windowContext->blendEnable();
    windowContext->prepareVertexArray();
    windowContext->prepareVertexBuffer();
    windowContext->prepareShaders();
    windowContext->unbindStuff();
    windowContext->setupCallbacks();
    windowContext->setupMatrices();

    //todo: move somewhere else, added for testing
    DungeonLevel CurrentDungeon;
    std::string LevelName = "test.XDD";
    if (!CurrentDungeon.LoadMapFromSave(LevelName)) {
        MessageBox(nullptr, L"Failed to load save, render thread is aborting.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
    windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());

    Position PlayerMove = Position(-1, -1);
    while (true)
    {
        windowContext->windowUpdate();

        PlayerMove = windowContext->GetClickPosition();
        if (PlayerMove.x == -1) continue;

        //handle action

        if (CurrentDungeon.PerformAction(PlayerMove)) {
            windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
            windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());
        }



        //MessageBox(NULL, L"Rendered a frame", L"Info", MB_OK | MB_ICONINFORMATION);
        //todo: fetch some actual pieces, and set up some frame rate instead of wasting resources


    }
}

// Function to initialize game and pass the callback function
extern "C" __declspec(dllexport) void InitializeGame(HotbarCallback NewHotbarCallback, WindowHwndCallback WindowCallback)
{

    // Store the callback function pointer
    g_HotbarCallback = NewHotbarCallback;
    g_WindowCallback = WindowCallback;


    graphicalInterface* windowContext;
    windowContext = new graphicalInterface();

    if (g_WindowCallback) {
        HWND hWnd = glfwGetWin32Window(windowContext->GetWindow());
        g_WindowCallback(hWnd);
    }

    //load some map before running draw loop

    _beginthread((void(*)(void*))DrawLoop, 0, windowContext);

    if (g_HotbarCallback) {
        g_HotbarCallback(0, static_cast<int>(ItemTypes::SwordItem));
        g_HotbarCallback(7, static_cast<int>(ItemTypes::BowItem));
    }

}
