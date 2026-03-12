#include <fstream>
#include <sstream>
#include <direct.h>
#include <algorithm>
#include <io.h>

#include "SaveManager.h"

#include "../Game/Data/RunGameData.h"

std::string Wannabe::SaveManager::GetPath(int iSlotIdx)
{
    std::stringstream ss;
    ss << "../SavedData/Save_" << iSlotIdx << ".json";
    return ss.str();
}

bool Wannabe::SaveManager::Check(int iSlotIdx)
{
    std::ifstream File(GetPath(iSlotIdx));
    bool bIsExist = File.good();
    return bIsExist;
}

bool Wannabe::SaveManager::SaveData(int iSlotIdx, const RunGameData& data)
{
    int mkdirResult = _mkdir("../SavedData"); // 폴더가 없으면 생성

    std::string Path = GetPath(iSlotIdx);
    std::ofstream File(Path);

    if (File.is_open() == false)
        return false;

    File << std::setw(4) << data.ToJson() << std::endl;
    return true;
}

bool Wannabe::SaveManager::LoadData(int iSlotIdx, json& data)
{
    std::ifstream File(GetPath(iSlotIdx));
    if (File.is_open() == false)
        return false;

    File >> data;
    return true;
}

bool SaveManager::SaveRecord(const std::string& fileName, const RunGameData& data)
{
    int mkdirResult = _mkdir("../Records"); // 기록 보관용 폴더
    std::string path = "../Records/" + fileName + ".json";
    std::ofstream File(path);

    if (File.is_open() == false)
    {
        return false;
    }

    File << std::setw(4) << data.ToJson() << std::endl;
    return true;
}

std::vector<RunGameData> Wannabe::SaveManager::LoadAllRecords()
{
    std::vector<RunGameData> records;
    std::string recordDir = "../Records/*.json";
    _finddata_t fd;
    intptr_t handle = _findfirst(recordDir.c_str(), &fd);

    if (handle != -1)
    {
        do
        {
            // 파일명 결합
            std::string fileName = "../Records/";
            fileName += fd.name;

            std::ifstream file(fileName);
            if (file.is_open())
            {
                json j;
                file >> j;

                RunGameData data;

                data.FromJson(j);
                records.push_back(data);
            }
        } while (_findnext(handle, &fd) == 0);

        _findclose(handle);
    }

    std::sort(records.begin(), records.end(), [](const auto& a, const auto& b) 
    {
        return a.m_iKillCount > b.m_iKillCount;
    });

    return records;
}
