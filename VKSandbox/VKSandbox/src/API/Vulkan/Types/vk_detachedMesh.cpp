#include "vk_detachedMesh.h"
#include "vk_buffer.h"
#include "../Managers/vk_command_manager.h"
#include "../Managers/vk_device_manager.h"

void VulkanDetachedMesh::UpdateBuffers(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    VkDevice device = VulkanDeviceManager::GetDevice();
    VkPhysicalDevice physicalDevice = VulkanDeviceManager::GetPhysicalDevice();

    // Vertices
    VkDeviceSize vertexSize = sizeof(Vertex) * vertices.size();
    if (vertexBuffer.GetSize() < vertexSize) {
        Cleanup(device);
        vertexBuffer.Initialize(physicalDevice, device, vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vertexBuffer.CopyData(device, vertices.data());
    }
    else {
        vertexBuffer.CopyData(device, vertices.data());
    }
    // Indices
    VkDeviceSize indexSize = sizeof(uint32_t) * indices.size();
    if (indexBuffer.GetSize() < indexSize) {
        Cleanup(device);
        indexBuffer.Initialize(physicalDevice, device, indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        indexBuffer.CopyData(device, indices.data());
    }
    else {
        indexBuffer.CopyData(device, indices.data());
    }
}

void VulkanDetachedMesh::Cleanup(VkDevice device) {
    vertexBuffer.Cleanup(device);
    indexBuffer.Cleanup(device);
}
