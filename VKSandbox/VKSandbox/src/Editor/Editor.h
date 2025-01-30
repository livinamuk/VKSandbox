#pragma once
#include "HellTypes.h"
#include "../Types/Camera.h"
#include <string>

namespace Editor {

    void Init();
    void Update();
    void UpdateDebug();
    void Open();
    void Close();
    void ToggleOpenState();
    bool IsOpen();

    float GetSplitX();
    float GetSplitY();
    bool IsViewportOrthographic(uint32_t viewportIndex);
    Camera* GetCameraByIndex(uint32_t index);
    EditorObjectType GetSelectedObjectType();
    std::string EditorObjectTypeToString(const EditorObjectType& type);
}