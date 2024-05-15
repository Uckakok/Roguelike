
#include "pch.h"
#include "GameEngine.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLEW_STATIC

#include <iostream>
#include "glew.h"
#include <gl/GL.h>
#include "glfw3.h"
#include<filesystem>
#include "glfw3native.h"
#include <gl/GLU.h>
#include "MonsterLibrary.h"
#include "LocalizationManager.h"
#include <fstream>


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
    LocalizationManager::GetInstance()->LoadAllLocalizedStrings();
    MonsterManager::GetInstance()->ReadMonsterDataFromFile();

    std::string LevelName;

    std::string SaveSlot = PlayerName; //todo: add support for multiple saved characters
    SaveSlot.append(EXTENSION);

    std::ifstream file(SaveSlot); 
    if (!file.is_open()) {
        //player died. Generate new player save
        std::ofstream NewFile(SaveSlot);
        if (!NewFile.is_open()) {
            MessageBox(nullptr, L"Can't create a new save slot! Critical error", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        NewFile << PlayerName << '1' << EXTENSION; //create first level
        CurrentDungeon.LevelIndex = 1;
        CurrentDungeon.GenerateMap();
        PlayerCharacter = Entity(MonsterManager::GetInstance()->GetMonster(EntityTypes::PlayerEntity));
        CurrentDungeon.SpawnPlayer(true, &PlayerCharacter);
        windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
        windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());
        return;
    }
    else {
        file >> LevelName;
    }

    if (!CurrentDungeon.LoadMapFromSave(LevelName)) {
        MessageBox(nullptr, L"Failed to load save, aborting.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    size_t digitPosition = LevelName.find_first_of("0123456789");
    if (digitPosition != std::string::npos) {
        std::string levelNumberString = LevelName.substr(digitPosition);
        int levelNumber = std::stoi(levelNumberString);
        CurrentDungeon.LevelIndex = levelNumber;
    }
    else {
        MessageBox(nullptr, L"No level number found in the file name. Aborting", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
    windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());
}

void GameEngine::SavePlayerState()
{
    std::string SaveName = PlayerName;
    SaveName.append(EXTENSION);


    std::ofstream file(SaveName);
    if (!file.is_open()) {
        std::wstring errorMessage = L"Failed to save player data: " + std::wstring(SaveName.begin(), SaveName.end());
        MessageBox(nullptr, errorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    file << PlayerName << CurrentDungeon.LevelIndex << EXTENSION;

    file.close();
}

std::string GameEngine::GetPlayerName()
{
    return PlayerName;
}

void GameEngine::LoadLevel(int LevelNumber)
{
    std::string LevelName = PlayerName;
    LevelName.append(std::to_string(LevelNumber));
    LevelName.append(EXTENSION);

    int PreviousIndex = CurrentDungeon.LevelIndex;

    PlayerCharacter = *CurrentDungeon.GetPlayer();
    CurrentDungeon.RemovePlayer();

    CurrentDungeon.SaveMapToSave();

    CurrentDungeon.LevelIndex = LevelNumber;
    if (!CurrentDungeon.LoadMapFromSave(LevelName)) {
        CurrentDungeon.GenerateMap();
        //MessageBox(nullptr, L"Failed to load save, aborting.", L"Error", MB_OK | MB_ICONERROR);
    }
    CurrentDungeon.SpawnPlayer(PreviousIndex < LevelNumber, &PlayerCharacter);
    windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
    windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());

    SavePlayerState();
    CurrentDungeon.SaveMapToSave();
}

void GameEngine::RunTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback, LoggerCallback NewLoggerCallback)
{
    
    g_HoverCallback = NewHoverCallback;
    g_ShowUseCallback = NewShowUseCallback;
    g_LoggerCallback = NewLoggerCallback;
    Position PlayerMove;
    windowContext->windowUpdate();
    HoverInfo Info = CurrentDungeon.ConstructHoverInfo(windowContext->GetCursorHoverPosition());

    if (g_HoverCallback) {
        g_HoverCallback(Info.Name, Info.CurrentHP, Info.MaxHP);
    }

    if (CurrentDungeon.GetGameEnded()) {
        return;
    }

    //try to consume use
    if (bShouldUse) {
        bShouldUse = false;
        CurrentDungeon.UseCurrentObject();
        windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
        windowContext->windowUpdate();
        if (g_ShowUseCallback) {
            if (CurrentDungeon.IsUseAvailable()) {
                g_ShowUseCallback(true);
            }
            else {
                g_ShowUseCallback(false);
            }
        }
    }

    PlayerMove = windowContext->GetClickPosition();
    if (PlayerMove.x == -1) return;

    //handle action

    if (CurrentDungeon.PerformAction(PlayerMove)) {
        CurrentDungeon.PerformEntitiesTurn();
        windowContext->newTilesToDraw(CurrentDungeon.GatherTilesForRender());
        if (!CurrentDungeon.GetGameEnded()) {
            windowContext->NewPlayerCoords(CurrentDungeon.GetPlayerPosition());
            if (g_ShowUseCallback) {
                if (CurrentDungeon.IsUseAvailable()) {
                    g_ShowUseCallback(true);
                }
                else {
                    g_ShowUseCallback(false);
                }
            }
        }
        else {
            windowContext->windowUpdate();
            //delete all player saves
            
            for (const auto& entry : std::filesystem::directory_iterator(".")) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find(PlayerName) == 0 && filename.find(EXTENSION) == filename.size() - std::string(EXTENSION).size()) {
                        std::error_code ec;
                        std::filesystem::remove(entry, ec);
                        if (ec) {
                            MessageBox(nullptr, L"Couldn't delete one of the player files", L"Error", MB_OK | MB_ICONERROR);
                        }
                    }
                }
            }
        }
    }
}

void GameEngine::TurnOnShouldUse()
{
    bShouldUse = true;
}

void GameEngine::AppendLogger(BSTR NewLog)
{
    if (g_LoggerCallback) {
        g_LoggerCallback(NewLog);
    }
}

// Function to initialize game and pass the callback function
extern "C" __declspec(dllexport) void InitializeGame(WindowHwndCallback WindowCallback)
{
    GameEngine::GetInstance()->InitializeEngine(WindowCallback);
}

extern "C" __declspec(dllexport) void GameTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback, LoggerCallback NewLoggerCallback)
{
    GameEngine::GetInstance()->RunTick(NewHoverCallback, NewShowUseCallback, NewLoggerCallback);
}

extern "C" __declspec(dllexport) void UseActivated()
{
    GameEngine::GetInstance()->TurnOnShouldUse();
}

extern "C" __declspec(dllexport) BSTR GetTranslation(const char* Key)
{
    return LOCALIZED_TEXT(Key);
}

extern "C" __declspec(dllexport) void ChangeLanguage(const char* Language)
{
    LocalizationManager::GetInstance()->ChangeLanguage(Language);
}
