#include "VK_synchronization_manager.h"
#include "VK_device_manager.h"
#include <iostream>

namespace VulkanSynchronizationManager {

    //std::vector<VkSemaphore> g_imageAvailableSemaphores;
    std::vector<VkSemaphore> g_renderFinishedSemaphores;
    std::vector<VkSemaphore> g_uploadSemaphores;
    std::vector<VkFence> g_inFlightFences;
    const int MAX_FRAMES_IN_FLIGHT = 2;

    bool Init() {
        //g_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        g_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        g_uploadSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        g_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (
                //vkCreateSemaphore(VulkanDeviceManager::GetDevice(), &semaphoreInfo, nullptr, &g_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(VulkanDeviceManager::GetDevice(), &semaphoreInfo, nullptr, &g_renderFinishedSemaphores[i]) != VK_SUCCESS || 
                vkCreateSemaphore(VulkanDeviceManager::GetDevice(), &semaphoreInfo, nullptr, &g_uploadSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(VulkanDeviceManager::GetDevice(), &fenceInfo, nullptr, &g_inFlightFences[i]) != VK_SUCCESS) {
                std::cout << "Failed to create synchronization objects\n";
                return false;
            }
        }

        return true;
    }

    void Destroy() {
        VkDevice device = VulkanDeviceManager::GetDevice();
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            //vkDestroySemaphore(device, g_imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(device, g_renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, g_uploadSemaphores[i], nullptr);
            vkDestroyFence(device, g_inFlightFences[i], nullptr);
        }
    }

    //VkSemaphore GetImageAvailableSemaphore(size_t frameIndex) {
    //    return g_imageAvailableSemaphores[frameIndex];
    //}

    VkSemaphore GetRenderFinishedSemaphore(size_t frameIndex) {
        return g_renderFinishedSemaphores[frameIndex];
    }

    VkSemaphore GetUploadSemaphore(size_t frameIndex) {
        return g_uploadSemaphores[frameIndex];
    }

    VkFence GetInFlightFence(size_t frameIndex) {
        return g_inFlightFences[frameIndex];
    }

    void WaitAndResetFence(size_t frameIndex) {
        VkDevice device = VulkanDeviceManager::GetDevice();
        vkWaitForFences(device, 1, &g_inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &g_inFlightFences[frameIndex]);
    }
}
