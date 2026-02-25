#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanBuffer
 *
 * 通用 Buffer 封装：
 * - 管理 VkBuffer / VkDeviceMemory
 * - 支持映射写入
 * - 可作为 Vertex / Index / Uniform / Staging Buffer
 */
class VulkanBuffer
{
public:
    VulkanBuffer();

    ~VulkanBuffer();

    /**
     * @brief 创建 Buffer
     * @param physicalDevice 物理设备
     * @param device 逻辑设备
     * @param size Buffer 大小（字节）
     * @param usage Buffer 用途（VERTEX / INDEX / UNIFORM / TRANSFER）
     * @param properties 内存属性
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkDeviceSize size, VkBufferUsageFlags usage,
              VkMemoryPropertyFlags properties);

    void Destroy();

    /// 映射内存
    void *Map();

    /// 解除映射
    void Unmap();

    /// Buffer 句柄
    VkBuffer Get() const
    {
        return m_buffer;
    }

    /// Buffer 大小
    VkDeviceSize GetSize() const
    {
        return m_size;
    }

private:
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);

private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDeviceSize m_size = 0;

    void *m_mapped = nullptr;
};

} // namespace VKB