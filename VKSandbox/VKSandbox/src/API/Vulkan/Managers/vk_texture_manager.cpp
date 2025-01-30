#include "vk_texture_manager.h"
#include "vk_device_manager.h"
#include "vk_command_manager.h"
#include "../vk_util.h"

#include "../../../API/OpenGL/GL_Util.h"

namespace VulkanTextureManager {

    void AllocateTexture(Texture& texture) {
        VkDevice device = VulkanDeviceManager::GetDevice();
        VkPhysicalDevice physicalDevice = VulkanDeviceManager::GetPhysicalDevice();

        VulkanTexture& vkTexture = texture.GetVKTexture();
        if (vkTexture.m_image != VK_NULL_HANDLE) {
            return; // Texture already allocated
        }
        // Determine the number of mip levels
        int mipmapWidth = texture.GetWidth(0);
        int mipmapHeight = texture.GetHeight(0);
        uint32_t mipLevels = texture.MipmapsAreRequested() ? texture.GetMipmapLevelCount() : 1;

        // Create the VkImage
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = mipmapWidth;
        imageCreateInfo.extent.height = mipmapHeight;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = mipLevels;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = VulkanUtil::GLInternalFormatToVkFormat(texture.GetInternalFormat());
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, &vkTexture.m_image);
        if (result != VK_SUCCESS) {
            std::cout << "vkCreateImage failed with error code: " << result << " for " << texture.GetFileName() << "\n";
            return;
        }
        // Allocate memory for the VkImage
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, vkTexture.m_image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanUtil::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkAllocateMemory(device, &allocInfo, nullptr, &vkTexture.m_memory);
        vkBindImageMemory(device, vkTexture.m_image, vkTexture.m_memory, 0);

        // Create an image view for the texture
        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = vkTexture.m_image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = imageCreateInfo.format;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = mipLevels;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(device, &viewCreateInfo, nullptr, &vkTexture.m_imageView);

        // Initialize mip levels
        vkTexture.m_mipLayouts.resize(mipLevels, VK_IMAGE_LAYOUT_UNDEFINED);

        // Store shit 
        // THIS COULD BE BETTER!!!!!!!!!!!!!!!!!!!!!!!!!!!
        vkTexture.m_width = texture.GetWidth(0);
        vkTexture.m_height = texture.GetHeight(0);
    }

    void VulkanTextureManager::GenerateMipmaps(Texture& texture) {
        VulkanCommandManager::ImmediateSubmit2([&](VkCommandBuffer commandBuffer) {
            VkDevice device = VulkanDeviceManager::GetDevice();
            VulkanTexture& vkTexture = texture.GetVKTexture();
            int width = texture.GetWidth(0);
            int height = texture.GetHeight(0);

            int mipLevels = texture.GetMipmapLevelCount();
            for (int i = 1; i < mipLevels; i++) {
                VkImageBlit blit{};
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { width, height, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;

                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { std::max(1, width / 2), std::max(1, height / 2), 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                VulkanUtil::TransitionImageLayoutSingleMip(device, commandBuffer, vkTexture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i - 1);
                VulkanUtil::TransitionImageLayoutSingleMip(device, commandBuffer, vkTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i);

                vkCmdBlitImage(
                    commandBuffer,
                    vkTexture.m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    vkTexture.m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit, VK_FILTER_LINEAR
                );

                VulkanUtil::TransitionImageLayoutSingleMip(device, commandBuffer, vkTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i - 1);
                VulkanUtil::TransitionImageLayoutSingleMip(device, commandBuffer, vkTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i);

                width = std::max(1, width / 2);
                height = std::max(1, height / 2);
            }

            VulkanUtil::TransitionImageLayout(device, commandBuffer, vkTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            std::cout << "Generated mips for " << texture.GetFileName() << "\n";
        });
    }

}
