#pragma once

#include <unordered_map>
#include <string>
#include <stdarg.h>
#include <comutil.h>
#define LOCALIZATION_DIR "Resources/Localization/"

#define LOCALIZED_TEXT(key) LocalizationManager::GetInstance()->GetLocalizedString(key)
#define LOCALIZED_WSTRING(key) std::wstring(LocalizationManager::GetInstance()->GetLocalizedString(key))

#define FORMAT(Main, ...) LocalizationManager::GetInstance()->format(Main, __VA_ARGS__)


class LocalizationManager {
private:
    LocalizationManager() = default;
    ~LocalizationManager() = default;

    LocalizationManager(const LocalizationManager&) = delete;

    // Private assignment operator to prevent assignment
    LocalizationManager& operator=(const LocalizationManager&) = delete;

    // Static instance of the class
    static LocalizationManager* instance;

    std::unordered_map<std::string, BSTR> LocalizedStringCache;

    std::string CurrentLanguage = "en";
    std::wstring ConvertStringToWideChar(const std::string& str);
public:
    void ChangeLanguage(std::string NewLanguage);
    void LoadAllLocalizedStrings();
    BSTR GetLocalizedString(std::string Key);
    static LocalizationManager* GetInstance();

    BSTR format(std::wstring Main, ...)
    {
        va_list args;
        va_start(args, Main);

        int bufferSize = _vscwprintf(Main.c_str(), args) + 1; // Get required buffer size
        std::vector<wchar_t> buffer(bufferSize); // Create buffer
        vswprintf_s(&buffer[0], bufferSize, Main.c_str(), args); // Format string
        va_end(args);

        return SysAllocString(&buffer[0]);
    }
};