#include <fstream>
#include <sstream>
#include <string>
#include <direct.h>

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