#include "SectorManager.h"
#include "Core/JSON.h"
#include "World/World.h"
#include "Util.h"

namespace SectorManager {
    std::vector<SectorCreateInfo> g_sectorCreateInfoSet;

    void Init() {
        g_sectorCreateInfoSet.clear();

        // Load sectors from disk
        std::vector<FileInfo> files = Util::IterateDirectory("res/sectors");
        for (FileInfo& file : files) {
            SectorCreateInfo sectorCreateInfo = JSON::LoadSector(file.path);
            g_sectorCreateInfoSet.emplace_back(sectorCreateInfo);
            std::cout << "Loaded sector: " << file.path << "\n";
        }
    }

    void NewSector(const std::string& name) {
        if (name == "") return;
        SectorCreateInfo& sectorCreateInfo = g_sectorCreateInfoSet.emplace_back();
        sectorCreateInfo.sectorName = name;
        sectorCreateInfo.heightMapName = "None";
        SaveSector(name);
        World::LoadSingleSector(sectorCreateInfo);
    }
    
    void SaveSector(const std::string& name) {        
        SectorCreateInfo* sectorCreateInfo = GetSectorCreateInfoByName(name);
        if (sectorCreateInfo) {
            JSON::SaveSector("res/sectors/" + name + ".json", *sectorCreateInfo);
        }
    }

    SectorCreateInfo* GetSectorCreateInfoByName(const std::string& name) {
        for (SectorCreateInfo& sectorCreateInfo : g_sectorCreateInfoSet) {
            if (sectorCreateInfo.sectorName == name) {
                return &sectorCreateInfo;
            }
        }
        return nullptr;
    }
}