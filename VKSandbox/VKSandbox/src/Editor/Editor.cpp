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

    #define ORTHO_CAMERA_DISTANCE_FROM_ORIGIN 250.0f // Rethink this

    int g_activeViewportIndex = 3;
    bool g_isOpen = false;
    bool g_isOrthographic[4];
    float g_splitX = 0.4f;
    float g_splitY = 0.4f;
    Camera g_cameras[4];
    uint16_t g_selectedObjectIndex = 0;
    uint16_t g_hoveredObjectIndex = 0;
    EditorObjectType g_selectedObjectType = EditorObjectType::NONE;
    EditorObjectType g_hoveredObjectType = EditorObjectType::NONE;
    ShadingMode g_shadingModes[4];
    CameraView g_cameraViews[4];
    ViewportResizeState g_viewportResizeState;

    void SetViewportToFrontView(uint32_t viewportindex, glm::vec3 viewportOrigin) {
        Camera* camera = GetCameraByIndex(viewportindex);
        g_cameras[viewportindex].SetPosition(viewportOrigin + glm::vec3(0.0f, 0.0F, ORTHO_CAMERA_DISTANCE_FROM_ORIGIN));
        g_cameras[viewportindex].SetEulerRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        g_cameraViews[viewportindex] = CameraView::FRONT;
        g_isOrthographic[viewportindex] = true;
        ViewportManager::UpdateViewports();
    }

    void SetViewportResizeState(ViewportResizeState viewportResizeState) {
        g_viewportResizeState = viewportResizeState;
    }

    void Init() {        

        // Top left
        g_cameras[0].SetPosition(glm::vec3(0, ORTHO_CAMERA_DISTANCE_FROM_ORIGIN, 0.0f));
        g_cameras[0].SetEulerRotation(glm::vec3(HELL_PI * -0.5f, HELL_PI * -0.5f, 0.0f));
        g_shadingModes[0] = ShadingMode::WIREFRAME;
        g_cameraViews[0] = CameraView::TOP;
        g_isOrthographic[0] = true;

        // Top right
        SetViewportToFrontView(1, glm::vec3(0.0f, 0.8f, 0.0f));

        // Bottom left
        g_cameras[2].SetPosition(glm::vec3(-ORTHO_CAMERA_DISTANCE_FROM_ORIGIN, 0.8f, 0.0f));
        g_cameras[2].SetEulerRotation(glm::vec3(0.0f, HELL_PI * -0.5f, 0.0f));
        g_shadingModes[2] = ShadingMode::WIREFRAME;
        g_cameraViews[2] = CameraView::LEFT;
        g_isOrthographic[2] = true;

        // Bottom right
        g_cameras[3].SetPosition(glm::vec3(-2.09f, 1.45f, 0.68f));
        g_cameras[3].SetEulerRotation(glm::vec3(-0.30f, -1.32f, 0.0f));
        g_shadingModes[3] = ShadingMode::SHADED;
        g_cameraViews[3] = CameraView::PERSPECTIVE;
        g_isOrthographic[3] = false;

        for (int i = 0; i < 4; i++) {
            //g_isOrthographic[i] = false;
        }
    }

    void Update() {
        if (!IsOpen()) return;

        Gizmo::Update();
        UpdateCamera();
        UpdateMouseRays();
        UpdateInput();
        UpdateUI();
        // Translate the selected object
        if (g_selectedObjectType == EditorObjectType::GAME_OBJECT) {
            Scene::g_gameObjects[g_selectedObjectIndex].m_transform.position = Gizmo::GetPosition();
        }

        UpdateDebug();
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
        g_splitX = value;
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

    Camera* GetCameraByIndex(uint32_t index) {
        if (index >= 0 && index < 4) {
            return &g_cameras[index];
        }
        else {
            std::cout << "Game::GetCameraByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
            return nullptr;
        }
    }

    Viewport* GetActiveViewport() {
        if (g_activeViewportIndex >= 0 && g_activeViewportIndex < 4) {
            return ViewportManager::GetViewportByIndex(g_activeViewportIndex);
        }
        else {
            std::cout << "Editor::GetActiveViewport(int index) failed. " << g_activeViewportIndex << " out of range of editor viewport count 4\n";
            return nullptr;
        }
    }

    ShadingMode GetViewportModeByIndex(uint32_t index) {
        if (index >= 0 && index < 4) {
            return g_shadingModes[index];
        }
        else {
            std::cout << "Editor::GetViewportModeByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
            return ShadingMode::SHADED;
        }
    }

    CameraView GetCameraViewByIndex(uint32_t index) {
        if (index >= 0 && index < 4) {
            return g_cameraViews[index];
        }
        else {
            std::cout << "Editor::GetViewportTypeByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
            return CameraView::PERSPECTIVE;
        }
    }

    ViewportResizeState GetViewportResizeState() {
        return g_viewportResizeState;
    }

    float GetSplitX() {
        return g_splitX;
    }

    float GetSplitY() {
        return g_splitY;
    }
}