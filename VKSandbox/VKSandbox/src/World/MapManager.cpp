#include "MapManager.h"
#include "Map.h"

namespace MapManager {

    std::vector<Map> g_maps;

    void Init() {
        g_maps.clear();

        // Load maps
        // TODO

        // Temporarily hard code a map
        Map& map = g_maps.emplace_back();
        map.SetName("TestMap");
        map.SetWidth(5);
        map.SetDepth(8);
        map.SetSpawnCoords(0, 2);
        map.SetSectorLocation(0, 0, "TestSector");
        map.SetSectorLocation(0, 1, "TestSector");
    }

    void NewMap(const std::string& filename) {

    }

    void LoadMap(const std::string& filename) {

    }

    void SaveMap(const std::string& filename) {

    }

    Map* GetMapByName(const std::string& name) {
        for (Map& map : g_maps) {
            if (map.GetName() == name) {
                return &map;
            }
        }
        return nullptr;
    }
}