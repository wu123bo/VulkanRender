#ifndef VULKANDEVICE_H_
#define VULKANDEVICE_H_

#include "VulkanPhysicalDevice.h"

namespace VKB
{

/**
 * @brief VulkanDevice
 *
 * 负责逻辑设备 VkDevice 和 Queue 的创建
 *
 * 依赖：
 * - VulkanPhysicalDevice（必须先选出 PhysicalDevice）
 */

class VulkanDevice
{
public:
    VulkanDevice();

    ~VulkanDevice();

public:
    /**
     * @brief 初始化 Logical Device
     * @param physicalDevice 已选择的物理设备
     */
    bool Init(const VulkanPhysicalDevice *physicalDevice);

    /**
     * @brief 销毁 Logical Device
     */
    void Destroy();

    /**
     * @brief 获取逻辑设备
     */
    VkDevice Get() const
    {
        return _device;
    }

    /**
     * @brief 获取图形队列
     */
    VkQueue GetGraphicsQueue() const
    {
        return _graphicsQueue;
    }

    /**
     * @brief 获取呈现队列
     */
    VkQueue GetPresentQueue() const
    {
        return _presentQueue;
    }

private:
    // 逻辑设备
    VkDevice _device = VK_NULL_HANDLE;

    // 图形队列
    VkQueue _graphicsQueue = VK_NULL_HANDLE;

    // 呈现队列
    VkQueue _presentQueue = VK_NULL_HANDLE;

private:
    // 物理设备必需支持扩展
    const std::vector<const char *> _deviceExtensions = {
        // 必须启用交换链(swapChain)扩展
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

} // namespace VKB

#endif // !VULKANDEVICE_H_
