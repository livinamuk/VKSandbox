#pragma once
#include "HellTypes.h"
#include <map>

struct Map {
    void SetName(const std::string name);
    void SetWidth(int width);
    void SetDepth(int depth);
    void SetSpawnCoords(int x, int z);
    void SetSectorLocation(int x, int z, const std::string& sectorName);
    void ClearSector(int x, int z);

    const std::string& GetName()                        { return m_name; };
    uint32_t GetWidth()                                 { return m_width; };
    uint32_t GetDepth()                                 { return m_depth; };
    uint32_t GetSpawnXCoord()                           { return m_spawnCoords.x; };
    uint32_t GetSpawnZCoord()                           { return m_spawnCoords.z; };
    std::map<ivecXZ, std::string>& GetSectorLocations() { return m_sectorLocations; };

private:
    std::string m_name;
    uint32_t m_width = 4;
    uint32_t m_depth = 4;
    ivecXZ m_spawnCoords = ivecXZ(0, 0);
    std::map<ivecXZ, std::string> m_sectorLocations;
};