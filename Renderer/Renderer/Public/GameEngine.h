#pragma once

#include <Windows.h>
#include "Defines.h"
#include "GraphicHandler.h"
#include "Level.h"
#include <string>


//main window initialization.
typedef void(*WindowHwndCallback)(HWND);

//hover info callback
typedef void(*HoverInfoCallback)(BSTR Name, int CurrentHP, int MaxHP);

typedef void(*ShowUseCallback)(bool bShow);

typedef void(*LoggerCallback)(BSTR LogEvent);

// Function prototypes
extern "C" __declspec(dllexport) void InitializeGame(WindowHwndCallback);

extern "C" __declspec(dllexport) void GameTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback, LoggerCallback NewLoggerCallback);

extern "C" __declspec(dllexport) void UseActivated();

extern "C" __declspec(dllexport) BSTR GetTranslation(const char* Key);

extern "C" __declspec(dllexport) void ChangeLanguage(const char* Language);

extern "C" __declspec(dllexport) void SavePostExit();


class GameEngine {
private:

    GameEngine() = default;

    bool bShouldUse;

    // Define a global variable to hold the callback function pointer
    HoverInfoCallback g_HoverCallback = nullptr;
    WindowHwndCallback g_WindowCallback = nullptr;
    ShowUseCallback g_ShowUseCallback = nullptr;
    LoggerCallback g_LoggerCallback = nullptr;
    graphicalInterface* windowContext;
    DungeonLevel CurrentDungeon;

    // Private destructor to prevent deletion
    ~GameEngine() = default;

    // Private copy constructor to prevent copying
    GameEngine(const GameEngine&) = delete;

    // Private assignment operator to prevent assignment
    GameEngine& operator=(const GameEngine&) = delete;

    // Static instance of the class
    static GameEngine* instance;
    std::string PlayerName = "test";

    Entity PlayerCharacter; //should be a different class, but works for now

    void SavePlayerState();

public:
    void SavePostGame();
    std::string GetPlayerName();
    void LoadLevel(int LevelNumber);
    static GameEngine* GetInstance();
    void InitializeEngine(WindowHwndCallback WindowCallback);
    void PrepareMap();
    void RunTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback, LoggerCallback NewLoggerCallback);
    void TurnOnShouldUse();
    void AppendLogger(BSTR NewLog);
};
