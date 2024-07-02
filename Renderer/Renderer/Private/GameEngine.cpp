
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


GameEngine* GameEngine::m_instance = nullptr;

GameEngine* GameEngine::GetInstance()
{
    if (!m_instance)
    {
        m_instance = new GameEngine();
    }
    return m_instance;
}

void GameEngine::InitializeEngine(WindowHwndCallback WindowCallback)
{
    m_windowCallback = WindowCallback;
    m_windowContext = new GraphicalInterface();

    if (m_windowCallback)
    {
        HWND hWnd = glfwGetWin32Window(m_windowContext->GetWindow());
        m_windowCallback(hWnd);
    }

    m_windowContext->MakeContextCurrent();
    PrepareMap();
}

void GameEngine::PrepareMap()
{
    m_windowContext->EnableBlending();
    m_windowContext->PrepareVertexArray();
    m_windowContext->PrepareVertexBuffer();
    m_windowContext->PrepareShaders();
    m_windowContext->UnbindObjects();
    m_windowContext->SetupCallbacks();
    m_windowContext->SetupMatrices();
    LocalizationManager::GetInstance()->LoadAllLocalizedStrings();
    MonsterManager::GetInstance()->ReadMonsterDataFromFile();

    std::string LevelName;

    std::string SaveSlot = m_playerName; //todo: add support for multiple saved characters
    SaveSlot.append(EXTENSION);

    std::ifstream File(SaveSlot); 
    if (!File.is_open()) 
    {
        //player died. Generate new player save
        std::ofstream NewFile(SaveSlot);
        if (!NewFile.is_open()) 
        {
            MessageBox(nullptr, L"Can't create a new save slot! Critical error", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        NewFile << m_playerName << '1' << EXTENSION; //create first level
        m_currentDungeon.LevelIndex = 1;
        m_currentDungeon.GenerateMap();
        m_PlayerCharacter = Entity(MonsterManager::GetInstance()->GetMonster(EntityTypes::PlayerEntity));
        m_currentDungeon.SpawnPlayer(true, &m_PlayerCharacter);
        m_windowContext->NewTilesToDraw(m_currentDungeon.GatherTilesForRender());
        m_windowContext->NewPlayerCoords(m_currentDungeon.GetPlayerPosition());
        return;
    }
    else 
    {
        File >> LevelName;
    }

    if (!m_currentDungeon.LoadMapFromSave(LevelName)) 
    {
        MessageBox(nullptr, L"Failed to load save. Regenerating level and player", L"Error", MB_OK | MB_ICONERROR);
        size_t DigitPosition = LevelName.find_first_of("0123456789");
        if (DigitPosition != std::string::npos)
        {
            std::string LevelNumberString = LevelName.substr(DigitPosition);
            int LevelNumber = std::stoi(LevelNumberString);
            m_currentDungeon.LevelIndex = LevelNumber;
        }
        m_currentDungeon.GenerateMap();
        auto TemporaryPlayer = MonsterManager::GetInstance()->GetMonster(EntityTypes::PlayerEntity);
        m_currentDungeon.SpawnPlayer(true, &TemporaryPlayer);
    }
    else
    {
        size_t DigitPosition = LevelName.find_first_of("0123456789");
        if (DigitPosition != std::string::npos)
        {
            std::string LevelNumberString = LevelName.substr(DigitPosition);
            int LevelNumber = std::stoi(LevelNumberString);
            m_currentDungeon.LevelIndex = LevelNumber;
        }
        else
        {
            MessageBox(nullptr, L"No level number found in the file name. Aborting", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
    }
    m_windowContext->NewTilesToDraw(m_currentDungeon.GatherTilesForRender());
    m_windowContext->NewPlayerCoords(m_currentDungeon.GetPlayerPosition());
}

void GameEngine::SavePlayerState() const
{
    std::string SaveName = m_playerName;
    SaveName.append(EXTENSION);

    std::ofstream File(SaveName);
    if (!File.is_open()) 
    {
        std::wstring errorMessage = L"Failed to save player data: " + std::wstring(SaveName.begin(), SaveName.end());
        MessageBox(nullptr, errorMessage.c_str(), L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    File << m_playerName << m_currentDungeon.LevelIndex << EXTENSION;
    File.close();
}

void GameEngine::DeleteAllPlayerSaves() const
{
    for (const auto& Entry : std::filesystem::directory_iterator("."))
    {
        if (!Entry.is_regular_file())
        {
            continue;
        }
        std::string Filename = Entry.path().filename().string();
        if (Filename.find(m_playerName) == 0 && Filename.find(EXTENSION) == Filename.size() - std::string(EXTENSION).size())
        {
            std::error_code ErrorCode;
            std::filesystem::remove(Entry, ErrorCode);
            if (ErrorCode)
            {
                MessageBox(nullptr, L"Couldn't delete one of the player files", L"Error", MB_OK | MB_ICONERROR);
            }
        }
    }
}

void GameEngine::SavePostGame()
{
    if (m_currentDungeon.GetGameWon()) 
    {
        DeleteAllPlayerSaves();
    }
    else if (!m_currentDungeon.GetGameEnded()) 
    {
        SavePlayerState();
        m_currentDungeon.SaveMapToSave();
    }
}

std::string GameEngine::GetPlayerName() const
{
    return m_playerName;
}

void GameEngine::LoadLevel(int LevelNumber)
{
    std::string LevelName = m_playerName;
    LevelName.append(std::to_string(LevelNumber));
    LevelName.append(EXTENSION);

    int PreviousIndex = m_currentDungeon.LevelIndex;

    m_PlayerCharacter = *m_currentDungeon.GetPlayer();
    m_currentDungeon.RemovePlayer();
    m_currentDungeon.SaveMapToSave();
    m_currentDungeon.LevelIndex = LevelNumber;

    if (!m_currentDungeon.LoadMapFromSave(LevelName)) 
    {
        m_currentDungeon.GenerateMap();
    }
    m_currentDungeon.SpawnPlayer(PreviousIndex < LevelNumber, &m_PlayerCharacter);
    m_windowContext->NewTilesToDraw(m_currentDungeon.GatherTilesForRender());
    m_windowContext->NewPlayerCoords(m_currentDungeon.GetPlayerPosition());

    SavePlayerState();
    m_currentDungeon.SaveMapToSave();
}

void GameEngine::RunTick(HoverInfoCallback NewHoverCallback, ShowUseCallback NewShowUseCallback, LoggerCallback NewLoggerCallback)
{
    m_hoverCallback = NewHoverCallback;
    m_showUseCallback = NewShowUseCallback;
    m_loggerCallback = NewLoggerCallback;
    Position PlayerMove;
    m_windowContext->WindowUpdate();
    HoverInfo Info = m_currentDungeon.ConstructHoverInfo(m_windowContext->GetCursorHoverPosition());

    if (m_hoverCallback) 
    {
        m_hoverCallback(Info.Name, Info.CurrentHp, Info.MaxHp);
    }

    if (m_currentDungeon.GetGameEnded()) 
    {
        return;
    }

    //try to consume use
    if (m_bShouldUse) 
    {
        m_bShouldUse = false;
        m_currentDungeon.UseCurrentObject();
        m_windowContext->NewTilesToDraw(m_currentDungeon.GatherTilesForRender());
        m_windowContext->WindowUpdate();
        if (m_showUseCallback) 
        {
            if (m_currentDungeon.IsUseAvailable()) 
            {
                m_showUseCallback(true);
            }
            else 
            {
                m_showUseCallback(false);
            }
        }
    }

    PlayerMove = m_windowContext->GetClickPosition();
    if (PlayerMove.X == -1) return;

    //handle actions
    if (m_currentDungeon.PerformAction(PlayerMove)) 
    {
        m_currentDungeon.PerformEntitiesTurn();
        m_windowContext->NewTilesToDraw(m_currentDungeon.GatherTilesForRender());
        if (!m_currentDungeon.GetGameEnded())
        {
            m_windowContext->NewPlayerCoords(m_currentDungeon.GetPlayerPosition());
            if (m_showUseCallback) 
            {
                if (m_currentDungeon.IsUseAvailable()) 
                {
                    m_showUseCallback(true);
                }
                else 
                {
                    m_showUseCallback(false);
                }
            }
        }
        else 
        {
            m_windowContext->WindowUpdate();
            //Player died
            DeleteAllPlayerSaves();
        }
    }
}

void GameEngine::TurnOnShouldUse()
{
    m_bShouldUse = true;
}

void GameEngine::AppendLogger(BSTR NewLog) const
{
    if (m_loggerCallback) 
    {
        m_loggerCallback(NewLog);
    }
}

//dll exposed functions
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

extern "C" __declspec(dllexport) void SavePostExit()
{
    GameEngine::GetInstance()->SavePostGame();
}
