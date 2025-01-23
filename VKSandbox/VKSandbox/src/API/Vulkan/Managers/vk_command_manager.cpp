#include "VK_command_manager.h"
#include "VK_device_manager.h"
#include "vk_frame_manager.h"
#include "vk_swapchain_manager.h"
#include "vk_synchronization_manager.h"
#include <iostream>
#include "../Types/vk_texture.h"
#include "../vk_backend.h"
#include "../vk_util.h"

namespace VulkanCommandManager {

    void VulkanCommandManager::ImmediateSubmit3(std::function<void(VkCommandBuffer)>&& recordFunc) {
        VkDevice device = VulkanDeviceManager::GetDevice();
        VkQueue graphicsQueue = VulkanDeviceManager::GetGraphicsQueue();

        // Create a dedicated transient command pool for this operation
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.queueFamilyIndex = VulkanDeviceManager::GetGraphicsQueueFamilyIndex();

        VkCommandPool immediateCommandPool;
        VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &immediateCommandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool for ImmediateSubmit");
        }

        // Allocate a one-time command buffer
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = immediateCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer cmd;
        result = vkAllocateCommandBuffers(device, &allocInfo, &cmd);
        if (result != VK_SUCCESS) {
            vkDestroyCommandPool(device, immediateCommandPool, nullptr);
            throw std::runtime_error("Failed to allocate command buffer for ImmediateSubmit");
        }

        // Begin recording commands
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        result = vkBeginCommandBuffer(cmd, &beginInfo);
        if (result != VK_SUCCESS) {
            vkFreeCommandBuffers(device, immediateCommandPool, 1, &cmd);
            vkDestroyCommandPool(device, immediateCommandPool, nullptr);
            throw std::runtime_error("Failed to begin command buffer for ImmediateSubmit");
        }

        // Let the caller record its commands
        recordFunc(cmd);

        // End the command buffer
        result = vkEndCommandBuffer(cmd);
        if (result != VK_SUCCESS) {
            vkFreeCommandBuffers(device, immediateCommandPool, 1, &cmd);
            vkDestroyCommandPool(device, immediateCommandPool, nullptr);
            throw std::runtime_error("Failed to end command buffer for ImmediateSubmit");
        }

        // Create a fence to wait on
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VkFence submitFence;
        result = vkCreateFence(device, &fenceInfo, nullptr, &submitFence);
        if (result != VK_SUCCESS) {
            vkFreeCommandBuffers(device, immediateCommandPool, 1, &cmd);
            vkDestroyCommandPool(device, immediateCommandPool, nullptr);
            throw std::runtime_error("Failed to create fence for ImmediateSubmit");
        }

