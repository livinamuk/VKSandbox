#pragma once
#include <vulkan/vulkan.h>
#include "Managers/vk_device_manager.h"
#include "Types/vk_texture.h"
#include <string>
#include <iostream>

inline void VK_CHECK(VkResult err) {
    if (err) {
        std::cout << "Detected Vulkan error: " << err << "\n";
        abort();
    }
}

namespace VulkanUtil {
    VkFormat GLFormatToVkFormat(unsigned int glFormat);
    VkFormat GLInternalFormatToVkFormat(unsigned int glInternalFormat);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void TransitionImageLayout(VkDevice device, VkCommandBuffer commandBuffer, VulkanTexture& vkTexture, VkImageLayout newLayout, const std::string& name = "");
    void TransitionImageLayoutSingleMip(VkDevice device, VkCommandBuffer commandBuffer, VulkanTexture& vkTexture, VkImageLayout newLayout, uint32_t mipLevel);
    std::string VkImageLayoutToString(VkImageLayout layout);
}