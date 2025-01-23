#pragma once
#include "vk_util.h"
#include <glad/glad.h>
#include <iostream>

namespace VulkanUtil {

    VkFormat GLFormatToVkFormat(unsigned int glFormat) {
        switch (glFormat) {
        case GL_RGB8:          return VK_FORMAT_R8G8B8_UNORM;
        case GL_RGBA8:         return VK_FORMAT_R8G8B8A8_UNORM;
        case GL_RGB16F:        return VK_FORMAT_R16G16B16_SFLOAT;
        case GL_RGBA16F:       return VK_FORMAT_R16G16B16A16_SFLOAT;
        case GL_R8:            return VK_FORMAT_R8_UNORM;
        case GL_RG8:           return VK_FORMAT_R8G8_UNORM;
        case GL_R16F:          return VK_FORMAT_R16_SFLOAT;
        case GL_RG16F:         return VK_FORMAT_R16G16_SFLOAT;
        case GL_DEPTH_COMPONENT16: return VK_FORMAT_D16_UNORM;
        case GL_DEPTH_COMPONENT24: return VK_FORMAT_D32_SFLOAT; // Vulkan has no direct equivalent to GL_DEPTH_COMPONENT24
        case GL_DEPTH24_STENCIL8: return VK_FORMAT_D24_UNORM_S8_UINT;
        case GL_DEPTH32F_STENCIL8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    VkFormat GLInternalFormatToVkFormat(GLenum glInternalFormat) {
        switch (glInternalFormat) {
            // S3TC/DXT Formats
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: return VK_FORMAT_BC2_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: return VK_FORMAT_BC3_UNORM_BLOCK;

            // BPTC/BC7 Formats
        case GL_COMPRESSED_RGBA_BPTC_UNORM_ARB: return VK_FORMAT_BC7_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB: return VK_FORMAT_BC7_SRGB_BLOCK;
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB: return VK_FORMAT_BC6H_UFLOAT_BLOCK;

            // RGTC Formats (BC4 and BC5)
        case GL_COMPRESSED_RED_RGTC1: return VK_FORMAT_BC4_UNORM_BLOCK;
        case GL_COMPRESSED_SIGNED_RED_RGTC1: return VK_FORMAT_BC4_SNORM_BLOCK;
        case GL_COMPRESSED_RG_RGTC2: return VK_FORMAT_BC5_UNORM_BLOCK;
        case GL_COMPRESSED_SIGNED_RG_RGTC2: return VK_FORMAT_BC5_SNORM_BLOCK;

            // ETC2 Formats
        case GL_COMPRESSED_RGB8_ETC2: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ETC2: return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2: return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2: return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
        case GL_COMPRESSED_RGBA8_ETC2_EAC: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC: return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;

            // EAC Formats
        case GL_COMPRESSED_R11_EAC: return VK_FORMAT_EAC_R11_UNORM_BLOCK;
        case GL_COMPRESSED_SIGNED_R11_EAC: return VK_FORMAT_EAC_R11_SNORM_BLOCK;
        case GL_COMPRESSED_RG11_EAC: return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
        case GL_COMPRESSED_SIGNED_RG11_EAC: return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;

            // ASTC Formats
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR: return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR: return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR: return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR: return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR: return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR: return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR: return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR: return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR: return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR: return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR: return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;

            // Uncompressed Formats
        case GL_RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
        case GL_RGB8: return VK_FORMAT_R8G8B8_UNORM;
        case GL_RGBA16F: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case GL_RGB16F: return VK_FORMAT_R16G16B16_SFLOAT;

        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    std::string VkImageLayoutToString(VkImageLayout layout) {
        switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED: return "UNDEFINED";
        case VK_IMAGE_LAYOUT_GENERAL: return "GENERAL";
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return "COLOR_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return "DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL: return "DEPTH_STENCIL_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return "SHADER_READ_ONLY_OPTIMAL";
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return "TRANSFER_SRC_OPTIMAL";
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return "TRANSFER_DST_OPTIMAL";
        case VK_IMAGE_LAYOUT_PREINITIALIZED: return "PREINITIALIZED";
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return "PRESENT_SRC_KHR";
        default: return "UNKNOWN_LAYOUT";
        }
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        const auto& memProperties = VulkanDeviceManager::GetMemoryProperties();
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        return UINT32_MAX;
    }

    void VulkanUtil::TransitionImageLayout(VkDevice device, VkCommandBuffer commandBuffer, VulkanTexture& vkTexture, VkImageLayout newLayout, const std::string& name) {
        uint32_t mipLevels = static_cast<uint32_t>(vkTexture.m_mipLayouts.size());

        for (uint32_t mipLevel = 0; mipLevel < mipLevels; ++mipLevel) {
            VkImageLayout oldLayout = vkTexture.m_mipLayouts[mipLevel];
            if (oldLayout == newLayout) {

                std::cout << name << " " << "Skipping redundant transition for mip " << mipLevel << "\n\n";
                continue;
            }

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = vkTexture.m_image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = mipLevel;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

            // Determine source access and stage
            switch (oldLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                barrier.srcAccessMask = 0;
                srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            default:
                barrier.srcAccessMask = 0;
                srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                break;
            }

            // Determine destination access and stage
            switch (newLayout) {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; // or whichever stage you will need
                break;
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                barrier.dstAccessMask = 0;
                dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                break;
            default:
                barrier.dstAccessMask = 0;
                dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                break;
            }

            vkCmdPipelineBarrier(
                commandBuffer,
                srcStageMask,
                dstStageMask,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            vkTexture.m_mipLayouts[mipLevel] = newLayout;

            std::cout << name << " " << VulkanUtil::VkImageLayoutToString(oldLayout) << " -> " << VulkanUtil::VkImageLayoutToString(newLayout) << "\n\n";
        }
    }


    void VulkanUtil::TransitionImageLayoutSingleMip(VkDevice device, VkCommandBuffer commandBuffer, VulkanTexture& vkTexture, VkImageLayout newLayout, uint32_t mipLevel) {
        if (vkTexture.m_mipLayouts[mipLevel] == newLayout) {
            return;
        }
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = vkTexture.m_mipLayouts[mipLevel];
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = vkTexture.m_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = mipLevel;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        vkTexture.m_mipLayouts[mipLevel] = newLayout;
    }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkDevice device = VulkanDeviceManager::GetDevice();
        VkPhysicalDevice physicalDevice = VulkanDeviceManager::GetPhysicalDevice();
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            std::cout << "Error: Failed to create Vulkan buffer\n";
            buffer = VK_NULL_HANDLE;
            bufferMemory = VK_NULL_HANDLE;
            return;
        }
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanUtil::FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            std::cout << "Error: Failed to allocate memory for Vulkan buffer\n";
            vkDestroyBuffer(device, buffer, nullptr);
            buffer = VK_NULL_HANDLE;
            bufferMemory = VK_NULL_HANDLE;
            return;
        }
        if (vkBindBufferMemory(device, buffer, bufferMemory, 0) != VK_SUCCESS) {
            std::cout << "Error: Failed to bind Vulkan buffer memory\n";
            vkDestroyBuffer(device, buffer, nullptr);
            vkFreeMemory(device, bufferMemory, nullptr);
            buffer = VK_NULL_HANDLE;
            bufferMemory = VK_NULL_HANDLE;
        }
    }
}