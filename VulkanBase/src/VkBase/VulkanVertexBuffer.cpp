#include "VulkanVertexBuffer.h"

#include <cstring>

namespace VKB
{

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    Destroy();
}

bool VulkanVertexBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                              VkCommandPool commandPool, VkQueue graphicsQueue,
                              const void *vertexData, VkDeviceSize size)
{
    // =========================
    // 1. staging buffer（CPU 可写）
    // =========================
    VulkanBuffer staging;
    staging.Init(physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                     | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data = staging.Map();
    memcpy(data, vertexData, static_cast<size_t>(size));
    staging.Unmap();

    // =========================
    // 2. device local buffer（真正用来画）
    // =========================
    _buffer.Init(physicalDevice, device, size,
                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
                     | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // =========================
    // 3. staging → device local
    // =========================
    _buffer.CopyFrom(staging, commandPool, graphicsQueue);

    staging.Destroy();
    return true;
}

void VulkanVertexBuffer::Destroy()
{
    _buffer.Destroy();
}

} // namespace VKB