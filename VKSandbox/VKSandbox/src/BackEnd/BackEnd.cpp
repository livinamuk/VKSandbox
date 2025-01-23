#include "BackEnd.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "../API/OpenGL/GL_backEnd.h"
#include "../API/OpenGL/GL_renderer.h"
#include "../API/Vulkan/VK_backEnd.h"
#include "../AssetManagement/AssetManager.h"
#include "../Core/Audio.h"
#include "../Input/Input.h"
#include "../Renderer/Renderer.h"
#include "../UI/UIBackEnd.h"

#include "GLFWIntegration.h"

// Prevent accidentally selecting integrated GPU
extern "C" {
    __declspec(dllexport) unsigned __int32 AmdPowerXpressRequestHighPerformance = 0x1;
    __declspec(dllexport) unsigned __int32 NvOptimusEnablement = 0x1;
}

namespace BackEnd {
    API g_api = API::UNDEFINED;
    int g_presentTargetWidth = 0;
    int g_presentTargetHeight = 0;

    bool Init(API api, int windowWidth, int windowHeight, WindowedMode windowMode) {
        g_api = api;
        if (GLFWIntegration::Init(api, windowWidth, windowHeight, windowMode)) {
            return false;
        }        
        if (GetAPI() == API::OPENGL) {
            OpenGLBackEnd::Init();
            OpenGLRenderer::Init();
        }
        else if (GetAPI() == API::VULKAN) {
            if (!VulkanBackEnd::Init()) {
                return false;
            }
        }
        AssetManager::Init();
        UIBackEnd::Init();
        Audio::Init();
        Input::Init(BackEnd::GetWindowPointer());

        //Input::Init();
        //Audio::Init();
        //Physics::Init();
        //InputMulti::Init();
        //CSG::Init();
        //Pathfinding2::Init();
        //WeaponManager::Init();
        glfwShowWindow(static_cast<GLFWwindow*>(BackEnd::GetWindowPointer()));
        return true;
    }

    void BeginFrame() {
        GLFWIntegration::BeginFrame(g_api);
    }

    void EndFrame() {
        GLFWIntegration::EndFrame(g_api);
    }

    void UpdateSubSystems() {
        Input::Update();
        Audio::Update();
        if (Input::KeyPressed(HELL_KEY_ESCAPE)) {
            BackEnd::ForceCloseWindow();
        }
        if (Input::KeyPressed(HELL_KEY_G)) {
            BackEnd::ToggleFullscreen();
        }
        if (Input::KeyPressed(HELL_KEY_H)) {
            Renderer::HotloadShaders();
        }
    }

    void CleanUp() {
        GLFWIntegration::Destroy();
    }

    void SetAPI(API api) {
        g_api = api;
    }

    void SetPresentTargetSize(int width, int height) {
        g_presentTargetWidth = width;
        g_presentTargetHeight = height;
    }

    const API GetAPI() {
        return g_api;
    }

    // Window
    void* GetWindowPointer() {
        return GLFWIntegration::GetWindowPointer();;
    }

    const WindowedMode& GetWindowedMode() {
        return GLFWIntegration::GetWindowedMode();
    }

    void BackEnd::SetWindowedMode(const WindowedMode& windowedMode) {
        GLFWIntegration::SetWindowedMode(windowedMode);
    }

    void BackEnd::ToggleFullscreen() {
        GLFWIntegration::ToggleFullscreen();
    }

    void BackEnd::ForceCloseWindow() {
        GLFWIntegration::ForceCloseWindow();
    }

    bool BackEnd::WindowIsOpen() {
        return GLFWIntegration::WindowIsOpen();
    }

    bool BackEnd::WindowHasFocus() {
        return GLFWIntegration::WindowHasFocus();
    }

    bool BackEnd::WindowHasNotBeenForceClosed() {
        return GLFWIntegration::WindowHasNotBeenForceClosed();
    }

    bool BackEnd::WindowIsMinimized() {
        return GLFWIntegration::WindowIsMinimized();
    }

    int BackEnd::GetWindowedWidth() {
        return GLFWIntegration::GetWindowedWidth();
    }

    int BackEnd::GetWindowedHeight() {
        return GLFWIntegration::GetWindowedHeight();
    }

    int BackEnd::GetCurrentWindowWidth() {
        return GLFWIntegration::GetCurrentWindowWidth();
    }

    int BackEnd::GetCurrentWindowHeight() {
        return GLFWIntegration::GetCurrentWindowHeight();
    }

    int BackEnd::GetFullScreenWidth() {
        return GLFWIntegration::GetFullScreenWidth();
    }

    int BackEnd::GetFullScreenHeight() {
        return GLFWIntegration::GetFullScreenHeight();
    }

    int GetPresentTargetWidth() {
        return g_presentTargetWidth;
    }

    int GetPresentTargetHeight() {
        return g_presentTargetHeight;
    }
}

