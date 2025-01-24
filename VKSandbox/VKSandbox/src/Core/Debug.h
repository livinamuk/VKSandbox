#pragma once
#include <string>

namespace Debug {
    void Update();
    void AddText(const std::string& text);
    const std::string& GetText();
    void EndFrame();
}