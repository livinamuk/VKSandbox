#include "Editor.h"
#include "Gizmo.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Core/Audio.h"
#include "Core/Scene.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"
#include "UI/UIBackEnd.h"

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




        const Resolutions& resolutions = Config::GetResolutions();

        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        int gBufferWidth = resolutions.gBuffer.x;
        int gBufferHeight = resolutions.gBuffer.y;

        int mappedMouseX = Util::MapRange(mouseX, 0, BackEnd::GetWindowedWidth(), 0, gBufferWidth);
        int mappedMouseY = Util::MapRange(mouseY, 0, BackEnd::GetWindowedHeight(), 0, gBufferHeight);

        // Object selection rectangle
        ViewportSelectionRectangleState& rectangleState = GetViewportSelectionRectangleState();

        // Begin drag
        if (Input::LeftMousePressed() && !rectangleState.dragging) {
            rectangleState.dragging = true;
            rectangleState.beginX = mappedMouseX;
            rectangleState.beginY = mappedMouseY;
        }

        // Calculate other part of rectangle
        if (rectangleState.dragging) {
            rectangleState.currentX = mappedMouseX;
            rectangleState.currentY = mappedMouseY;
        }

        // End drag
        if (!Input::LeftMouseDown()) {
            rectangleState.dragging = false;
        }
        // Still dragging? Then draw it
        else {
            glm::ivec2 location = glm::ivec2(0, 0);
            location.x = rectangleState.beginX;
            location.y = rectangleState.beginY;

            glm::ivec2 size = glm::ivec2(0, 0);
            size.x = rectangleState.currentX - rectangleState.beginX;
            size.y = rectangleState.currentY - rectangleState.beginY;

            UIBackEnd::BlitTexture("DefaultRMA", location, Alignment::TOP_LEFT, glm::vec4(1, 1, 1, 0.5f), size);
        }

    }
}