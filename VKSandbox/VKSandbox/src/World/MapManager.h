#pragma once
#include "CreateInfo.h"

namespace MapManager {
    void Init();
    void NewMap(const std::string& filename);
    void LoadMap(const std::string& filename);
    void SaveMap(const std::string& filename);

    void SetMapWidth(MapCreateInfo* createInfo, int width);
    void SetMapDepth(MapCreateInfo* createInfo, int depth);
    void SetMapName(MapCreateInfo* createInfo, const std::string& filename);
    void SetMapSectorLocation(MapCreateInfo* createInfo, int x, int z, const std::string& sectorName);
    void ClearSector(MapCreateInfo* createInfo, int x, int z);

    bool MapExists(const std::string& filename);
    uint32_t GetMapWidth(MapCreateInfo* createInfo);
    uint32_t GetMapDepth(MapCreateInfo* createInfo);
    uint32_t GetHeightMapCount(MapCreateInfo* createInfo);
    bool IsSectorAtLocation(MapCreateInfo* createInfo, int x, int z);
    const std::string& GetSectorNameAtLocation(MapCreateInfo* createInfo, int x, int z);

    MapCreateInfo* GetMapCreateInfoByName(const std::string& name);
}