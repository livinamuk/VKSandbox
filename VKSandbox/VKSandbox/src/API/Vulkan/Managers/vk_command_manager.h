#pragma once
#include <vulkan/vulkan.h>
#include <functional>
#include "../Types/vk_texture.h"

namespace VulkanCommandManager {
    VkCommandBuffer BeginFrame();
    void EndFrame(VkCommandBuffer commandBuffer);
    void ImmediateSubmit3(std::function<void(VkCommandBuffer cmd)>&& function);
    void ImmediateSubmit2(std::function<void(VkCommandBuffer cmd)>&& function);

    VulkanTexture& GetCurrentSwapChainTexture(); // do u really need this here? it's also in a function in the swapchain
}
