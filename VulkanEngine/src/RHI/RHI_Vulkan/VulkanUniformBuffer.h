#ifndef VULKANUNIFORMBUFFER_H_
#define VULKANUNIFORMBUFFER_H_

#include "VulkanBuffer.h"

namespace RHI
{

class VulkanUniformBuffer
{
public:
    VulkanUniformBuffer() = default;

    VulkanUniformBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                        VkDeviceSize size, uint32_t binding);

    ~VulkanUniformBuffer();

    // 初始化 uniform buffer
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkDeviceSize size, uint32_t binding);

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

    uint32_t GetBinding() const
    {
        return _binding;
    }

private:
    VulkanBuffer _buffer;

    VkDeviceSize _size = 0;

    // shader binding
    uint32_t _binding = 0;
};

} // namespace RHI

#endif // !VULKANUNIFORMBUFFER_H_