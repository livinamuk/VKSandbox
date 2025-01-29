#include "GLFWIntegration.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Vulkan/vulkan.h>
#include <iostream>
#include <string>
#include "../API/Vulkan/Managers/VK_device_manager.h"

namespace GLFWIntegration {
    GLFWwindow* g_window = NULL;
    WindowedMode g_windowedMode = WindowedMode::WINDOWED;
    GLFWmonitor* g_monitor;
    const GLFWvidmode* g_mode;
    bool g_forceCloseWindow = false;
    bool g_windowHasFocus = true;
    int g_windowedWidth = 0;
    int g_windowedHeight = 0;
    int g_fullscreenWidth = 0;
    int g_fullscreenHeight = 0;
    int g_currentWindowWidth = 0;
    int g_currentWindowHeight = 0;

    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void window_focus_callback(GLFWwindow* window, int focused);

    bool Init(API api, int windowWidth, int windowHeight, WindowedMode windowedMode) {
        glfwInit();
        glfwSetErrorCallback([](int error, const char* description) { std::cout << "GLFW Error (" << std::to_string(error) << "): " << description << "\n"; });

        if (api == API::OPENGL) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        }
        else if (api == API::VULKAN) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

        // Resolution and window size
        g_monitor = glfwGetPrimaryMonitor();
        g_mode = glfwGetVideoMode(g_monitor);
        glfwWindowHint(GLFW_RED_BITS, g_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, g_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, g_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, g_mode->refreshRate);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        g_fullscreenWidth = g_mode->width;
        g_fullscreenHeight = g_mode->height;
        g_windowedWidth = windowWidth;
        g_windowedHeight = windowHeight;

        // Create window
        g_windowedMode = windowedMode;
        if (g_windowedMode == WindowedMode::WINDOWED) {
            g_currentWindowWidth = g_windowedWidth;
            g_currentWindowHeight = g_windowedHeight;
            g_window = glfwCreateWindow(g_windowedWidth, g_windowedHeight, "Unloved", NULL, NULL);
            glfwSetWindowPos(g_window, 0, 0);
        }
        else if (windowedMode == WindowedMode::FULLSCREEN) {
            g_currentWindowWidth = g_fullscreenWidth;
            g_currentWindowHeight = g_fullscreenHeight;
            g_window = glfwCreateWindow(g_fullscreenWidth, g_fullscreenHeight, "Unloved", g_monitor, NULL);
        }
        g_windowedMode = windowedMode;
        if (g_window == NULL) {
            std::cout << "GLFWIntegration() Failed to create window\n";
            glfwTerminate();
            return false;
        }
        glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
        glfwSetWindowFocusCallback(g_window, window_focus_callback);
        return true;
    }

    void MakeContextCurrent() {
        glfwMakeContextCurrent(g_window);
    }

    bool CreateSurface(void* surface) {
        VkInstance instance = VulkanDeviceManager::GetInstance();
        VkPhysicalDevice GetPhysicalDevice();
        if (glfwCreateWindowSurface(instance, g_window, nullptr, static_cast<VkSurfaceKHR*>(surface)) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void BeginFrame(API api) {
        glfwPollEvents();
    }

    void EndFrame(API api) {
        if (api == API::OPENGL) {
            glfwSwapBuffers(g_window);
        }
        else if (api == API::VULKAN) {
            // Nothing required
        }
    }

    void Destroy() {
        if (GetWindowedMode() == WindowedMode::FULLSCREEN) {
            ToggleFullscreen();
        }
        glfwTerminate();
    }

    void SetWindowedMode(const WindowedMode& windowedMode) {
        if (windowedMode == WindowedMode::WINDOWED) {
            g_currentWindowWidth = g_windowedWidth;
            g_currentWindowHeight = g_windowedHeight;
            glfwSetWindowMonitor(g_window, nullptr, 0, 0, g_windowedWidth, g_windowedHeight, g_mode->refreshRate);
            glfwSetWindowPos(g_window, 0, 0);
        }
        else if (windowedMode == WindowedMode::FULLSCREEN) {
            g_currentWindowWidth = g_fullscreenWidth;
            g_currentWindowHeight = g_fullscreenHeight;
            glfwSetWindowMonitor(g_window, nullptr, 0, 0, g_fullscreenWidth - 1, g_fullscreenHeight - 1, g_mode->refreshRate);
            //glfwSetWindowMonitor(g_window, g_monitor, 0, 0, g_fullscreenWidth, g_fullscreenHeight, g_mode->refreshRate);
        }
        g_windowedMode = windowedMode;
    }

    void ToggleFullscreen() {
        if (g_windowedMode == WindowedMode::WINDOWED) {
            SetWindowedMode(WindowedMode::FULLSCREEN);
        }
        else {
            SetWindowedMode(WindowedMode::WINDOWED);
        }
    }

    void ForceCloseWindow() {
        g_forceCloseWindow = true;
    }

    bool WindowHasFocus() {
        return g_windowHasFocus;
    }

    bool WindowHasNotBeenForceClosed() {
        return !g_forceCloseWindow;
    }

    void* GetWindowPointer() {
        return g_window;
    }

    int GetWindowedWidth() {
        return g_windowedWidth;
    }

    int GetWindowedHeight() {
        return g_windowedHeight;
    }

    int GetFullScreenWidth() {
        return g_fullscreenWidth;
    }

    int GetFullScreenHeight() {
        return g_fullscreenHeight;
    }

    int GetCurrentWindowWidth() {
        return g_currentWindowWidth;
    }

    int GetCurrentWindowHeight() {
        return g_currentWindowHeight;
    }

    bool WindowIsOpen() {
        return !(glfwWindowShouldClose(g_window) || g_forceCloseWindow);
    }

    bool WindowIsMinimized() {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(g_window, &width, &height);
        return (width == 0 || height == 0);
    }

    const WindowedMode& GetWindowedMode() {
        return g_windowedMode;
    }

    const std::vector<const char*> GetRequiredInstanceExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        if (!glfwExtensions) {
            std::cerr << "GLFWIntegration::GetRequiredInstanceExtensions() failed to get required instance extensions\n";
            return {};
        }
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        // Debug: Print the required extensions
        //std::cout << "Required extensions:\n";
        //for (const auto& ext : extensions) {
        //    std::cout << " - " << ext << "\n";
        //}
        return extensions;
    }

    void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height) {
        // Nothing as of yet
    }

    void window_focus_callback(GLFWwindow* /*window*/, int focused) {
        if (focused) {
            g_windowHasFocus = true;
        }
        else {
            g_windowHasFocus = false;
        }
    }
}