        // Submit the command buffer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        std::cout << "vkQueueSubmit in immediatesubmit3\n";
        result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, submitFence);
        if (result != VK_SUCCESS) {
            vkDestroyFence(device, submitFence, nullptr);
            vkFreeCommandBuffers(device, immediateCommandPool, 1, &cmd);
            vkDestroyCommandPool(device, immediateCommandPool, nullptr);
            throw std::runtime_error("Failed to submit command buffer for ImmediateSubmit");
        }
        std::cout << "after vkQueueSubmit in immediatesubmit3\n";

        // Wait for the fence to ensure GPU completion
        result = vkWaitForFences(device, 1, &submitFence, VK_TRUE, UINT64_MAX);
        if (result != VK_SUCCESS) {
            vkDestroyFence(device, submitFence, nullptr);
            vkFreeCommandBuffers(device, immediateCommandPool, 1, &cmd);
            vkDestroyCommandPool(device, immediateCommandPool, nullptr);
            throw std::runtime_error("Failed to wait on fence for ImmediateSubmit");
        }

        // Clean up
        vkDestroyFence(device, submitFence, nullptr);
        vkFreeCommandBuffers(device, immediateCommandPool, 1, &cmd);
        vkDestroyCommandPool(device, immediateCommandPool, nullptr);
    }


    
    VkCommandBuffer BeginFrame() {
        VkDevice device = VulkanDeviceManager::GetDevice();
        VkSwapchainKHR swapchain = VulkanSwapchainManager::GetSwapchain();
        FrameContext& frame = VulkanFrameManager::GetCurrentFrame();
        size_t frameIndex = VulkanFrameManager::GetCurrentFrameIndex();


        std::cout << "before WaitAndResetFence()\n";

        // Wait for the previous frame's rendering to finish
        VulkanSynchronizationManager::WaitAndResetFence(frameIndex);
        
        std::cout << "after WaitAndResetFence()\n";

        // Wait on the upload semaphore for this frame
        VkSemaphore uploadSemaphore = VulkanSynchronizationManager::GetUploadSemaphore(frameIndex);
        if (uploadSemaphore != VK_NULL_HANDLE) {
            VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

            // Submit a dummy operation to wait on the upload semaphore
            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &uploadSemaphore;
            submitInfo.pWaitDstStageMask = &waitStage;
            submitInfo.commandBufferCount = 0; // No commands; just waiting

            std::cout << "vkQueueSubmit in begin frame\n";
            VkQueue graphicsQueue = VulkanDeviceManager::GetGraphicsQueue();
            if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
                throw std::runtime_error("Failed to wait on upload semaphore.");
            }
            std::cout << "after vkQueueSubmit in begin frame\n";
        }

        // Acquire the next image from the swapchain
        VkResult result = vkAcquireNextImageKHR(
            device, swapchain,
            UINT64_MAX, // Timeout
            frame.imageAvailableSemaphore,
            VK_NULL_HANDLE,
            &frame.swapChainImageIndex
        );

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            throw std::runtime_error("Swapchain is out of date.");
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image.");
        }

        // Reset the command pool for this frame
        vkResetCommandPool(device, frame.commandPool, 0);

        // Allocate and begin recording a command buffer
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = frame.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer.");
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer.");
        }

        return commandBuffer;
    }




    void EndFrame(VkCommandBuffer commandBuffer) {
        VkDevice device = VulkanDeviceManager::GetDevice();
        VkSwapchainKHR swapchain = VulkanSwapchainManager::GetSwapchain();
        VkQueue graphicsQueue = VulkanDeviceManager::GetGraphicsQueue();
        VkQueue presentQueue = VulkanDeviceManager::GetPresentQueue();
        FrameContext& frame = VulkanFrameManager::GetCurrentFrame();

        //std::cout << "Ending command buffer.\n";
        VkResult endResult = vkEndCommandBuffer(commandBuffer);
        if (endResult != VK_SUCCESS) {
            std::cerr << "Failed to end command buffer: " << endResult << "\n";
            throw std::runtime_error("Failed to end command buffer.");
        }

        std::cout << "Fence state before queue submission: " << (vkGetFenceStatus(device, frame.inFlightFence) == VK_SUCCESS ? "Signaled" : "Not Signaled") << "\n";

        // Submit the command buffer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        std::cout << "Submitting command buffer.\n";
        VkResult submitResult = vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame.inFlightFence);
        if (submitResult != VK_SUCCESS) {
            std::cerr << "Failed to submit command buffer: " << submitResult << "\n";
            throw std::runtime_error("Failed to submit command buffer.");
        }

        std::cout << "Fence state after queue submission: " << (vkGetFenceStatus(device, frame.inFlightFence) == VK_SUCCESS ? "Signaled" : "Not Signaled") << "\n";

        // Present the image
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        VkSemaphore waitSemaphoresPresent[] = { frame.renderFinishedSemaphore };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphoresPresent;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &frame.swapChainImageIndex;
        presentInfo.pResults = nullptr;

        std::cout << "Presenting image (swapchain index: " << frame.swapChainImageIndex << ").\n";
        VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
            std::cerr << "Swapchain is out of date or suboptimal.\n";
            throw std::runtime_error("Swapchain out of date or suboptimal.");
        }
        else if (presentResult != VK_SUCCESS) {
            std::cerr << "Failed to present swapchain image: " << presentResult << "\n";
            throw std::runtime_error("Failed to present swapchain image.");
        }

        // Debug fence wait
        std::cout << "Waiting for fence to signal GPU work completion.\n";
        VkResult waitResult = vkWaitForFences(device, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);
        if (waitResult != VK_SUCCESS) {
            std::cerr << "Failed to wait for fence: " << waitResult << "\n";
            throw std::runtime_error("Failed to wait for fence.");
        }

        std::cout << "Fence successfully waited on. GPU work complete for this frame.\n";

        // Increment the frame index
        VulkanFrameManager::IncrementFrameIndex();
        //std::cout << "Frame index incremented.\n";
    }


    VulkanTexture& GetCurrentSwapChainTexture() {
        FrameContext& frame = VulkanFrameManager::GetCurrentFrame();
        uint32_t imageIndex = frame.swapChainImageIndex;


        //std::cout << "frameIndex : " << VulkanFrameManager::GetCurrentFrameIndex() << "\n";
        //std::cout << "imageIndex: " << imageIndex << "\n";
        //std::cout << "swapchainImageCount: " << VulkanSwapchainManager::GetSwapchainImageCount() << "\n\n";

        if (imageIndex >= VulkanSwapchainManager::GetSwapchainImageCount()) {
            throw std::out_of_range("Swap chain image index out of range.");
        }

        return VulkanSwapchainManager::GetCurrentSwapchainTexture(imageIndex);
    }

    void ImmediateSubmit2(std::function<void(VkCommandBuffer cmd)>&& function) {
        size_t frameIndex = VulkanFrameManager::GetCurrentFrameIndex();
        VkDevice device = VulkanDeviceManager::GetDevice();
        VkQueue graphicsQueue = VulkanDeviceManager::GetGraphicsQueue();
        VkSemaphore uploadSemaphore = VulkanSynchronizationManager::GetUploadSemaphore(frameIndex);
        UploadContext& uploadContext = VulkanBackEnd::GetUploadContext();
        VkCommandBuffer cmd = uploadContext.commandBuffer;

        // Begin recording the command buffer
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (vkBeginCommandBuffer(cmd, &commandBufferBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer for ImmediateSubmit2.");
        }

        // Execute the user-provided function
        function(cmd);

        if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
            throw std::runtime_error("Failed to end recording command buffer for ImmediateSubmit2.");
        }

        // Submit the command buffer and signal the upload semaphore
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &uploadSemaphore;

        std::cout << "vkQueueSubmit in immediatesubmit2\n";
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, uploadContext.uploadFence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit upload command buffer.");
        }
        std::cout << "after vkQueueSubmit in immediatesubmit2\n";

        // Wait for the fence to ensure the upload is complete
        if (vkWaitForFences(device, 1, &uploadContext.uploadFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
            throw std::runtime_error("Failed to wait for upload fence.");
        }

        // Reset the fence to allow reuse in the next frame
        if (vkResetFences(device, 1, &uploadContext.uploadFence) != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset upload fence.");
        }

        // Reset the command pool to allow the command buffer to be reused
        if (vkResetCommandPool(device, uploadContext.commandPool, 0) != VK_SUCCESS) {
            throw std::runtime_error("Failed to reset upload command pool.");
        }
    }
}
