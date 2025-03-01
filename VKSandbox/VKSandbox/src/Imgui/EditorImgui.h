#pragma once
#include "Types/Types.h"

namespace EditorImGui {
    void Init();
    void Update();
    void CreateImguiElements();
    void CloseAnyOpenContent();

    bool ImGuiContentIsVisible();
    bool ImGuiContentWasVisibleLastFrame();
    bool AnyElementHasHover();
    bool FloatingWindowsAreVisible();
    bool HasKeyboardFocus();;

    void ShowNewFileWindow(const std::string& name);
    void ShowOpenFileWindow(const std::string& name);

    NewFileWindow& CreateNewFileWindow(const std::string& name, const std::string& title, NewFileCallback callback);
    NewFileWindow* GetNewFileWindow(const std::string& name);

    OpenFileWindow& CreateOpenFileWindow(const std::string& name, const std::string& title, const std::string filepath, OpenFileCallback callback);
    OpenFileWindow* GetOpenFileWindow(const std::string& name);

    FileMenu& CreateFileMenu(const std::string& name);
    FileMenu* GetFileMenuByName(const std::string& name);

    EditorWindow* CreateEditorWindow(const std::string& windowName);
    EditorWindow* GetEditorWindowByName(const std::string& windowName);
}