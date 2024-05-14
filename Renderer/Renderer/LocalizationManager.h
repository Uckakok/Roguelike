#pragma once

#include <unordered_map>
#include <string>
#include <comutil.h>
#define LOCALIZATION_DIR "Resources/Localization/"

#define LOCALIZED_TEXT(key) LocalizationManager::GetInstance()->GetLocalizedString(key)


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

    std::string CurrentLanguage = "pl";

    std::wstring ConvertStringToWideChar(const std::string& str);

public:
    void ChangeLanguage(std::string NewLanguage);
    void LoadAllLocalizedStrings();
    BSTR GetLocalizedString(std::string Key);
    static LocalizationManager* GetInstance();
};