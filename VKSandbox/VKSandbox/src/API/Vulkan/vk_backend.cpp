#include "VK_backend.h"
#include "Managers/vK_device_manager.h"
#include "Managers/vK_command_manager.h"
#include "Managers/vK_frame_manager.h"
#include "Managers/vK_swapchain_manager.h"
#include "Managers/vK_synchronization_manager.h"
#include "Managers/vk_texture_manager.h"
#include "vk_util.h"
#include "../AssetManagement/BakeQueue.h"
#include <iostream>

namespace VulkanBackEnd {

    UploadContext g_uploadContext;

    bool Init() {
        if (!VulkanDeviceManager::Init() ||
            !VulkanSwapchainManager::Init() ||
            !VulkanSynchronizationManager::Init() ||
            !VulkanFrameManager::Init()) {
            return false;
        }
        else {
            std::cout << "Made it to the end!\n";

            // put this shit somewhere better
            //VkFenceCreateInfo uploadFenceCreateInfo = {};
            //uploadFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            //uploadFenceCreateInfo.flags = 0;
            //uploadFenceCreateInfo.pNext = nullptr;
            //
            //VkDevice device = VulkanDeviceManager::GetDevice();
            //vkCreateFence(device, &uploadFenceCreateInfo, nullptr, &g_uploadContext.uploadFence);
            //
            //VkCommandPoolCreateInfo uploadCommandPoolInfo = {};
            //uploadCommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            //uploadCommandPoolInfo.flags = 0;
            //uploadCommandPoolInfo.pNext = nullptr;
            //
            //vkCreateCommandPool(device, &uploadCommandPoolInfo, nullptr, &g_uploadContext.commandPool);
            //VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            //commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            //commandBufferAllocateInfo.commandPool = g_uploadContext.commandPool;
            //commandBufferAllocateInfo.commandBufferCount = 1;
            //commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            //commandBufferAllocateInfo.pNext = nullptr;
            //vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &g_uploadContext.commandBuffer);

            return true;
        }
    }

    void Destroy() {
        VulkanDeviceManager::Destroy();
        VulkanSwapchainManager::Destroy();
    }

    void UpdateTextureBaking() {

    }

    void AllocateTextureMemory(Texture& texture) {
        VulkanTextureManager::AllocateTexture(texture);
    }

    void VulkanBackEnd::ImmediateBake(QueuedTextureBake& queuedTextureBake) {
        Texture* texture = static_cast<Texture*>(queuedTextureBake.texture);
        if (texture->GetTextureDataLevelBakeState(queuedTextureBake.mipmapLevel) == BakeState::BAKE_COMPLETE) {
            std::cout << "ERROR!!! Tried to bake already baked texture!\n";
            return;
        }
        VulkanTexture& vkTexture = texture->GetVKTexture();
        VkDevice device = VulkanDeviceManager::GetDevice();

        std::cout << "immediate baking: " << texture->GetFileName();

        int width = queuedTextureBake.width;
        int height = queuedTextureBake.height;
        int format = queuedTextureBake.format;
        int internalFormat = queuedTextureBake.internalFormat;
        int level = queuedTextureBake.mipmapLevel;
        int dataSize = queuedTextureBake.dataSize;
        const void* data = queuedTextureBake.data;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanUtil::CreateBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* stagingData;
        vkMapMemory(device, stagingBufferMemory, 0, dataSize, 0, &stagingData);
        memcpy(stagingData, data, dataSize);
        vkUnmapMemory(device, stagingBufferMemory);

        VulkanCommandManager::ImmediateSubmit2([&](VkCommandBuffer commandBuffer) {
            VulkanUtil::TransitionImageLayout(device, commandBuffer, vkTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;  // Tightly packed
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = level;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };

            vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, vkTexture.m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        });

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        if (texture->MipmapsAreRequested() && texture->GetTextureDataCount() == 1) {
            VulkanTextureManager::GenerateMipmaps(*texture);
        }

        texture->SetTextureDataLevelBakeState(level, BakeState::BAKE_COMPLETE);
        BakeQueue::RemoveQueuedTextureBakeByJobID(queuedTextureBake.jobID);
    }


    void AsyncBakeQueuedTextureBake(QueuedTextureBake& queuedTextureBake) {

    }

    UploadContext& GetUploadContext() {
        return g_uploadContext;
    }
}