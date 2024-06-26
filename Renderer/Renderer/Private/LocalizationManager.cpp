#include "pch.h"
#include "LocalizationManager.h"
#include <fstream>

LocalizationManager* LocalizationManager::m_instance = nullptr;

void LocalizationManager::ChangeLanguage(std::string NewLanguage)
{
	m_currentLanguage = NewLanguage;
    LoadAllLocalizedStrings();
}

void LocalizationManager::LoadAllLocalizedStrings()
{
    m_localizedStringCache.clear();
    std::string FileName = LOCALIZATION_DIR;
    FileName.append(m_currentLanguage);
    FileName.append(".po");

    std::ifstream File(FileName);
    if (!File.is_open()) 
    {
        MessageBox(nullptr, L"Can't find .po file with translations", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    std::string Line;
    std::string Key, Value;
    while (std::getline(File, Line))
    {
        if (Line.substr(0, 5) == "msgid")
        {
            Key = Line.substr(7);
            Key.erase(std::remove(Key.begin(), Key.end(), '\"'), Key.end());
        }
        else if (Line.substr(0, 6) == "msgstr") 
        {
            Value = Line.substr(8);
            Value.erase(std::remove(Value.begin(), Value.end(), '\"'), Value.end());
            m_localizedStringCache[Key] = SysAllocString(ConvertStringToWideChar(Value).c_str());
            Key.clear();
            Value.clear();
        }
    }
    File.close();
}

BSTR LocalizationManager::GetLocalizedString(std::string Key) const
{
    auto Iterator = m_localizedStringCache.find(Key);
    if (Iterator != m_localizedStringCache.end()) 
    {
        return Iterator->second;
    }
    else 
    {
        //log missing translation and just return the converted key
        std::ofstream LogFile("missing_translations.log", std::ios_base::app);
        if (LogFile.is_open()) 
        {
            LogFile << "Missing translation: Key=\"" << Key << "\", Language=\"" << m_currentLanguage << "\"\n";
            LogFile.close();
        }

        return SysAllocString(ConvertStringToWideChar(Key).c_str());
    }
}

LocalizationManager* LocalizationManager::GetInstance()
{
    if (!m_instance) 
    {
        m_instance = new LocalizationManager();
    }
    return m_instance;
}

BSTR LocalizationManager::Format(std::wstring Main, ...) const
{
    va_list Args;
    va_start(Args, Main);

    int BufferSize = _vscwprintf(Main.c_str(), Args) + 1;
    std::vector<wchar_t> Buffer(BufferSize);
    vswprintf_s(&Buffer[0], BufferSize, Main.c_str(), Args);
    va_end(Args);

    return SysAllocString(&Buffer[0]);
}

std::wstring LocalizationManager::ConvertStringToWideChar(const std::string& str) const
{
    int Length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring WideString(Length, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &WideString[0], Length);
    return WideString;
}