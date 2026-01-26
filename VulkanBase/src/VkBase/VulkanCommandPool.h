#ifndef VULKANCOMMANDPOOL_H_
#define VULKANCOMMANDPOOL_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanCommandPool
 *
 * CommandBuffer 的分配源：
 * - 与某个队列族绑定（通常是 Graphics）
 * - 可选择允许 CommandBuffer 单独 reset
 */

class VulkanCommandPool
{
public:
    VulkanCommandPool();

    ~VulkanCommandPool();

public:
    /**
     * @brief 初始化 CommandPool
     * @param device 逻辑设备
     * @param queueFamilyIndex 队列族索引（通常 Graphics）
     */
    bool Init(VkDevice device, uint32_t queueFamilyIndex);

    void Destroy();

    /**
     * @brief 获取命令池
     */
    VkCommandPool Get() const
    {
        return _commandPool;
    }

private:
    // 逻辑设备
    VkDevice _device = VK_NULL_HANDLE;

    // 命令池
    VkCommandPool _commandPool = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANCOMMANDPOOL_H_
