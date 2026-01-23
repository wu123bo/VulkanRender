#ifndef VULKANPHYSICALDEVICE_H_
#define VULKANPHYSICALDEVICE_H_

#include <optional>

#include "MacroHead.h"

namespace VKB
{

/**
 * @brief VulkanPhysicalDevice
 *
 * 负责物理设备（GPU）的选择与能力查询
 *
 * 依赖：
 * - VkInstance
 * - VkSurfaceKHR（用于判断 Present 支持）
 */

class VulkanPhysicalDevice
{
private:
    // 队列族索引
    struct QueueFamily
    {
        // 支持图形操作的队列族索引
        std::optional<uint32_t> graphicsFamily;

        // 支持呈现操作的队列族索引
        std::optional<uint32_t> presentFamily;

        // 重置
        void Reset()
        {
            graphicsFamily.reset();
            presentFamily.reset();
        }

        // 通用检查
        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

public:
    VulkanPhysicalDevice();

    ~VulkanPhysicalDevice();

public:
    /**
     * @brief 选择一个合适的物理设备
     */
    bool Pick(VkInstance instance, VkSurfaceKHR surface);

    /**
     * @brief 获取 VkPhysicalDevice
     */
    VkPhysicalDevice Get() const
    {
        return _physicalDevice;
    }

    /**
     * @brief Queue Family 支持图形操作的队列族索引
     */
    uint32_t GetGraphicsQueueFamily() const
    {
        return _queueFamilyIndices.graphicsFamily.value();
    }

    /**
     * @brief Queue Family 支持呈现操作的队列族索引
     */
    uint32_t GetPresentQueueFamily() const
    {
        return _queueFamilyIndices.presentFamily.value();
    }

    /**
     * @brief 设备属性（名称、类型等）
     */
    const VkPhysicalDeviceProperties &GetProperties() const
    {
        return _properties;
    }

private:
    /**
     * @brief 检查某个物理设备是否满足最低要求
     */
    bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    /**
     * @brief 查找 Queue Family
     */
    void findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

    /**
     * @brief 检查 Swapchain 扩展是否支持
     */
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    /**
     * @brief 评分设备
     * - 独立显卡加分
     * - 最大图形队列数量加分
     * - 支持更多特性可以加分
     */
    int RateDevice(VkPhysicalDevice device);

private:
    // 选中的物理设备
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

    // 队列族索引
    QueueFamily _queueFamilyIndices;

    // 设备属性
    VkPhysicalDeviceProperties _properties{};

    // 必需的设备扩展
    const std::vector<const char *> _requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

} // namespace VKB

#endif // !VULKANPHYSICALDEVICE_H_
