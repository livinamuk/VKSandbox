#pragma once
#include "HellTypes.h"
#include "Camera/Camera.h"
#include "Viewport/Viewport.h"
#include "Types/EditorMesh.h"
#include <string>

struct EditorLightingSettings {
    bool lightingEnabled = true;
    bool grassEnabled = true;
};

namespace Editor {
    
    void Init();
    void UpdateImGui();
    void ResetCameras();
    void ResetViewports();
    void Update(float deltaTime);
    void UpdateCursor();
    void UpdateDividers();
    void UpdateInput();
    void UpdateUI();
    void UpdateCamera();
    void UpdateMouseRays();
    void UpdateCameraInterpolation(float deltaTime);
    void UpdateDebug();
    void OpenEditor(EditorMode editorMode);
    void Close();
    void ShowEditorSelectMenu();
    void CloseEditorSelectMenu();
    void SetActiveViewportIndex(int index);
    void SetSelectedObjectIndex(int index);
    void SetHoveredObjectIndex(int index);
    void SetSelectedObjectType(EditorObjectType editorObjectType);
    void SetHoveredObjectType(EditorObjectType editorObjectType);
    void SetSplitX(float value);
    void SetSplitY(float value);
    void SetViewportView(uint32_t viewportIndex, glm::vec3 viewportOrigin, CameraView targetView);
    void SetEditorState(EditorState editorState);
    void SetViewportOrthographicState(uint32_t viewportIndex, bool state);
    //void SetCameraView(uint32_t cameraViewIndex, CameraView cameraView);
    void SetViewportOrthoSize(uint32_t viewportIndex, float size);
    void SetEditorViewportSplitMode(EditorViewportSplitMode mode);

    int GetActiveViewportIndex();
    int GetHoveredViewportIndex();
    int GetSelectedObjectIndex();
    int GetHoveredObjectIndex();
    bool IsOpen();
    bool IsEditorSelectMenuVisible();
    bool IsViewportOrthographic(uint32_t viewportIndex);
    bool EditorIsIdle();
    bool EditorWasIdleLastFrame();
    float GetVerticalDividerXPos();
    float GetHorizontalDividerYPos();
    glm::vec3 GetMouseRayOriginByViewportIndex(int32_t viewportIndex);
    glm::vec3 GetMouseRayDirectionByViewportIndex(int32_t viewportIndex);
    glm::mat4 GetViewportViewMatrix(int32_t viewportIndex);
    std::string EditorObjectTypeToString(const EditorObjectType& type);
   // Camera* GetCameraByIndex(uint32_t index);
    Viewport* GetActiveViewport();
    ShadingMode GetViewportModeByIndex(uint32_t index);
    CameraView GetCameraViewByIndex(uint32_t index);
    EditorObjectType GetSelectedObjectType();
    EditorObjectType GetHoveredObjectType();
    EditorState GetEditorState();
    EditorViewportSplitMode GetEditorViewportSplitMode();
    SelectionRectangleState& GetSelectionRectangleState();
    EditorMode& GetEditorMode();
    EditorLightingSettings& GetLightingSettings();

    EditorMesh& GetEditorMesh(); // BROKEN / 5% IMPLEMENTED

    // Dividers
    bool IsVerticalDividerHovered();
    bool IsHorizontalDividerHovered();
}