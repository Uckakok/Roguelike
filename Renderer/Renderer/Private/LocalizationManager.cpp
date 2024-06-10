#include "pch.h"
#include "LocalizationManager.h"
#include <fstream>

LocalizationManager* LocalizationManager::instance = nullptr;

void LocalizationManager::ChangeLanguage(std::string NewLanguage)
{
	CurrentLanguage = NewLanguage;
    LoadAllLocalizedStrings();
}

void LocalizationManager::LoadAllLocalizedStrings()
{
    LocalizedStringCache.clear();
    std::string FileName = LOCALIZATION_DIR;
    FileName.append(CurrentLanguage);
    FileName.append(".po");

    std::ifstream file(FileName);
    if (!file.is_open()) {
        MessageBox(nullptr, L"Can't find .po file with translations", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    std::string line;
    std::string key, value;
    while (std::getline(file, line)) {
        if (line.substr(0, 5) == "msgid") {
            key = line.substr(7);
            key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
        }
        else if (line.substr(0, 6) == "msgstr") {
            value = line.substr(8);
            value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
            LocalizedStringCache[key] = SysAllocString(ConvertStringToWideChar(value).c_str());
            key.clear();
            value.clear();
        }
    }
    file.close();
}

BSTR LocalizationManager::GetLocalizedString(std::string Key)
{
    auto it = LocalizedStringCache.find(Key);
    if (it != LocalizedStringCache.end()) {
        return it->second;
    }
    else {
        //log missing translation and just return the converted key
        std::ofstream logFile("missing_translations.log", std::ios_base::app);
        if (logFile.is_open()) {
            logFile << "Missing translation: Key=\"" << Key << "\", Language=\"" << CurrentLanguage << "\"\n";
            logFile.close();
        }

        return SysAllocString(ConvertStringToWideChar(Key).c_str());
    }
}

LocalizationManager* LocalizationManager::GetInstance()
{
    if (!instance) {
        instance = new LocalizationManager();
    }
    return instance;
}


std::wstring LocalizationManager::ConvertStringToWideChar(const std::string& str) {
    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wideString(length, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wideString[0], length);
    return wideString;
}