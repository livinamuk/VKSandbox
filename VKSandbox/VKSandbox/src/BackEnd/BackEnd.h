#pragma once
#include "HellEnums.h"
#include "stdint.h"

namespace BackEnd {
    // Core
    bool Init(API api, int windowWidth, int windowHeight, WindowedMode windowMode);
    void BeginFrame();
    void UpdateGame();
    void UpdateSubSystems();
    void EndFrame();
    void CleanUp();
    bool IsRenderDocActive();

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

    // MousePick
    void UpdateMousePicking(int x, int);
    uint16_t GetMousePickR();
    uint16_t GetMousePickG();
}