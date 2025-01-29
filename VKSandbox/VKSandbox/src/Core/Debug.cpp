#include "Debug.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Game.h"
#include "../Editor/Editor.h"
#include "../UI/UIBackEnd.h"
#include "../Util.hpp"

namespace Debug {
    std::string g_text = "";

    void Update() {

        Player* player = Game::GetPlayerByIndex(0);
        if (player) {
            AddText("Cam Pos: " + Util::Vec3ToString(player->GetCameraPosition()));
            AddText("Cam Forward: " + Util::Vec3ToString(player->GetCameraForward()));
            AddText("MousePick: " + std::to_string(BackEnd::GetMousePickR()) + ", " + std::to_string(BackEnd::GetMousePickG()));
        }
        AddText("\nSelected Object: " + Editor::EditorObjectTypeToString(Editor::GetSelectedObjectType()));
        
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