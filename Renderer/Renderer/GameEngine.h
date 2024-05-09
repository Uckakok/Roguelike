#pragma once

#include <Windows.h>
#include "Defines.h"
#include "GraphicHandler.h"
#include "Level.h"
#include <string>


//main window initialization.
typedef void(*WindowHwndCallback)(HWND);

//hover info callback
typedef void(*HoverInfoCallback)(const char* Name, int CurrentHP, int MaxHP);

typedef void (*ShowUseCallback)(bool bShow);

// Function prototypes
extern "C" __declspec(dllexport) void InitializeGame(WindowHwndCallback);

extern "C" __declspec(dllexport) void GameTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback);

extern "C" __declspec(dllexport) void UseActivated();


class GameEngine {
private:
    bool bShouldUse;

    // Define a global variable to hold the callback function pointer
    HoverInfoCallback g_HoverCallback = nullptr;
    WindowHwndCallback g_WindowCallback = nullptr;
    ShowUseCallback g_ShowUseCallback = nullptr;
    graphicalInterface* windowContext;
    DungeonLevel CurrentDungeon;

    GameEngine() = default;

    // Private destructor to prevent deletion
    ~GameEngine() = default;

    // Private copy constructor to prevent copying
    GameEngine(const GameEngine&) = delete;

    // Private assignment operator to prevent assignment
    GameEngine& operator=(const GameEngine&) = delete;

    // Static instance of the class
    static GameEngine* instance;
    std::string PlayerName = "test";

public:
    void LoadLevel(int LevelNumber);
    static GameEngine* GetInstance();
    void InitializeEngine(WindowHwndCallback WindowCallback);
    void PrepareMap();
    void RunTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback);
    void TurnOnShouldUse();
};
