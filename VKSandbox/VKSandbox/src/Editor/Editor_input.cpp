#include "Editor.h"
#include "Gizmo.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Scene.h"
#include "../Input/Input.h"
#include "../Viewport/ViewportManager.h"

namespace Editor {

    void UpdateInput() {

        // Set active viewport
        if (Input::LeftMousePressed() ||
            Input::MiddleMousePressed() ||
            Input::RightMousePressed()) {
            SetActiveViewportIndex(GetHoveredViewportIndex());
        }

        // Toggle viewport types
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsHovered()) {
                if (Input::KeyPressed(HELL_KEY_NUMPAD_1)) {
                    SetViewportToFrontView(i, Gizmo::GetPosition());
                }
            }
        }

        // Object Selection
        uint16_t mousePickType = BackEnd::GetMousePickR();
        uint16_t mousePickIndex = BackEnd::GetMousePickG();

        SetHoveredObjectType(static_cast<EditorObjectType>(mousePickType));
        SetHoveredObjectIndex(mousePickIndex);

        if (Input::LeftMousePressed() && !Gizmo::HasHover()) {
            SetSelectedObjectType(GetHoveredObjectType());
            SetSelectedObjectIndex(GetHoveredObjectIndex());

            if (GetSelectedObjectType() == EditorObjectType::GAME_OBJECT) {
                Gizmo::SetPosition(Scene::g_gameObjects[mousePickIndex].m_transform.position);
                for (int i = 0; i < 4; i++) {
                    Camera* camera = GetCameraByIndex(i);
                }
            }
        }
    }
}