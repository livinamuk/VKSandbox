#include "VK_renderer.h"
#include "../Managers/vk_command_manager.h"
#include "../Managers/vk_device_manager.h"
#include "../Managers/vk_frame_manager.h"
#include "../vk_util.h"
#include "../../../AssetManagement/AssetManager.h"
#include "../BackEnd/BackEnd.h"

namespace VulkanRenderer {


    void BlitTexture(VkCommandBuffer cmd, VulkanTexture& srcTexture, VulkanTexture& dstTexture, BlitRegion srcRegion, BlitRegion dstRigion) {
        VkDevice device = VulkanDeviceManager::GetDevice();
        VulkanUtil::TransitionImageLayout(device, cmd, srcTexture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        VulkanUtil::TransitionImageLayout(device, cmd, dstTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkImageBlit blitRegion = {};
        blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.srcSubresource.mipLevel = 0;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.srcOffsets[0] = { 0, 0, 0 };
        blitRegion.srcOffsets[1] = { srcRegion.originX, srcRegion.originY, 1 };

        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.mipLevel = 0;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstOffsets[0] = { 0, 0, 0 };
        blitRegion.dstOffsets[1] = { dstRigion.originX, dstRigion.originY, 1 };

        vkCmdBlitImage(cmd, srcTexture.m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstTexture.m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_LINEAR);
    }

    void RenderLoadingScreen() {
        std::cout << "\n\\\\\\\\\\\\\\\\\\\nLoading screen\n\\\\\\\\\\\\\\\\\\\n\n";

        Texture* texture = AssetManager::GetTextureByName("StandardFont");
        VulkanTexture vkTexture = texture->GetVKTexture();

        std::cout << "AT START OF FRAME: StandardFont.png layout: " << VulkanUtil::VkImageLayoutToString(vkTexture.m_mipLayouts[0]) << "\n\n";



        VkDevice device = VulkanDeviceManager::GetDevice();
        FrameContext& frame = VulkanFrameManager::GetCurrentFrame();
        VkResult fenceStatus = vkGetFenceStatus(device, frame.inFlightFence);
        std::cout << "Fence state: " << (fenceStatus == VK_SUCCESS ? "Signaled" : "Not Signaled") << "\n";

        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();

        VkCommandBuffer commandBuffer = VulkanCommandManager::BeginFrame();


        VulkanTexture& swapChainTexture = VulkanCommandManager::GetCurrentSwapChainTexture();

        // Transition textures
        VulkanUtil::TransitionImageLayout(
            VulkanDeviceManager::GetDevice(),
            commandBuffer,
            vkTexture,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            "StandardFont.png"
        );

        VulkanUtil::TransitionImageLayout(
            VulkanDeviceManager::GetDevice(),
            commandBuffer,
            swapChainTexture,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            "Swapchain"
        );

        VkImageBlit blitRegion{};
        blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.srcSubresource.mipLevel = 0;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.srcOffsets[0] = { 0, 0, 0 };
        blitRegion.srcOffsets[1] = { vkTexture.m_width, vkTexture.m_height, 1 };

        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.mipLevel = 0;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstOffsets[0] = { 0, 0, 0 };
        blitRegion.dstOffsets[1] = { windowWidth, windowHeight, 1 };

        std::cout << "StandardFont.png layout: " << VulkanUtil::VkImageLayoutToString(vkTexture.m_mipLayouts[0]) << "\n";
        std::cout << "\n vkCmdBlitImage StandardFont.png into the swap chain!\n\n";

        vkCmdBlitImage(
            commandBuffer,
            vkTexture.m_image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            swapChainTexture.m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &blitRegion,
            VK_FILTER_LINEAR
        );

        // Transition textures back to expected layouts
       //VulkanUtil::TransitionImageLayout(
       //    VulkanDeviceManager::GetDevice(),
       //    commandBuffer,
       //    vkTexture,
       //    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
       //    "StandardFont.png"
       //);

        VulkanUtil::TransitionImageLayout(
            VulkanDeviceManager::GetDevice(),
            commandBuffer,
            swapChainTexture,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            "Swapchain"
        );

        VulkanCommandManager::EndFrame(commandBuffer);


        std::cout << "AT END OF FRAME: StandardFont.png layout: " << VulkanUtil::VkImageLayoutToString(vkTexture.m_mipLayouts[0]) << "\n";
    }




    void RenderGame() {

    }
}