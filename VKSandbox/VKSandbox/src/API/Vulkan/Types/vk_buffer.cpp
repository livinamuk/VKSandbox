#include "vk_buffer.h"
#include <iostream>

bool VulkanBuffer::Initialize(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    m_size = size;
    if (!CreateBuffer(physicalDevice, device, size, usage, properties)) {
        return false;
    }

    if (!AllocateMemory(physicalDevice, device, size, properties)) {
        return false;
    }
    return true;
}

bool VulkanBuffer::CreateBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &m_buffer);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create Vulkan buffer!\n";
        return false;
    }
    return true;
}

bool VulkanBuffer::AllocateMemory(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkMemoryPropertyFlags properties) {
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, m_buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties);

    VkResult result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_memory);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to allocate memory for Vulkan buffer!\n";
        return false;
    }

    vkBindBufferMemory(device, m_buffer, m_memory, 0);
    return true;
}

uint32_t VulkanBuffer::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    std::cout << "Failed to find suitable memory type!\n";
    return -1;
}

void VulkanBuffer::CopyData(VkDevice device, const void* data) {
    void* mappedData;
    vkMapMemory(device, m_memory, 0, m_size, 0, &mappedData);
    memcpy(mappedData, data, m_size);
    vkUnmapMemory(device, m_memory);
}

void VulkanBuffer::Cleanup(VkDevice device) {
    if (m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
    }
    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }
}

VkBuffer VulkanBuffer::GetBuffer() const {
    return m_buffer;
}

VkDeviceSize VulkanBuffer::GetSize() const {
    return m_size;
}
