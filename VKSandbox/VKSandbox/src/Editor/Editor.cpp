#include "Editor.h"
#include "HellDefines.h"
#include "Gizmo.h"
#include "BackEnd/BackEnd.h"
#include "Camera/Camera.h"
#include "Core/Audio.h"
#include "Core/Debug.h"
#include "Core/Scene.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"

namespace Editor {

    EditorMesh g_editorMesh;
    int g_activeViewportIndex = 3;
    bool g_isOpen = false;
    bool g_isOrthographic[4];
    float g_OrthographicSizes[4];
    float g_verticalDividerXPos = 0.0f;
    float g_splitY = 0.0f;
    uint16_t g_selectedObjectIndex = 0;
    uint16_t g_hoveredObjectIndex = 0;
    EditorObjectType g_selectedObjectType = EditorObjectType::NONE;
    EditorObjectType g_hoveredObjectType = EditorObjectType::NONE;
    ShadingMode g_shadingModes[4];
    //CameraView g_cameraViews[4];
    
    float g_orthoCameraDistances[4];
    ViewportResizeState g_viewportResizeState;

    void Init() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            g_editorMesh.Init(glm::vec3(0.5f, 1.45f, 2.53));
            g_editorMesh.RecalculateMesh();
        }
        ResetViewports();
    }

    void ResetViewports() {
        float ORTHO_CAMERA_DISTANCE_FROM_ORIGIN = 250.0f;
        g_orthoCameraDistances[0] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[1] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[2] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[3] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;

        // Top left
        SetViewportView(0, Gizmo::GetPosition(), CameraView::TOP);
        //g_cameras[0].SetPosition(glm::vec3(0, g_orthoCameraDistances[0], 0.0f));
        //g_cameras[0].SetEulerRotation(glm::vec3(HELL_PI * -0.5f, HELL_PI * -0.5f, 0.0f));

        //g_shadingModes[0] = ShadingMode::WIREFRAME;
        //g_cameraViews[0] = CameraView::TOP;
        //g_isOrthographic[0] = true;

        // Top right
        SetViewportView(1, Gizmo::GetPosition(), CameraView::FRONT);

        // Bottom left
        SetViewportView(2, Gizmo::GetPosition(), CameraView::LEFT);
        //g_cameras[2].SetPosition(glm::vec3(-g_orthoCameraDistances[2], 0.8f, 0.0f));
        //g_cameras[2].SetEulerRotation(glm::vec3(0.0f, HELL_PI * -0.5f, 0.0f));
      //g_shadingModes[2] = ShadingMode::WIREFRAME;
      //g_cameraViews[2] = CameraView::LEFT;
      //g_isOrthographic[2] = true;

        // Bottom right
        SetViewportView(3, Gizmo::GetPosition(), CameraView::RIGHT);
        //g_cameras[3].SetPosition(glm::vec3(-2.09f, 1.45f, 0.68f));
        //g_cameras[3].SetEulerRotation(glm::vec3(-0.30f, -1.32f, 0.0f));
        //g_shadingModes[3] = ShadingMode::SHADED;
        //g_cameraViews[3] = CameraView::PERSPECTIVE;
        //g_isOrthographic[3] = false;
    }

    void Update(float deltaTime) {
        if (!IsOpen()) return;

        UpdateMouseRays();
        UpdateCamera();
        UpdateDividers();
        UpdateInput();
        UpdateUI();
        // Translate the selected object
        if (g_selectedObjectType == EditorObjectType::GAME_OBJECT) {
            Scene::GetGameObjects()[g_selectedObjectIndex].m_transform.position = Gizmo::GetPosition();
        }
        UpdateCursor();
        UpdateDebug();
        UpdateCameraInterpolation(deltaTime);
        Gizmo::Update();
    }

    void Open() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::ShowCursor();
        Input::SetCursorPosition(BackEnd::GetCurrentWindowWidth() / 2, BackEnd::GetCurrentWindowHeight() / 2);
        g_isOpen = true;
    }

    void Close() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::DisableCursor();
        g_isOpen = false;
    }

    void ToggleOpenState() {
        if (g_isOpen) {
            Close();
        }
        else {
            Open();
        }
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
        ViewportManager::UpdateViewports();
    }

    void SetSplitY(float value) {
        g_splitY = value;
        ViewportManager::UpdateViewports();
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

    void SetViewportOrthoSize(uint32_t viewportIndex, float size) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            g_OrthographicSizes[viewportIndex] = size;
        }
        else {
            std::cout << "Editor::SetViewportOrthoSize(uint32_t viewportIndex, float size) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
        }
    }

 

    ViewportResizeState GetViewportResizeState() {
        return g_viewportResizeState;
    }

    float GetVerticalDividerXPos() {
        return g_verticalDividerXPos;
    }

    float GetHorizontalDividerYPos() {
        return g_splitY;
    }

    EditorMesh& GetEditorMesh() {
        return g_editorMesh;
    }

    void SetViewportResizeState(ViewportResizeState viewportResizeState) {
        g_viewportResizeState = viewportResizeState;
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