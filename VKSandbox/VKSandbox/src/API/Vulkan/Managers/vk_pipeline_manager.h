#pragma once
#include <vulkan/vulkan.h>

namespace VulkanPipelineManager {
    VkPipeline CreateGraphicsPipeline(const VkPipelineRenderingCreateInfo& renderingInfo, VkPipelineLayout pipelineLayout, VkShaderModule vertexShader, VkShaderModule fragmentShader);
    void DestroyPipeline(VkPipeline pipeline);
    VkPipelineLayout CreatePipelineLayout(const VkPipelineLayoutCreateInfo& layoutInfo);
    void DestroyPipelineLayout(VkPipelineLayout pipelineLayout);
}
