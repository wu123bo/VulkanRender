#ifndef VULKANVERTEXBUFFER_H_
#define VULKANVERTEXBUFFER_H_

#include "VulkanBuffer.h"

namespace VKB
{

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer() = default;

    ~VulkanVertexBuffer();

    /**
     * @brief 使用 staging buffer 创建顶点缓冲
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkCommandPool commandPool, VkQueue graphicsQueue,
              const void *vertexData, VkDeviceSize size);

    void Destroy();

    VkBuffer Get() const
    {
        return _buffer.Get();
    }

private:
    VulkanBuffer _buffer;
};

} // namespace VKB

#endif // !VULKANVERTEXBUFFER_H_
