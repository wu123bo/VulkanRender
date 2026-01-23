#ifndef VULKANINSTANCE_H_
#define VULKANINSTANCE_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanInstance
 *
 * 负责 Vulkan Instance 的创建与销毁。
 *
 * 职责范围：
 * 1. 创建 VkInstance
 * 2. 管理 Validation Layer
 * 3. 管理 Debug Utils Messenger（调试回调）
 */

class VulkanInstance
{
public:
    VulkanInstance();

    ~VulkanInstance();

public:
    /**
     * @brief 销毁 Vulkan Instance 及其 Debug 资源
     */
    bool Create();

    /**
     * @brief 销毁 Vulkan Instance 及其 Debug 资源
     */
    void Destroy();

    /**
     * @brief 获取 VkInstance 句柄
     */
    VkInstance Get() const
    {
        return _instance;
    }

private:
    /**
     * @brief 实际调用 vkCreateInstance
     */
    bool createInstance();

    /**
     * @brief 创建 Debug Utils Messenger
     *        仅在启用 Validation Layer 时调用
     */
    bool setupDebugMessenger();

    /**
     * @brief 检查请求的 Validation Layer 是否存在
     */
    bool checkValidationLayerSupport() const;

    /**
     * @brief 获取创建 Instance 所需的扩展列表
     *        包含 GLFW 扩展和 Debug 扩展
     */
    std::vector<const char *> getRequiredExtensions() const;

    /**
     * @brief Vulkan Debug 回调函数
     *        所有 Validation Layer 输出都会走这里
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                  VkDebugUtilsMessageTypeFlagsEXT type,
                  const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
                  void *userData);

    /**
     * @brief 填充 Debug Messenger 创建信息
     */
    void
    populateDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

private:
    // Vulkan Instance 句柄
    VkInstance _instance = VK_NULL_HANDLE;

    // Debug Utils Messenger 句柄
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;

#ifdef _DEBUG
    /// 是否启用 Validation Layer（Debug 模式）
    const bool _enableValidationLayers = true;
#else
    /// Release 模式默认关闭 Validation Layer
    const bool _enableValidationLayers = false;
#endif

    /// 请求的 Validation Layer 列表
    const std::vector<const char *> _validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
};

} // namespace VKB

#endif // !VULKANINSTANCE_H_
