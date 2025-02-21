#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "WorldCommon.h"

struct Sector {
    void ResetData();
    void LoadSectorData(SectorData sectorData);
    void SpawnHouse(HouseData houseData);

private:
    uint32_t m_heightMapIndex = 0;
    SectorData m_sectorData;
    std::vector<HouseData> m_houseDataList;
    bool m_hasHouse = false;
};