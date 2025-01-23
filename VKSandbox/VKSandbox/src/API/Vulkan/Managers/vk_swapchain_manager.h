#pragma once
#include <vulkan/vulkan.h>
#include "../Types/vk_texture.h"

namespace VulkanSwapchainManager {
    bool Init();
    void Destroy();
    VulkanTexture& GetCurrentSwapchainTexture(uint32_t imageIndex);
    VkSwapchainKHR GetSwapchain();
    uint32_t GetSwapchainImageCount();
}
