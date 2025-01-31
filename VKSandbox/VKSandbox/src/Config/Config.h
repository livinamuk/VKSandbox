#pragma once
#include "HellTypes.h"

namespace Config {
    void Init();
    const Resolutions& GetResolutions();
    const RendererSettings& GetRendererSettings();
    void SetDepthPeelCount(int count);
}