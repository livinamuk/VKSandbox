#pragma once
#include "HellTypes.h"
#include <vector>

namespace RenderDataManager {
    void Update();
    const RendererData& GetRendererData();
    const std::vector<PlayerData>& GetPlayerData();
}