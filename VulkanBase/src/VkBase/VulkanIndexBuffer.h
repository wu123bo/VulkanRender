#ifndef VULKANINDEXBUFFER_H_
#define VULKANINDEXBUFFER_H_

#include "VulkanBuffer.h"

namespace VKB
{

class VulkanIndexBuffer
{
public:
    VulkanIndexBuffer() = default;

    ~VulkanIndexBuffer();

    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkCommandPool commandPool, VkQueue graphicsQueue,
              const void *indexData, VkDeviceSize size);

    void Destroy();

    VkBuffer Get() const
    {
        return _buffer.Get();
    }

private:
    VulkanBuffer _buffer;
};

} // namespace VKB

#endif // !VULKANINDEXBUFFER_H_
