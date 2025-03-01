#include "Map.h"

void Map::SetName(const std::string name) {
    m_name = name;
}

void Map::SetWidth(int width) {
    if (width > 0) {
        m_width = width;
    }
}
void Map::SetDepth(int depth) {
    if (depth > 0) {
        m_depth = depth;
    }
}

void Map::SetSpawnCoords(int x, int z) {
    m_spawnCoords = ivecXZ(x, z);
}

void Map::SetSectorLocation(int x, int z, const std::string& sectorName) {
    m_sectorLocations[ivecXZ(x, z)] = sectorName;
}

void Map::ClearSector(int x, int z) {
    for (auto it = m_sectorLocations.begin(); it != m_sectorLocations.end(); ) {
        if (it->first.x == x && it->first.z == z) {
            it = m_sectorLocations.erase(it);
        }
        else {
            ++it;
        }
    }
}