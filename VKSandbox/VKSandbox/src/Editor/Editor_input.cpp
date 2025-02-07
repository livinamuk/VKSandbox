#include "Editor.h"
#include "Gizmo.h"
#include "BackEnd/BackEnd.h"
#include "Core/Audio.h"
#include "Core/Scene.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"

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
                    SetViewportView(i, Gizmo::GetPosition(), CameraView::FRONT);
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                }
                if (Input::KeyPressed(HELL_KEY_NUMPAD_3)) {
                    SetViewportView(i, Gizmo::GetPosition(), CameraView::BACK);
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                }
                if (Input::KeyPressed(HELL_KEY_NUMPAD_4)) {
                    SetViewportView(i, Gizmo::GetPosition(), CameraView::LEFT);
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                }
                if (Input::KeyPressed(HELL_KEY_NUMPAD_6)) {
                    SetViewportView(i, Gizmo::GetPosition(), CameraView::RIGHT);
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                }
                if (Input::KeyPressed(HELL_KEY_NUMPAD_5)) {
                    SetViewportView(i, Gizmo::GetPosition(), CameraView::TOP);
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                }
                if (Input::KeyPressed(HELL_KEY_NUMPAD_2)) {
                    SetViewportView(i, Gizmo::GetPosition(), CameraView::BOTTOM);
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                }
                if (Input::KeyPressed(HELL_KEY_C)) {
                    SetViewportView(i, Gizmo::GetPosition(), GetCameraViewByIndex(i));
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
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
                Gizmo::SetPosition(Scene::GetGameObjects()[mousePickIndex].m_transform.position);
                for (int i = 0; i < 4; i++) {
                   // Camera* camera = GetCameraByIndex(i);
                }
            }
        }

        // Wrap mouse
        if (Editor::GetViewportResizeState() == ViewportResizeState::IDLE) {
            if (Input::LeftMouseDown() || Input::RightMouseDown() || Input::MiddleMouseDown()) {
                if (Input::GetMouseX() == 0) {
                    Input::SetCursorPosition(BackEnd::GetFullScreenWidth() - 2, Input::GetMouseY());
                }
                if (Input::GetMouseX() == BackEnd::GetFullScreenWidth() - 1) {
                    Input::SetCursorPosition(1, Input::GetMouseY());
                }
                if (Input::GetMouseY() == 0) {
                    Input::SetCursorPosition(Input::GetMouseX(), BackEnd::GetFullScreenHeight() - 2);
                }
                if (Input::GetMouseY() == BackEnd::GetFullScreenHeight() - 1) {
                    Input::SetCursorPosition(Input::GetMouseX(), 1);
                }
            }
        }
    }
}