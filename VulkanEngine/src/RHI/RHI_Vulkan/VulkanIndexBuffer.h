#ifndef VULKANINDEXBUFFER_H_
#define VULKANINDEXBUFFER_H_

#include "VulkanBuffer.h"

namespace RHI
{

class VulkanIndexBuffer
{
public:
    VulkanIndexBuffer() = default;

    VulkanIndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                      VkCommandPool commandPool, VkQueue graphicsQueue,
                      const void *indexData, VkDeviceSize size,
                      VkIndexType indexType);

    ~VulkanIndexBuffer();

    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkCommandPool commandPool, VkQueue graphicsQueue,
              const void *indexData, VkDeviceSize size, VkIndexType indexType);

    void Destroy();

    VkBuffer Get() const
    {
        return _buffer.Get();
    }

    VkIndexType GetIndexType() const
    {
        return _indexType;
    }

private:
    VulkanBuffer _buffer;

    // 索引类型
    VkIndexType _indexType;
};

} // namespace RHI

#endif // !VULKANINDEXBUFFER_H_
