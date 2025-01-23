#pragma once
#include "HellEnums.h"
#include "../Types/Texture.h"

struct UploadContext {
    VkFence uploadFence;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
};

namespace VulkanBackEnd{
    bool Init();
    void Destroy();

    UploadContext& GetUploadContext();

    // Textures
    void UpdateTextureBaking();
    void AllocateTextureMemory(Texture& texture);
    void ImmediateBake(QueuedTextureBake& queuedTextureBake);
    void AsyncBakeQueuedTextureBake(QueuedTextureBake& queuedTextureBake);
}