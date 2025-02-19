#pragma once
#include "HellTypes.h"

struct Sector {
    void ResetData();


private:
    uint32_t m_heightMapIndex = 0;
};