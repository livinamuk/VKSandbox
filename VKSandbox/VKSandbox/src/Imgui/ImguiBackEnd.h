#pragma once
#include "HellTypes.h"

namespace ImGuiBackend {    
    void Init();
    void Update();
    void RecordFileMenuHeight(uint32_t height);

    bool ImGuiOwnsMouse();
    uint32_t GetFileMenuHeight();
}