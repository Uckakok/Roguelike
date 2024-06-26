#pragma once

#include <Windows.h>
#include "Defines.h"
#include "GraphicHandler.h"
#include "Level.h"
#include <string>


//callback functions
typedef void(*WindowHwndCallback)(HWND);
typedef void(*HoverInfoCallback)(BSTR Name, int CurrentHP, int MaxHP);
typedef void(*ShowUseCallback)(bool bShow);
typedef void(*LoggerCallback)(BSTR LogEvent);

//exposed dll functions
extern "C" __declspec(dllexport) void InitializeGame(WindowHwndCallback);

extern "C" __declspec(dllexport) void GameTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback, LoggerCallback NewLoggerCallback);

extern "C" __declspec(dllexport) void UseActivated();

extern "C" __declspec(dllexport) BSTR GetTranslation(const char* Key);

extern "C" __declspec(dllexport) void ChangeLanguage(const char* Language);

extern "C" __declspec(dllexport) void SavePostExit();


class GameEngine
{
private:
    GameEngine() = default;

    bool m_bShouldUse;
    std::string m_playerName = "test"; //name of saves
    Entity m_PlayerCharacter; //data of player character

    //stored callbacks
    HoverInfoCallback m_hoverCallback = nullptr;
    WindowHwndCallback m_windowCallback = nullptr;
    ShowUseCallback m_showUseCallback = nullptr;
    LoggerCallback m_loggerCallback = nullptr;
    GraphicalInterface* m_windowContext;
    DungeonLevel m_currentDungeon;
    static GameEngine* m_instance;

    ~GameEngine() = default;
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    void SavePlayerState() const;
    void DeleteAllPlayerSaves() const;

public:
    void SavePostGame();
    std::string GetPlayerName() const;
    void LoadLevel(int LevelNumber);
    static GameEngine* GetInstance();
    void InitializeEngine(WindowHwndCallback WindowCallback);
    void PrepareMap();
    void RunTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback, LoggerCallback NewLoggerCallback);
    void TurnOnShouldUse();
    void AppendLogger(BSTR NewLog) const;
};
