#ifndef VULKANSURFACE_H_
#define VULKANSURFACE_H_

#include "../SurfaceRHI.h"
#include "VulkanHeadRHI.h"

namespace RHI
{

/**
 * @brief VulkanSurface
 *
 * Vulkan 后端中用于：
 * 1. 根据 SurfaceDescRHI 创建 VkSurfaceKHR
 * 2. 管理 VkSurfaceKHR 的生命周期
 *
 * 重要说明：
 * - 这是 Vulkan 后端私有类
 * - VkSurfaceKHR 不会暴露到 RHI 公共接口
 * - GLFW / Win32 / Qt 不会出现在 RHI 中
 */
class VulkanSurface
{
public:
    VulkanSurface() = default;

    ~VulkanSurface();

    /**
     * @brief 初始化 Vulkan Surface
     *
     * @param instance Vulkan Instance（必须已创建）
     * @param desc     API 无关的 Surface 描述
     *
     * @return true  创建成功
     * @return false 创建失败
     *
     * 调用时机：
     * - Vulkan Instance 创建之后
     * - 物理设备选择之前
     */
    bool Init(VkInstance instance, const SurfaceDescRHI &desc);

    /**
     * @brief 销毁 Vulkan Surface
     *
     * - 安全可重复调用
     * - 在 VulkanContext / VulkanRHI 析构时调用
     */
    void Destroy();

    /// 获取 VkSurfaceKHR（仅供 Vulkan 后端内部使用）
    VkSurfaceKHR Get() const
    {
        return _surface;
    }

    /// Surface 是否有效
    bool IsValid() const
    {
        return _surface != VK_NULL_HANDLE;
    }

private:
    /**
     * Windows 平台使用 vkCreateWin32SurfaceKHR
     *
     * SurfaceDescRHI 提供 HWND / HINSTANCE，
     * VulkanSurface 负责将其转换为 Vulkan 结构。
     */
    bool createWin32Surface(const SurfaceDescRHI &desc);

private:
    /// Vulkan Instance（用于销毁 Surface）
    VkInstance _instance = VK_NULL_HANDLE;

    /// Vulkan Surface 句柄
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
};

} // namespace RHI

#endif // VULKANSURFACE_H_