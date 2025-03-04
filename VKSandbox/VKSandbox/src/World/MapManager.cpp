#include "MapManager.h"
#include "CreateInfo.h"
#include "Core/JSON.h"
#include "Editor/Editor.h"

namespace MapManager {

    std::vector<MapCreateInfo> g_mapCreateInfoSet;

    void Init() {
        g_mapCreateInfoSet.clear();

        // Load maps
        // TODO

        // Temporarily hard code a map
        if (!MapExists("TestMap")) {
            NewMap("TestMap");
        }
        MapCreateInfo* createInfo = GetMapCreateInfoByName("TestMap");
        if (createInfo) {
            SetMapWidth(createInfo, 3);
            SetMapDepth(createInfo, 4);
            SetMapSectorLocation(createInfo, 1, 0, "TestSector");
            SetMapSectorLocation(createInfo, 0, 1, "TestSector");
            SetMapSectorLocation(createInfo, 1, 1, "TestSector");
            SetMapSectorLocation(createInfo, 2, 1, "TestSector");
            SetMapSectorLocation(createInfo, 2, 2, "TestSector");
            SetMapSectorLocation(createInfo, 2, 3, "TestSector");
            SetMapSectorLocation(createInfo, 1, 3, "TestSector");
            SaveMap("TestMap");
        }
    }

    void NewMap(const std::string& filename) {
        if (!MapExists(filename)) {
            MapCreateInfo& createInfo = g_mapCreateInfoSet.emplace_back();
            SetMapName(&createInfo, filename);
            SetMapWidth(&createInfo, 2);
            SetMapDepth(&createInfo, 3);
            SaveMap(filename);
        } 
        else {
            std::cout << "MapManager::NewMap() failed: map name '" << filename << "' already exists\n";
        }
    }

    void LoadMap(const std::string& filename) {

    }

    void SaveMap(const std::string& filename) {
        MapCreateInfo* createInfo = GetMapCreateInfoByName(filename);
        if (createInfo) {
            JSON::SaveMap("res/maps/" + filename + ".json", *createInfo);
        }
    }

    void SetMapWidth(MapCreateInfo* createInfo, int width) {
        if (!createInfo) return;

        if (width > 0) {
            createInfo->width = width;
        }
        // Remove any sectors outside of that width
        for (auto it = createInfo->sectorLocations.begin(); it != createInfo->sectorLocations.end(); ) {
            if (it->first.x >= width) {
                it = createInfo->sectorLocations.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void SetMapDepth(MapCreateInfo* createInfo, int depth) {
        if (!createInfo) return;

        if (depth > 0) {
            createInfo->depth = depth;
        }
        // Remove any sectors outside of that depth
        for (auto it = createInfo->sectorLocations.begin(); it != createInfo->sectorLocations.end(); ) {
            if (it->first.z >= depth) {
                it = createInfo->sectorLocations.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void SetMapName(MapCreateInfo* createInfo, const std::string& filename) {
        if (!createInfo) return;

        createInfo->name = filename;
    }

    void SetMapSectorLocation(MapCreateInfo* createInfo, int x, int z, const std::string& sectorName) {
        if (!createInfo) return;

        createInfo->sectorLocations[ivecXZ(x, z)] = sectorName;
    }

    void ClearSector(MapCreateInfo* createInfo, int x, int z) {
        if (!createInfo) return;

        for (auto it = createInfo->sectorLocations.begin(); it != createInfo->sectorLocations.end(); ) {
            if (it->first.x == x && it->first.z == z) {
                it = createInfo->sectorLocations.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    bool MapExists(const std::string& filename) {
        for (const auto& mapCreateInfo : g_mapCreateInfoSet) {
            if (mapCreateInfo.name == filename) {
                return true;
            }
        }
        return false;
    }

    uint32_t GetMapWidth(MapCreateInfo* createInfo) {
        if (createInfo) {
            return createInfo->width;
        } 
        else{
            return 0;
        }
    }

    uint32_t GetMapDepth(MapCreateInfo* createInfo) {
        if (createInfo) {
            return createInfo->depth;
        }
        else {
            return 0;
        }
    }

    uint32_t GetHeightMapCount(MapCreateInfo* createInfo) {
        int count = 0;
        if (createInfo) {
            for (auto& sectorLoction : createInfo->sectorLocations) {
                count++;
            }
        }
        return count;
    }

    bool IsSectorAtLocation(MapCreateInfo* createInfo, int x, int z) {
        if (!createInfo) return false;

        return (createInfo->sectorLocations.find(ivecXZ(x, z)) != createInfo->sectorLocations.end());
    }

    const std::string& GetSectorNameAtLocation(MapCreateInfo* createInfo, int x, int z) {
        static const std::string empty = "";
        if (!createInfo) return empty;

        auto it = createInfo->sectorLocations.find(ivecXZ(x, z));
        if (it != createInfo->sectorLocations.end()) {
            return it->second;
        }
        return empty;
    }

    MapCreateInfo* GetMapCreateInfoByName(const std::string& name) {
        for (MapCreateInfo& createInfo : g_mapCreateInfoSet) {
            if (createInfo.name == name) {
                return &createInfo;
            }
        }
        return nullptr;
    }
}