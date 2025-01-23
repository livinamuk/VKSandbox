#pragma once
#include <vulkan/vulkan.h>
#include <vector>

struct VulkanBuffer {
public:
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDeviceSize m_size = 0;
    bool Initialize(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void CopyData(VkDevice device, const void* data);
    void Cleanup(VkDevice device);
    VkBuffer GetBuffer() const;
    VkDeviceSize GetSize() const;

private:
    bool CreateBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    bool AllocateMemory(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkMemoryPropertyFlags properties);
    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
