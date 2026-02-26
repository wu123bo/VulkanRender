#ifndef VULKANBUFFER_H_
#define VULKANBUFFER_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief Vulkan 通用 Buffer 封装
 *
 * 职责：
 *  - 创建 / 销毁 VkBuffer
 *  - 分配 / 释放 VkDeviceMemory
 *  - Map / Unmap
 *  - Buffer 拷贝（通过 CommandBuffer）
 *
 * 不关心：
 *  - 这是 Vertex / Index / Uniform
 *  - Shader / Descriptor
 */
class VulkanBuffer
{
public:
    VulkanBuffer() = default;

    ~VulkanBuffer();

    /**
     * @brief 创建 Buffer 并分配内存
     *
     * @param physicalDevice 物理设备
     * @param device         逻辑设备
     * @param size           Buffer 大小（字节）
     * @param usage          Buffer 用途（VERTEX / UNIFORM / TRANSFER 等）
     * @param properties     内存属性（HOST_VISIBLE / DEVICE_LOCAL）
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkDeviceSize size, VkBufferUsageFlags usage,
              VkMemoryPropertyFlags properties);

    /**
     * @brief 销毁 Buffer 和内存
     */
    void Destroy();

    /**
     * @brief 映射内存（仅 HOST_VISIBLE 可用）
     */
    void *Map();

    /**
     * @brief 解除内存映射
     */
    void Unmap();

    /**
     * @brief 从另一个 Buffer 拷贝数据
     *
     * 通常用于：
     *  - staging buffer → device local buffer
     */
    void CopyFrom(VulkanBuffer &src, VkCommandPool commandPool, VkQueue queue);

    VkBuffer Get() const
    {
        return _buffer;
    }
    VkDeviceSize GetSize() const
    {
        return _size;
    }

private:
    /**
     * @brief 查找合适的内存类型
     */
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkDeviceSize _size = 0;
};

} // namespace VKB

#endif // VULKANBUFFER_H_