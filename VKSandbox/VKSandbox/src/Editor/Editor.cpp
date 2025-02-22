#include "Editor.h"
#include "HellDefines.h"
#include "Gizmo.h"
#include "BackEnd/BackEnd.h"
#include "Camera/Camera.h"
#include "Config/Config.h"
#include "Core/Audio.h"
#include "Core/Debug.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"

namespace Editor {

    EditorMode g_editorMode = EditorMode::EDITOR_IS_CLOSED;
    EditorMesh g_editorMesh;
    EditorLightingSettings g_editorLightingSettings;
    int g_activeViewportIndex = 3;
    bool g_isOpen = false;
    bool g_isOrthographic[4];
    bool g_editorStateWasIdleLastFrame = true;
    bool g_editorSelectMenuVisible = false;
    float g_OrthographicSizes[4];
    float g_verticalDividerXPos = 0.2f;
    float g_horizontalDividerYPos = 0.2f;
    uint16_t g_selectedObjectIndex = 0;
    uint16_t g_hoveredObjectIndex = 0;
    EditorObjectType g_selectedObjectType = EditorObjectType::NONE;
    EditorObjectType g_hoveredObjectType = EditorObjectType::NONE;
    ShadingMode g_shadingModes[4];
    EditorViewportSplitMode g_editorViewportSplitMode = EditorViewportSplitMode::SINGLE;
    //CameraView g_cameraViews[4];
    
    float g_orthoCameraDistances[4];
    EditorState g_editorState;
    SelectionRectangleState g_viewportSelectionRectangleState;

