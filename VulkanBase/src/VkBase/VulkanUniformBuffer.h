#ifndef VULKANUNIFORMBUFFER_H_
#define VULKANUNIFORMBUFFER_H_

#include "VulkanBuffer.h"

namespace VKB
{

class VulkanUniformBuffer
{
public:
    VulkanUniformBuffer() = default;

    ~VulkanUniformBuffer();

    // 初始化 uniform buffer
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkDeviceSize size);

    // 更新 uniform 数据
    void Update(const void *data, VkDeviceSize size);

    void Destroy();

    VkBuffer Get() const
    {
        return _buffer.Get();
    }

    VkDeviceSize GetSize() const
    {
        return _size;
    }

private:
    VulkanBuffer _buffer;

    VkDeviceSize _size = 0;
};

} // namespace VKB

#endif // !VULKANUNIFORMBUFFER_H_