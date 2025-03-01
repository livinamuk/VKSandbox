#pragma once
#include "Map.h"

namespace MapManager {
    void Init();
    void NewMap(const std::string& filename);
    void LoadMap(const std::string& filename);
    void SaveMap(const std::string& filename);

    Map* GetMapByName(const std::string& name);
}