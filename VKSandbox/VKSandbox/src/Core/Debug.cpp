#include "Debug.h"
#include "Camera.h"
#include "../BackEnd/BackEnd.h"
#include "../UI/UIBackEnd.h"
#include "../Util.hpp"

namespace Debug {
    std::string g_text = "";

    void Update() {
        AddText("CamPos: " + Util::Vec3ToString(Camera::GetViewPos()));
        AddText("MousePick: " + std::to_string(BackEnd::GetMousePickR()) + ", " + std::to_string(BackEnd::GetMousePickG()));

    }

    void AddText(const std::string& text) {
        g_text += text + "\n";
    }

    const std::string& GetText() {
        return g_text;
    }

    void EndFrame() {
        g_text = "";
    }
}