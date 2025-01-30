#pragma once
#include "Viewport.h"
#include <vector>

namespace ViewportManager {
    void Init();
    void UpdateViewports();
    Viewport* GetViewportByIndex(int32_t index);
    std::vector<Viewport>& GetViewports();
}