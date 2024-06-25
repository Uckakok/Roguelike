#pragma once

#include <unordered_map>
#include <string>
#include <stdarg.h>
#include <comutil.h>

#define LOCALIZATION_DIR "Resources/Localization/"

#define LOCALIZED_TEXT(key) LocalizationManager::GetInstance()->GetLocalizedString(key)
#define LOCALIZED_WSTRING(key) std::wstring(LocalizationManager::GetInstance()->GetLocalizedString(key))
#define FORMAT(Main, ...) LocalizationManager::GetInstance()->format(Main, __VA_ARGS__)


class LocalizationManager
{
private:
    LocalizationManager() = default;
    ~LocalizationManager() = default;
    LocalizationManager(const LocalizationManager&) = delete;
    LocalizationManager& operator=(const LocalizationManager&) = delete;

    std::wstring ConvertStringToWideChar(const std::string& str);

    static LocalizationManager* m_instance;
    std::unordered_map<std::string, BSTR> m_localizedStringCache;
    std::string m_currentLanguage = "en";
public:
    void ChangeLanguage(std::string NewLanguage);
    void LoadAllLocalizedStrings();
    BSTR GetLocalizedString(std::string Key);
    static LocalizationManager* GetInstance();
    BSTR format(std::wstring Main, ...);
};