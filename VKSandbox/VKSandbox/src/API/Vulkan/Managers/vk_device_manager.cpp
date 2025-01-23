#include "VK_device_manager.h"
#include <iostream>
#include <vector>
#include "../BackEnd/BackEnd.h"
#include "../BackEnd/GLFWIntegration.h"

namespace VulkanDeviceManager {
    VkInstance g_instance = VK_NULL_HANDLE;
    VkDevice g_device = VK_NULL_HANDLE;
    VkPhysicalDevice g_physicalDevice = VK_NULL_HANDLE;
    VkQueue g_graphicsQueue = VK_NULL_HANDLE;
    VkQueue g_presentQueue = VK_NULL_HANDLE;
    uint32_t g_graphicsQueueFamilyIndex = 0;
    VkSurfaceKHR g_surface = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties g_memoryProperties;
    bool g_enableValidationLayers = true; 

    const std::vector<const char*> g_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    std::vector<const char*> g_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool CreateInstance();
    bool PickPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CreateLogicalDevice();
    bool PickPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CreateSurface();
    bool CheckValidationLayerSupport();
    void CacheMemoryProperties();

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    bool Init() {
        if (!CreateInstance()) {
            std::cout << "VulkanManager::Init() failed to create Vulkan instance\n"; 
            return false;
        }
        if (!PickPhysicalDevice()) {
            std::cout << "VulkanManager::Init() failed to pick physical instance\n";
            return false;
        }
        if (!CreateSurface()) {
            std::cout << "VulkanDeviceManager::Init() failed to create Vulkan surface\n";
            return false;
        }
        if (!CreateLogicalDevice()) {
            std::cout << "VulkanManager::Init() Failed to create logical device instance\n";
            return false;
        }
        CacheMemoryProperties();
        return true;
    }

    void Destroy() {
        if (g_device) {
            vkDestroyDevice(g_device, nullptr);
        }
        if (g_surface) {
            vkDestroySurfaceKHR(g_instance, g_surface, nullptr);
        }
        if (g_instance) {
            vkDestroyInstance(g_instance, nullptr);
        }
    }

    bool CreateSurface() {
        return GLFWIntegration::CreateSurface(&g_surface);
    }

    bool CreateInstance() {
        // Application Info
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Unloved";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "UnlovedEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        // Check for validation layer support if enabled
        if (g_enableValidationLayers && !CheckValidationLayerSupport()) {
            std::cout << "Validation layers requested, but not available!\n";
            return false;
        }

        // Get required extensions from GLFW
        std::vector<const char*> extensions = GLFWIntegration::GetRequiredInstanceExtensions();
        if (g_enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Add debug utils extension
        }

        // Instance Creation Info
        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceInfo.ppEnabledExtensionNames = extensions.data();

        // Add validation layers if enabled
        if (g_enableValidationLayers) {
            instanceInfo.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
            instanceInfo.ppEnabledLayerNames = g_validationLayers.data();
        }
        else {
            instanceInfo.enabledLayerCount = 0;
            instanceInfo.ppEnabledLayerNames = nullptr;
        }

        // Debug Messenger (optional)
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (g_enableValidationLayers) {
            debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            debugCreateInfo.pfnUserCallback = DebugCallback;
            instanceInfo.pNext = &debugCreateInfo; // Chain debug messenger
        }
        else {
            instanceInfo.pNext = nullptr;
        }

        // Create the Vulkan instance
        if (vkCreateInstance(&instanceInfo, nullptr, &g_instance) != VK_SUCCESS) {
            std::cout << "Failed to create Vulkan instance\n";
            return false;
        }

        return true;
    }

    bool PickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(g_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            std::cout << "No Vulkan-supported devices found\n";
            return false;
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(g_instance, &deviceCount, devices.data());

        for (const auto& dev : devices) {
            if (IsDeviceSuitable(dev)) {
                g_physicalDevice = dev;
                return true;
            }
        }
        std::cout << "No suitable Vulkan device found\n";
        return false;
    }

    bool IsDeviceSuitable(VkPhysicalDevice device) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                g_graphicsQueueFamilyIndex = i;
                return true;
            }
        }
        return false;
    }

    bool CreateLogicalDevice() {
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = g_graphicsQueueFamilyIndex;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = 1;
        deviceInfo.pQueueCreateInfos = &queueInfo;
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(g_deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = g_deviceExtensions.data();

        if (vkCreateDevice(g_physicalDevice, &deviceInfo, nullptr, &g_device) != VK_SUCCESS) {
            std::cerr << "Failed to create Vulkan logical device\n";
            return false;
        }

        vkGetDeviceQueue(g_device, g_graphicsQueueFamilyIndex, 0, &g_graphicsQueue);

        // Questionalbe?
        g_presentQueue = g_graphicsQueue;

        return true;
    }

    bool CheckValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (const char* layerName : g_validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                return false;
            }
        }
        return true;
    }

    void CacheMemoryProperties() {
        vkGetPhysicalDeviceMemoryProperties(GetPhysicalDevice(), &g_memoryProperties);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        // Suppress specific messages
        if (strstr(pCallbackData->pMessage, "Override layer has override paths") != nullptr) {
            return VK_FALSE; // Suppress this specific message
        }

        std::cout << "Validation Layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }


    VkDevice GetDevice() {
        return g_device;
    }

    VkInstance GetInstance() {
        return g_instance;
    }

    VkSurfaceKHR GetSurface() {
        return g_surface;
    }

    VkPhysicalDevice GetPhysicalDevice() {
        return g_physicalDevice;
    }

    VkQueue GetGraphicsQueue() {
        return g_graphicsQueue;
    }

    VkQueue GetPresentQueue() {
        return g_presentQueue;
    }

    uint32_t GetGraphicsQueueFamilyIndex() {
        return g_graphicsQueueFamilyIndex;
    }
    
    const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() {
        return g_memoryProperties;
    }
}