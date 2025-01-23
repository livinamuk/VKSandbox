#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanTexture {
public:
    int32_t m_width = 0;
    int32_t m_height = 0;
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    std::vector<VkImageLayout> m_mipLayouts; // Per-mip tracking

    void Cleanup(VkDevice device);
    VkImageLayout GetImageLayout(uint32_t mipLevel) const;
};
