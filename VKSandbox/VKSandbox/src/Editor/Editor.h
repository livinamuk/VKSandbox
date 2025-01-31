#pragma once
#include "HellTypes.h"
#include "Camera/Camera.h"
#include "Viewport/Viewport.h"
#include <string>

namespace Editor {
    void Init();
    void Update();
    void UpdateInput();
    void UpdateUI();
    void UpdateCamera();
    void UpdateMouseRays();
    void UpdateDebug();
    void Open();
    void Close();
    void ToggleOpenState();
    void SetActiveViewportIndex(int index);
    void SetSelectedObjectIndex(int index);
    void SetHoveredObjectIndex(int index);
    void SetSelectedObjectType(EditorObjectType editorObjectType);
    void SetHoveredObjectType(EditorObjectType editorObjectType);
    void SetSplitX(float value);
    void SetSplitY(float value);
    void SetViewportToFrontView(uint32_t viewportindex, glm::vec3 viewportOrigin);
    void SetViewportResizeState(ViewportResizeState viewportResizeState);

    int GetActiveViewportIndex();
    int GetHoveredViewportIndex();
    int GetSelectedObjectIndex();
    int GetHoveredObjectIndex();
    bool IsOpen();
    bool IsViewportOrthographic(uint32_t viewportIndex);
    float GetSplitX();
    float GetSplitY();
    glm::vec3 GetMouseRayOriginByViewportIndex(int32_t viewportIndex);
    glm::vec3 GetMouseRayDirectionByViewportIndex(int32_t viewportIndex);
    std::string EditorObjectTypeToString(const EditorObjectType& type);
    Camera* GetCameraByIndex(uint32_t index);
    Viewport* GetActiveViewport();
    ShadingMode GetViewportModeByIndex(uint32_t index);
    CameraView GetCameraViewByIndex(uint32_t index);
    EditorObjectType GetSelectedObjectType();
    EditorObjectType GetHoveredObjectType();
    ViewportResizeState GetViewportResizeState();
}