    void Init() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            g_editorMesh.Init(glm::vec3(0.5f, 1.45f, 2.53));
            g_editorMesh.RecalculateMesh();
        }
        ResetViewports();
        ResetCameras();


        g_editorLightingSettings.lightingEnabled = true;
        g_editorLightingSettings.grassEnabled = true;
    }

    void ResetViewports() {

        const Resolutions& resolutions = Config::GetResolutions();

        // Center the viewport splits
        float editorWidth = resolutions.ui.x - EDITOR_LEFT_PANEL_WIDTH;
        float editorHeight = resolutions.ui.y - EDITOR_FILE_MENU_HEIGHT;
        float editorWidthNormalized = editorWidth / resolutions.ui.x;
        float editorHeightNormalized = editorHeight / resolutions.ui.y;
        float panelRightEdgeNormalized = EDITOR_LEFT_PANEL_WIDTH / resolutions.ui.x;
        float fileMenuHeightNormalized = EDITOR_FILE_MENU_HEIGHT / resolutions.ui.y;
        g_verticalDividerXPos = panelRightEdgeNormalized + (editorWidthNormalized * 0.5f);
        g_horizontalDividerYPos = fileMenuHeightNormalized + (editorHeightNormalized * 0.5f);

        float ORTHO_CAMERA_DISTANCE_FROM_ORIGIN = 250.0f;
        g_orthoCameraDistances[0] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[1] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[2] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[3] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;

        // Top left
        SetViewportView(0, Gizmo::GetPosition(), CameraView::RIGHT);
        ViewportManager::GetViewportByIndex(0)->SetOrthoSize(1.14594f);

        // Top right
        SetViewportView(1, Gizmo::GetPosition(), CameraView::LEFT);
        ViewportManager::GetViewportByIndex(1)->SetOrthoSize(1.1958f);

        // Bottom left
        SetViewportView(2, Gizmo::GetPosition(), CameraView::TOP);
        ViewportManager::GetViewportByIndex(2)->SetOrthoSize(1.19627f);

        // Bottom right
        SetViewportView(3, Gizmo::GetPosition(), CameraView::FRONT);
        ViewportManager::GetViewportByIndex(3)->SetOrthoSize(1.1958f);
    }

    void Update(float deltaTime) {

        if (Input::KeyPressed(HELL_KEY_TAB)) {

            if (!Editor::IsOpen()) {

                // Show editor select menu
                if (!IsEditorSelectMenuVisible()) {
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    ShowEditorSelectMenu();
                    Input::ShowCursor();
                    Input::CenterMouseCursor();
                }
                // Close editor select menu
                else {
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    CloseEditorSelectMenu();
                    Input::DisableCursor();
                }
            }
            // Close editor
            else {
                Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                Editor::Close();
            }
        }

        if (!IsOpen()) return;

        //static bool runOnce = true;
        //if (runOnce) {
        //    glm::vec3 gizmoPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        //    GameObject* mermaid = Scene::GetGameObjectByName("Mermaid");
        //    if (mermaid) {
        //        gizmoPosition = mermaid->GetPosition();
        //        Gizmo::SetPosition(gizmoPosition);
        //        SetSelectedObjectType(EditorObjectType::GAME_OBJECT);
        //        SetHoveredObjectType(EditorObjectType::GAME_OBJECT);
        //    }
        //    runOnce = false;
        //}

        g_editorStateWasIdleLastFrame = g_editorState == EditorState::IDLE;

        UpdateMouseRays();
        UpdateCamera();
        UpdateDividers();
        UpdateInput();
        UpdateUI();
        UpdateCursor();
        UpdateDebug();
        UpdateCameraInterpolation(deltaTime);
        Gizmo::Update();

        if (Input::KeyPressed(HELL_KEY_Q)) {
            if (GetEditorViewportSplitMode() == EditorViewportSplitMode::SINGLE) {
                SetEditorViewportSplitMode(EditorViewportSplitMode::FOUR_WAY_SPLIT);
                std::cout << "four way\n";
            }
            else {
                SetEditorViewportSplitMode(EditorViewportSplitMode::SINGLE);
                std::cout << "single\n";
            }
        }
    }

    void OpenEditor(EditorMode mode) {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::ShowCursor();
        Input::CenterMouseCursor();
        g_isOpen = true;
        g_editorMode = mode;
    }

    void Close() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::DisableCursor();
        g_isOpen = false;
        g_editorMode = EditorMode::EDITOR_IS_CLOSED;
    }

    void ShowEditorSelectMenu() {
        g_editorSelectMenuVisible = true;
    }

    void CloseEditorSelectMenu() {
        g_editorSelectMenuVisible = false;
    }

    void SetActiveViewportIndex(int index) {
        g_activeViewportIndex = index;
    }

    int GetActiveViewportIndex() {
        return g_activeViewportIndex;
    }

    int GetHoveredViewportIndex() {
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible() && viewport->IsHovered()) {
                return i;
            }
        }
        return 0;
    }

    bool IsOpen() {
        return g_isOpen;
    }

    bool IsEditorSelectMenuVisible() {
        return g_editorSelectMenuVisible;
    }

    bool EditorIsIdle() {
        return g_editorState == EditorState::IDLE;
    }

    bool EditorWasIdleLastFrame() {
        return g_editorStateWasIdleLastFrame;
    }

    void SetSelectedObjectIndex(int index) {
        g_selectedObjectIndex = index;
    }

    void SetHoveredObjectIndex(int index) {
        g_hoveredObjectIndex = index;
    }

    void SetSelectedObjectType(EditorObjectType editorObjectType) {
        g_selectedObjectType = editorObjectType;
    }

    void SetHoveredObjectType(EditorObjectType editorObjectType) {
        g_hoveredObjectType = editorObjectType;
    }

    void SetSplitX(float value) {
        g_verticalDividerXPos = value;
        //ViewportManager::UpdateViewports();
    }

    void SetSplitY(float value) {
        g_horizontalDividerYPos = value;
        //ViewportManager::UpdateViewports();
    }

    int GetSelectedObjectIndex() {
        return g_selectedObjectIndex;
    }

    int GetHoveredObjectIndex() {
        return g_hoveredObjectIndex;
    }

    EditorObjectType GetSelectedObjectType() {
        return g_selectedObjectType;
    }

    EditorObjectType GetHoveredObjectType() {
        return g_hoveredObjectType;
    }

    std::string EditorObjectTypeToString(const EditorObjectType& type) {
        switch (type) {
        case EditorObjectType::NONE:        return "None";
        case EditorObjectType::GAME_OBJECT: return "Game Object";
        case EditorObjectType::DOOR:        return "Door";
        case EditorObjectType::WINDOW:      return "Window";
        case EditorObjectType::CSG:         return "CSG";
        default:                            return "Unknown";
        }
    }

    bool IsViewportOrthographic(uint32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_isOrthographic[viewportIndex];
        }
        else {
            std::cout << "Game::GetCameraByIndex(int index) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
            return false;
        }
    }

    //Camera* GetCameraByIndex(uint32_t index) {
    //    if (index >= 0 && index < 4) {
    //        return &g_cameras[index];
    //    }
    //    else {
    //        std::cout << "Game::GetCameraByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
    //        return nullptr;
    //    }
    //}

    Viewport* GetActiveViewport() {
        if (g_activeViewportIndex >= 0 && g_activeViewportIndex < 4) {
            return ViewportManager::GetViewportByIndex(g_activeViewportIndex);
        }
        else {
            std::cout << "Editor::GetActiveViewport(int index) failed. " << g_activeViewportIndex << " out of range of editor viewport count 4\n";
            return nullptr;
        }
    }

    ShadingMode GetViewportModeByIndex(uint32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_shadingModes[viewportIndex];
        }
        else {
            std::cout << "Editor::GetViewportModeByIndex(uint32_t viewportIndex) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
            return ShadingMode::SHADED;
        }
    }

    //void SetCameraView(uint32_t cameraViewIndex, CameraView cameraView) {
    //    if (cameraViewIndex >= 0 && cameraViewIndex < 4) {
    //        g_cameraViews[cameraViewIndex] = cameraView;
    //    }
    //    else {
    //        std::cout << "Editor::SetCameraViewByIndex(uint32_t cameraViewIndex, CameraView cameraView) failed. " << cameraViewIndex << " out of range of editor viewport count 4\n";
    //    }
    //}

    void SetEditorViewportSplitMode(EditorViewportSplitMode mode) {
        g_editorViewportSplitMode = mode;
    }

    void SetViewportOrthoSize(uint32_t viewportIndex, float size) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            g_OrthographicSizes[viewportIndex] = size;
        }
        else {
            std::cout << "Editor::SetViewportOrthoSize(uint32_t viewportIndex, float size) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
        }
    }    

    EditorState GetEditorState() {
        return g_editorState;
    }

    SelectionRectangleState& GetSelectionRectangleState() {
        return g_viewportSelectionRectangleState;
    }

    EditorMode& GetEditorMode() {
        return g_editorMode;
    }

    EditorLightingSettings& GetLightingSettings() {
        return g_editorLightingSettings;
    }

    float GetVerticalDividerXPos() {
        return g_verticalDividerXPos;
    }

    float GetHorizontalDividerYPos() {
        return g_horizontalDividerYPos;
    }

    EditorViewportSplitMode GetEditorViewportSplitMode() {
        return g_editorViewportSplitMode;
    }

    EditorMesh& GetEditorMesh() {
        return g_editorMesh;
    }

    void SetEditorState(EditorState editorState) {
        g_editorState = editorState;
    }

    void SetViewportOrthographicState(uint32_t index, bool state) {
        if (index >= 0 && index < 4) {
            g_isOrthographic[index] = state;
        }
        else {
            std::cout << "Editor::SetViewportOrthographicStateByIndex(uint32_t index, bool state) failed. " << index << " out of range of editor viewport count 4\n";
        }
    }


}