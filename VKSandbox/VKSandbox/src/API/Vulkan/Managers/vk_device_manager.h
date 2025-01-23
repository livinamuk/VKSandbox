#pragma once
#include <vulkan/vulkan.h>

namespace VulkanDeviceManager {
    bool Init();
    void Destroy();
    VkDevice GetDevice();
    VkInstance GetInstance();
    VkPhysicalDevice GetPhysicalDevice();
    VkQueue GetGraphicsQueue();
    VkQueue GetPresentQueue();
    uint32_t GetGraphicsQueueFamilyIndex();
    VkSurfaceKHR GetSurface();
    const VkPhysicalDeviceMemoryProperties& GetMemoryProperties();
}
