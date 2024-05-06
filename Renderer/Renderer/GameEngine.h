#pragma once

#include <Windows.h>
#include "Defines.h"


//main window initialization.
typedef void(*WindowHwndCallback)(HWND);

//button callbacks
typedef void(*HotbarCallback)(int slot, int Item);

// Function prototypes
extern "C" __declspec(dllexport) void InitializeGame(HotbarCallback HotbarCallback, WindowHwndCallback);
