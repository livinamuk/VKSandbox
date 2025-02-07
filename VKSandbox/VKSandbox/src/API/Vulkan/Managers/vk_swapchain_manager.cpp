#include "VK_swapchain_manager.h"
#include "VK_device_manager.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "../BackEnd/GLFWIntegration.h"

namespace VulkanSwapchainManager {
    VkSwapchainKHR g_swapchain = VK_NULL_HANDLE;
    VkExtent2D g_swapchainExtent = {};
    VkFormat g_swapchainImageFormat = VK_FORMAT_UNDEFINED;
    std::vector<VulkanTexture> g_swapchainTextures; // Updated to store VulkanTexture objects

    bool Init() {
        VkDevice device = VulkanDeviceManager::GetDevice();
        VkPhysicalDevice physicalDevice = VulkanDeviceManager::GetPhysicalDevice();
        VkSurfaceKHR surface = VulkanDeviceManager::GetSurface();

        // Query swapchain support
        uint32_t formatCount = 0, presentModeCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

        if (formatCount == 0 || presentModeCount == 0) {
            std::cerr << "VulkanSwapChainManager::Init() - No swapchain support found.\n";
            return false;
        }

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

        // Choose the best format
        VkSurfaceFormatKHR surfaceFormat = formats[0];
        for (const auto& availableFormat : formats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat = availableFormat;
                break;
            }
        }

        g_swapchainImageFormat = surfaceFormat.format;

        // Choose the best present mode
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; // V-Sync by default
        for (const auto& availablePresentMode : presentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                presentMode = availablePresentMode;
                break;
            }
        }

        // Choose the swap extent
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

        if (capabilities.currentExtent.width == UINT32_MAX) {
            g_swapchainExtent.width = GLFWIntegration::GetCurrentWindowWidth();
            g_swapchainExtent.height = GLFWIntegration::GetCurrentWindowHeight(); // Corrected height
            // Clamp the extent to the min/max extents from capabilities
            g_swapchainExtent.width = std::clamp(g_swapchainExtent.width,
                capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            g_swapchainExtent.height = std::clamp(g_swapchainExtent.height,
                capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        }
        else {
            g_swapchainExtent = capabilities.currentExtent; // Use Vulkan's suggested extent
        }

        // Validate image count
        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
            imageCount = capabilities.maxImageCount;
        }

        // Create the swapchain
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = g_swapchainImageFormat;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = g_swapchainExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; // Added transfer destination usage
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        // Since present and graphics queues are the same, use exclusive sharing mode
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &g_swapchain) != VK_SUCCESS) {
            std::cerr << "Failed to create swapchain.\n";
            return false;
        }

        // Retrieve swapchain images
        vkGetSwapchainImagesKHR(device, g_swapchain, &imageCount, nullptr);
        std::vector<VkImage> swapchainImages(imageCount);
        vkGetSwapchainImagesKHR(device, g_swapchain, &imageCount, swapchainImages.data());

        // Initialize g_swapchainTextures
        g_swapchainTextures.resize(imageCount);
        for (size_t i = 0; i < swapchainImages.size(); i++) {
            // Create VulkanTexture for each swapchain image
            g_swapchainTextures[i].m_image = swapchainImages[i];
            g_swapchainTextures[i].m_imageView = VK_NULL_HANDLE; // Will create next
            g_swapchainTextures[i].m_memory = VK_NULL_HANDLE; // Swapchain manages memory
            g_swapchainTextures[i].m_mipLayouts = { VK_IMAGE_LAYOUT_UNDEFINED }; // Initial layout
            g_swapchainTextures[i].m_width = g_swapchainExtent.width;   // Set width
            g_swapchainTextures[i].m_height = g_swapchainExtent.height; // Set height

                // Create image view for the swapchain image
                VkImageViewCreateInfo viewInfo{};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = swapchainImages[i];
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = g_swapchainImageFormat;
                viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.layerCount = 1;

                if (vkCreateImageView(device, &viewInfo, nullptr, &g_swapchainTextures[i].m_imageView) != VK_SUCCESS) {
                    std::cerr << "Failed to create image view for swapchain image " << i << ".\n";
                    return false;
                }
            }

            return true;
        }

    void Destroy() {
        VkDevice device = VulkanDeviceManager::GetDevice();
        for (auto& texture : g_swapchainTextures) {
            if (texture.m_imageView != VK_NULL_HANDLE) {
                vkDestroyImageView(device, texture.m_imageView, nullptr);
                texture.m_imageView = VK_NULL_HANDLE;
            }
            // No need to destroy texture.image or texture.memory as they are managed by swapchain
        }
        g_swapchainTextures.clear();

        if (g_swapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(device, g_swapchain, nullptr);
            g_swapchain = VK_NULL_HANDLE;
        }
    }

    // Implement the new function
    VulkanTexture& GetCurrentSwapchainTexture(uint32_t imageIndex) {
        if (imageIndex >= g_swapchainTextures.size()) {
            throw std::out_of_range("Swap chain image index out of range.");
        }
        return g_swapchainTextures[imageIndex];
    }

    VkSwapchainKHR GetSwapchain() {
        return g_swapchain;
    }

    uint32_t GetSwapchainImageCount() {
        return g_swapchainTextures.size();
    }
}
