#pragma once
#include "Viewport.h"
#include <vector>

namespace ViewportManager {
    void Init();
    void UpdateMouseHoverStates();
    void UpdateViewports();
    Viewport* GetViewportByIndex(int32_t viewportIndex);
    std::vector<Viewport>& GetViewports();
}