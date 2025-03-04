#include "Editor.h"
#include "Gizmo.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Core/Audio.h"
#include "ImGui/EditorImgui.h"
#include "ImGui/ImGuiBackEnd.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"
#include "UI/UIBackEnd.h"

#include "Renderer/Renderer.h"

// Get me out of here
#include "World/World.h"
// Get me out of here


namespace Editor {

    void UpdateGizmoInteract();
    void UpdateSelectRect();
    void UpdateMouseWrapping();

    glm::vec3 m_selectedObjectGizmoTranslateOffset = glm::vec3(0.0f);
    glm::vec3 m_selectedObjectGizmoRotateOffset = glm::vec3(0.0f);

    // This whole function should be split into heightmap/sector/map editor update functions
    // with the relevant parts in their own function, like changing camera view etc...

    void UpdateInput() {
        // Set active viewport
        if (Input::LeftMousePressed() ||
            Input::MiddleMousePressed() ||
            Input::RightMousePressed()) {
            SetActiveViewportIndex(GetHoveredViewportIndex());
        }

        // Toggle viewport types
        if (!EditorImGui::HasKeyboardFocus()) {
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
        }

        if (GetEditorMode() == EditorMode::SECTOR_EDITOR) {
            UpdateObjectSelection();
            UpdateGizmoInteract();
            UpdateSelectRect();
            UpdateMouseWrapping();
        }
    }

