#include "vk_frame_manager.h"
#include "vk_device_manager.h"
#include <stdexcept>
#include <iostream>

namespace VulkanFrameManager {

    std::vector<FrameContext> g_frameContexts;
    uint32_t g_currentFrameIndex = 0;

    bool Init() {
        VkDevice device = VulkanDeviceManager::GetDevice();
        g_frameContexts.resize(MAX_FRAMES_IN_FLIGHT);

        // Create synchronization objects and command pools for each frame
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            // Create semaphores
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g_frameContexts[i].imageAvailableSemaphore) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g_frameContexts[i].renderFinishedSemaphore) != VK_SUCCESS) {
                std::cout << "Failed to create semaphores for frame " << i << "\n";
                return false;
            }

            // Create fences
            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Initialize as signaled

            if (vkCreateFence(device, &fenceInfo, nullptr, &g_frameContexts[i].inFlightFence) != VK_SUCCESS) {
                std::cout << "Failed to create fence for frame " << i << "\n";
                return false;
            }

            // Create command pool
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.queueFamilyIndex = VulkanDeviceManager::GetGraphicsQueueFamilyIndex();
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow resetting command buffers individually

            if (vkCreateCommandPool(device, &poolInfo, nullptr, &g_frameContexts[i].commandPool) != VK_SUCCESS) {
                std::cerr << "Failed to create command pool for frame " << i << "\n";
                return false;
            }

            // Initialize swapChainImageIndex to an invalid value
            g_frameContexts[i].swapChainImageIndex = UINT32_MAX;
        }

        return true;
    }

    void Destroy() {
        VkDevice device = VulkanDeviceManager::GetDevice();
        for (auto& frame : g_frameContexts) {
            vkDestroyCommandPool(device, frame.commandPool, nullptr);
            vkDestroySemaphore(device, frame.imageAvailableSemaphore, nullptr);
            vkDestroySemaphore(device, frame.renderFinishedSemaphore, nullptr);
            vkDestroyFence(device, frame.inFlightFence, nullptr);
        }
        g_frameContexts.clear();
    }


    FrameContext& GetCurrentFrame() {
        return g_frameContexts[g_currentFrameIndex];
    }

    void IncrementFrameIndex() {
        g_currentFrameIndex = (g_currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    uint32_t GetCurrentFrameIndex() {
        return g_currentFrameIndex;
    }
}
