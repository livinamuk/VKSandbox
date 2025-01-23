#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct FrameContext {
    VkCommandPool commandPool;
    VkFence inFlightFence;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    uint32_t swapChainImageIndex;
};

namespace VulkanFrameManager {
    
    constexpr int MAX_FRAMES_IN_FLIGHT = 2; // Double buffering

    bool Init();
    void Destroy();
    FrameContext& GetCurrentFrame();
    void IncrementFrameIndex();
    uint32_t GetCurrentFrameIndex();
}
