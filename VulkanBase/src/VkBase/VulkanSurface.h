#ifndef VULKANSURFACE_H_
#define VULKANSURFACE_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanSurface
 *
 * 负责创建与销毁 VkSurfaceKHR
 *
 * 职责范围：
 * 1. 将窗口系统（GLFW）与 Vulkan 连接
 * 2. 管理 VkSurfaceKHR 生命周期
 *
 */

class VulkanSurface
{
public:
    VulkanSurface();

    ~VulkanSurface();

public:
    /**
     * @brief 创建 Vulkan Surface
     * @param instance Vulkan Instance
     * @param window GLFW 窗口指针
     */
    bool Create(VkInstance instance, GLFWwindow *window);

    /**
     * @brief 销毁 Vulkan Surface
     */
    void Destroy();

    /**
     * @brief 获取 VkSurfaceKHR
     */
    VkSurfaceKHR Get() const
    {
        return _surface;
    }

private:
    // Vulkan Surface 句柄
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    // 所属 Vulkan Instance（销毁时需要）
    VkInstance _instance = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANSURFACE_H_