    void UpdateObjectSelection() {
        uint16_t mousePickType = BackEnd::GetMousePickR();
        uint16_t mousePickIndex = BackEnd::GetMousePickG();

        SetHoveredObjectType(static_cast<EditorObjectType>(mousePickType));
        SetHoveredObjectIndex(mousePickIndex);

        if (Input::LeftMousePressed()) {

            // This prevents selecting objects under gizmo
            if (!Gizmo::HasHover()) {
                SetSelectedObjectType(GetHoveredObjectType());
                SetSelectedObjectIndex(GetHoveredObjectIndex());
            }

            if (GetSelectedObjectType() == EditorObjectType::GAME_OBJECT) {
                GameObject& gameObject = World::GetGameObjects()[GetSelectedObjectIndex()];
                if (!Gizmo::HasHover()) { // Prevents selection objects behind the gizmo
                    glm::vec3 newGizmoPosition = gameObject.GetModelMatrix()[3];
                    Gizmo::SetPosition(newGizmoPosition);
                }
                Gizmo::SetEuler(glm::vec3(0.0f, 0.0f, 0.0f));
                m_selectedObjectGizmoRotateOffset = gameObject.GetEulerRotation();
            }

            if (GetSelectedObjectType() == EditorObjectType::LIGHT) {
                Light& light = World::GetLights()[GetSelectedObjectIndex()];
                if (!Gizmo::HasHover()) { // Prevents selection objects behind the gizmo
                    Gizmo::SetPosition(light.GetPosition());
                }
                Gizmo::SetEuler(glm::vec3(0.0f, 0.0f, 0.0f));
            }

            if (GetSelectedObjectType() == EditorObjectType::PICK_UP) {
                PickUp& pickUp = World::GetPickUps()[GetSelectedObjectIndex()];
                if (!Gizmo::HasHover()) { // Prevents selection objects behind the gizmo
                    Gizmo::SetPosition(pickUp.GetPosition());
                }
                Gizmo::SetEuler(glm::vec3(0.0f, 0.0f, 0.0f));
            }

            if (GetSelectedObjectType() == EditorObjectType::TREE) {
                Tree& tree = World::GetTrees()[GetSelectedObjectIndex()];
                if (!Gizmo::HasHover()) { // Prevents selection objects behind the gizmo
                    Gizmo::SetPosition(tree.GetPosition());
                }
                Gizmo::SetEuler(glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }


        // Mark selected objects as selected, for outline rendering
        if (GetSelectedObjectType() == EditorObjectType::GAME_OBJECT) {
            GameObject& gameObject = World::GetGameObjects()[GetSelectedObjectIndex()];
            gameObject.MarkAsSelected();
        }
        if (GetSelectedObjectType() == EditorObjectType::TREE) {
            Tree& tree = World::GetTrees()[GetSelectedObjectIndex()];
            tree.MarkAsSelected();
        }
    }

    void UpdateGizmoInteract() {
        // Start translate/rotate/scale
        if (GetEditorState() == EditorState::IDLE && Gizmo::HasHover() && Input::LeftMousePressed()) {
            switch (Gizmo::GetMode()) {
            case GizmoMode::TRANSLATE:  SetEditorState(EditorState::GIZMO_TRANSLATING); break;
            case GizmoMode::ROTATE:     SetEditorState(EditorState::GIZMO_ROTATING);    break;
            case GizmoMode::SCALE:      SetEditorState(EditorState::GIZMO_SCALING);     break;
            }
        }

        // End translate/rotate/scale
        if (!Input::LeftMouseDown() && GetEditorState() == EditorState::GIZMO_TRANSLATING ||
            !Input::LeftMouseDown() && GetEditorState() == EditorState::GIZMO_ROTATING ||
            !Input::LeftMouseDown() && GetEditorState() == EditorState::GIZMO_SCALING) {
            SetEditorState(EditorState::IDLE);
        }

        // Translate the selected object
        if (GetEditorState() == EditorState::GIZMO_TRANSLATING) {

            if (GetSelectedObjectType() == EditorObjectType::GAME_OBJECT) {
                GameObject* gameObject = World::GetGameObjectByIndex(GetSelectedObjectIndex());
                if (gameObject) {
                    gameObject->m_transform.position = Gizmo::GetPosition();
                }
            }

            if (GetSelectedObjectType() == EditorObjectType::LIGHT) {
                Light* light = World::GetLightByIndex(GetSelectedObjectIndex());
                if (light) {
                    light->SetPosition(Gizmo::GetPosition());
                }
            }

            if (GetSelectedObjectType() == EditorObjectType::PICK_UP) {
                PickUp* pickUp = World::GetPickUpByIndex(GetSelectedObjectIndex());
                if (pickUp) {
                    pickUp->SetPosition(Gizmo::GetPosition());
                }
            }

            if (GetSelectedObjectType() == EditorObjectType::TREE) {
                Tree* tree = World::GetTreeByIndex(GetSelectedObjectIndex());
                if (tree) {
                    tree->SetPosition(Gizmo::GetPosition());
                }
            }
        }
        // Rotate the selected object
        if (GetEditorState() == EditorState::GIZMO_ROTATING) {
            if (GetSelectedObjectType() == EditorObjectType::GAME_OBJECT) {
                GameObject* gameObject = World::GetGameObjectByIndex(GetSelectedObjectIndex());
                if (gameObject) {
                    gameObject->m_transform.rotation = Gizmo::GetEulerRotation() + m_selectedObjectGizmoRotateOffset;
                }
            }
        }
    }

    void UpdateSelectRect() {
        const Resolutions& resolutions = Config::GetResolutions();

        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        int gBufferWidth = resolutions.gBuffer.x;
        int gBufferHeight = resolutions.gBuffer.y;

        int mappedMouseX = Util::MapRange(mouseX, 0, BackEnd::GetCurrentWindowWidth(), 0, gBufferWidth);
        int mappedMouseY = Util::MapRange(mouseY, 0, BackEnd::GetCurrentWindowHeight(), 0, gBufferHeight);

        // Object selection rectangle
        SelectionRectangleState& rectangleState = GetSelectionRectangleState();

        // Begin drag
        if (Input::LeftMousePressed() && EditorIsIdle() && EditorWasIdleLastFrame() && !ImGuiBackend::ImGuiOwnsMouse()) {
            SetEditorState(EditorState::DRAGGING_SELECT_RECT);
            rectangleState.beginX = mappedMouseX;
            rectangleState.beginY = mappedMouseY;
        }

        // Calculate other part of rectangle
        if (GetEditorState() == EditorState::DRAGGING_SELECT_RECT) {
            rectangleState.currentX = mappedMouseX;
            rectangleState.currentY = mappedMouseY;
        }

        // Clamp on screen
        rectangleState.beginX = std::max(rectangleState.beginX, 1);
        rectangleState.beginY = std::max(rectangleState.beginY, 1);
        rectangleState.currentX = std::max(rectangleState.currentX, 1);
        rectangleState.currentY = std::max(rectangleState.currentY, 1);

        // End drag
        if (!Input::LeftMouseDown() && GetEditorState() == EditorState::DRAGGING_SELECT_RECT) {
            SetEditorState(EditorState::IDLE);
        }
    }

    void UpdateMouseWrapping() {
        if (Editor::GetEditorState() == EditorState::GIZMO_TRANSLATING ||
            Editor::GetEditorState() == EditorState::GIZMO_ROTATING ||
            Editor::GetEditorState() == EditorState::GIZMO_SCALING) {

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