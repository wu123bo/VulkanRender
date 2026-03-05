#ifndef VULKANVERTEXBUFFER_H_
#define VULKANVERTEXBUFFER_H_

#include "VulkanBuffer.h"

namespace RHI
{

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer() = default;

    VulkanVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                       VkCommandPool commandPool, VkQueue graphicsQueue,
                       const void *vertexData, VkDeviceSize size,
                       uint32_t stride);

    ~VulkanVertexBuffer();

    /**
     * @brief 使用 staging buffer 创建顶点缓冲
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkCommandPool commandPool, VkQueue graphicsQueue,
              const void *vertexData, VkDeviceSize size, uint32_t stride);

    void Destroy();

    VkBuffer Get() const
    {
        return _buffer.Get();
    }

    uint32_t GetStride() const
    {
        return _stride;
    }

private:
    VulkanBuffer _buffer;

    // 每个顶点的字节数
    uint32_t _stride = 0;
};

} // namespace RHI

#endif // !VULKANVERTEXBUFFER_H_
