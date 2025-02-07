#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanSynchronizationManager {
    bool Init();
    void Destroy();
    //VkSemaphore GetImageAvailableSemaphore(size_t frameIndex);
    VkSemaphore GetRenderFinishedSemaphore(size_t frameIndex); 
    VkSemaphore GetUploadSemaphore(size_t frameIndex);
    VkFence GetInFlightFence(size_t frameIndex);
    void WaitAndResetFence(size_t frameIndex);
}
