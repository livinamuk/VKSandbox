#include "Debug.h"
#include "Util.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Game.h"
#include "../Editor/Editor.h"
#include "../Renderer/RenderDataManager.h"
#include "../Viewport/ViewportManager.h"
#include "../UI/UIBackEnd.h"

namespace Debug {
    std::string g_text = "";
    bool g_showDebugText = false;

    void Update() {
        if (!g_showDebugText) return;

        //AddText("MousePick: " + std::to_string(BackEnd::GetMousePickR()) + ", " + std::to_string(BackEnd::GetMousePickG()));
        //AddText("Selected Object: " + Editor::EditorObjectTypeToString(Editor::GetSelectedObjectType()));
        //AddText("");

        for (int i = 0; i < 4; i++) {
            const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
            AddText("ProjectionMatrix: \n" + Util::Mat4ToString(viewportData[i].projection) + "\n");
            AddText("ViewMatrix: \n" + Util::Mat4ToString(viewportData[i].view) + "\n");
        }

        for (int i = 0; i < 4; i++) {
            AddText(std::to_string(i) + ": " + std::to_string(ViewportManager::GetViewportByIndex(i)->IsVisible()));
        }
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

    void ToggleDebugText() {
        g_showDebugText = !g_showDebugText;
    }

    bool IsDebugTextVisible() {
        return g_showDebugText;
    }
}