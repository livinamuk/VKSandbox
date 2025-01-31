#pragma once
#include "HellEnums.h"
#include <vector>

namespace GLFWIntegration {
    bool Init(API api, int windowWidth, int windowHeight, WindowedMode windowedMode);
    void Destroy();
    void BeginFrame(API api);
    void EndFrame(API api);
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
    void* GetWindowPointer();
    const WindowedMode& GetWindowedMode();
    void SetCursor(int);

    // OpenGL only
    void MakeContextCurrent();

    // Vulkan only
    bool CreateSurface(void* surface);
    const std::vector<const char*> GetRequiredInstanceExtensions();
}