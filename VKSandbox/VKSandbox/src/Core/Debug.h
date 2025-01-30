#pragma once
#include <string>

namespace Debug {
    void Update();
    void AddText(const std::string& text);
    void EndFrame();
    void ToggleDebugText();
    bool IsDebugTextVisible();
    const std::string& GetText();
}