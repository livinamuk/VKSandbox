#include "vk_texture.h"
#include <iostream>

void VulkanTexture::Cleanup(VkDevice device) {
    if (m_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device, m_imageView, nullptr);
        m_imageView = VK_NULL_HANDLE;
    }
    if (m_image != VK_NULL_HANDLE) {
        vkDestroyImage(device, m_image, nullptr);
        m_image = VK_NULL_HANDLE;
    }
    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }
}

VkImageLayout VulkanTexture::GetImageLayout(uint32_t mipLevel) const {
    if (mipLevel >= m_mipLayouts.size()) {
        std::cout << "Mip level out of range";
    }
    return m_mipLayouts[mipLevel];
}