#pragma once
#include "HellTypes.h"
#include <string>

namespace Editor {

    void Update();
    void Open();
    void Close();
    void ToggleOpenState();
    bool IsOpen();

    EditorObjectType GetSelectedObjectType();
    std::string EditorObjectTypeToString(const EditorObjectType& type);
}