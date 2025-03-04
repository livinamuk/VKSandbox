#pragma once
#include "CreateInfo.h"

namespace SectorManager {

    void Init();
    void NewSector(const std::string& name);
    //void LoadSector();
    void SaveSector(const std::string& name);

    SectorCreateInfo* GetSectorCreateInfoByName(const std::string& name);

}