#include "Editor/Editor.h"

#include "Core/Audio.h"
#include "ImGui/EditorImgui.h"
#include "Renderer/Renderer.h"
#include "World/MapManager.h"
#include "World/World.h"

namespace Editor {

    void OpenMapEditor() {
        Audio::PlayAudio(AUDIO_SELECT, 1.0f);

        if (Editor::IsEditorClosed()) {
            Editor::OpenEditor();
        }

        if (Editor::GetEditorMode() != EditorMode::MAP_EDITOR) {
            Editor::SetEditorMode(EditorMode::MAP_EDITOR);
        }

        if (World::GetCurrentMapCreateInfo() == nullptr) {
            World::ResetWorld();
            EditorImGui::ShowOpenFileWindow("OpenMap");
        }
    }

    void UpdateMapEditor() {
        MapCreateInfo* mapCreateInfo = World::GetCurrentMapCreateInfo();
        if (!mapCreateInfo) {
            return;
        }

        float worldWidth = (mapCreateInfo->width) * 64.0f;
        float worldDepth = (mapCreateInfo->depth) * 64.0f;

        for (int x = 0; x <= mapCreateInfo->width; x++) {
            glm::vec3 p0 = glm::vec3(x * 64.0f, 0.0f, 0);
            glm::vec3 p1 = glm::vec3(x * 64.0f, 0.0f, worldDepth);
            Renderer::DrawLine(p0, p1, WHITE);
        }
        for (int z = 0; z <= mapCreateInfo->depth; z++) {
            glm::vec3 p0 = glm::vec3(0.0f, 0.0f, z * 64.0f);
            glm::vec3 p1 = glm::vec3(worldWidth, 0.0f, z * 64.0f);
            Renderer::DrawLine(p0, p1, WHITE);
        }
    }
}