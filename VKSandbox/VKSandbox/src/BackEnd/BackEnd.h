#pragma once
#include "HellEnums.h"

namespace BackEnd {
    // Core
    bool Init(API api, int windowWidth, int windowHeight, WindowedMode windowMode);
    void BeginFrame();
    void UpdateSubSystems();
    void EndFrame();
    void CleanUp();

    // API
    void SetAPI(API api);
    const API GetAPI();

    // Window
    void* GetWindowPointer();
    void SetWindowedMode(const WindowedMode& windowedMode);
    void ToggleFullscreen();
    void ForceCloseWindow();
    bool WindowIsOpen();
    bool WindowHasFocus();
    bool WindowHasNotBeenForceClosed();
    bool WindowIsMinimized();
    int GetWindowedWidth();
    int GetWindowedHeight();
    int GetCurrentWindowWidth();
    int GetCurrentWindowHeight();
    int GetFullScreenWidth();
    int GetFullScreenHeight();
    const WindowedMode& GetWindowedMode();

    // Render Targets
    void SetPresentTargetSize(int width, int height);
    int GetPresentTargetWidth();
    int GetPresentTargetHeight();
}