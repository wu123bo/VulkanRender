#ifndef VULKANCONTEXT_H_
#define VULKANCONTEXT_H_

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include "VulkanSync.h"

namespace RHI
{
/**
 * @class VulkanContext
 *
 * @brief Vulkan RHI 上下文对象
 *
 * 负责 Vulkan 的生命周期管理与平台绑定：
 *  - Instance / PhysicalDevice / Device
 *  - Surface / Swapchain
 *
 * 不包含任何渲染逻辑或 GPU 资源管理：
 *  - RenderPass / Pipeline
 *  - Buffer / Texture / Descriptor
 *
 * VulkanContext 只作为底层 RHI 入口，被 Renderer / ResourceManager 使用。
 */
class VulkanContext
{
public:
    VulkanContext();

    ~VulkanContext();

    /*
     * 初始化 Vulkan 上下文
     *
     * @param surfaceDesc  平台相关的表面描述（Win32 / GLFW / SDL 等）
     * @param width        初始窗口宽度
     * @param height       初始窗口高度
     */
    bool Init(const SurfaceDescRHI &surfaceDesc, int width, int height);

    /*
     * 释放 Vulkan 所有资源（需在应用退出时调用）
     *
     */
    void Shutdown();

    /*
     * 等待 GPU 空闲（销毁或重建资源前调用）
     *
     */
    void WaitIdle();

    /*
     * 当窗口尺寸变化或 Swapchain 失效时调用
     *
     */
    void RecreateSwapchain(int width, int height);

    // -------- Vulkan 对象访问接口 --------
public:
    // ========================
    // 封装对象访问接口（C++ 封装类）
    // ========================
    VulkanPhysicalDevice *GetPhysicalDevice() const
    {
        return _physicalDevice;
    }

    VulkanDevice *GetDevice() const
    {
        return _device;
    }

    VulkanSwapchain *GetSwapchain() const
    {
        return _swapchain;
    }

    VulkanCommandPool *GetCommandPool() const
    {
        return _commandPool;
    }

    VulkanSync *GetSync() const
    {
        return _sync;
    }

    // ========================
    // 原生 Vulkan 对象访问接口（Vk* 对象）
    // ========================
    VkPhysicalDevice GetVkPhysicalDevice() const
    {
        if (nullptr == _physicalDevice) {
            return VK_NULL_HANDLE;
        }

        return _physicalDevice->Get();
    }

    VkDevice GetVkDevice() const
    {
        if (nullptr == _device) {
            return VK_NULL_HANDLE;
        }

        return _device->Get();
    }

    VkSwapchainKHR GetVkSwapchain() const
    {
        if (nullptr == _swapchain) {
            return VK_NULL_HANDLE;
        }

        return _swapchain->Get();
    }

    VkCommandPool GetVkCommandPool() const
    {
        if (nullptr == _commandPool) {
            return VK_NULL_HANDLE;
        }

        return _commandPool->Get();
    }

    VkQueue GetGraphicsQueue() const
    {
        if (nullptr == _device) {
            return VK_NULL_HANDLE;
        }

        return _device->GetGraphicsQueue();
    }

    VkSemaphore GetImageAvailableSemaphore(uint32_t frameIndex) const
    {
        if (nullptr == _sync) {
            return VK_NULL_HANDLE;
        }

        return _sync->GetImageAvailable(frameIndex);
    }

    VkSemaphore GetRenderFinishedSemaphore(uint32_t frameIndex) const
    {
        if (nullptr == _sync) {
            return VK_NULL_HANDLE;
        }

        return _sync->GetRenderFinished(frameIndex);
    }

    VkFence GetInFlightFence(uint32_t frameIndex) const
    {
        if (nullptr == _sync) {
            return VK_NULL_HANDLE;
        }

        return _sync->GetInFlightFence(frameIndex);
    }

private:
    // Vulkan 实例（加载扩展、校验层）
    VulkanInstance *_instance = nullptr;

    // Vulkan 表面（与平台窗口绑定）
    VulkanSurface *_surface = nullptr;

    // 物理设备（GPU 选择）
    VulkanPhysicalDevice *_physicalDevice = nullptr;

    // 逻辑设备（队列、功能特性）
    VulkanDevice *_device = nullptr;

    // 交换链（窗口图像缓冲）
    VulkanSwapchain *_swapchain = nullptr;

    // 命令池（分配 CommandBuffer）
    VulkanCommandPool *_commandPool = nullptr;

    // 同步对象（Fence / Semaphore，多帧并行）
    VulkanSync *_sync = nullptr;
};

} // namespace RHI

#endif // !VULKANCONTEXT_H_