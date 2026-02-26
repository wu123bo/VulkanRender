#include "VulkanIndexBuffer.h"

namespace VKB
{

VulkanIndexBuffer::~VulkanIndexBuffer()
{
    Destroy();
}

bool VulkanIndexBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                             VkCommandPool commandPool, VkQueue graphicsQueue,
                             const void *indexData, VkDeviceSize size)
{
    // Staging buffer（CPU 可写）
    VulkanBuffer staging;
    staging.Init(physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                     | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data = staging.Map();
    memcpy(data, indexData, static_cast<size_t>(size));
    staging.Unmap();

    // Device local buffer（真正用来画）
    _buffer.Init(physicalDevice, device, size,
                 VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                     | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Copy staging -> device local
    _buffer.CopyFrom(staging, commandPool, graphicsQueue);

    staging.Destroy(); // 可以销毁临时 staging
    return true;
}
void VulkanIndexBuffer::Destroy()
{
    _buffer.Destroy();
}

} // namespace VKB