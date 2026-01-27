#ifndef VULKANSYNC_H_
#define VULKANSYNC_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanSync
 *
 * 帧同步管理类（Frames in Flight）
 *
 * 负责：
 * - ImageAvailable Semaphore：等待 Swapchain Image 可用
 * - RenderFinished Semaphore：等待渲染完成后再 Present
 * - InFlight Fence：CPU 等待 GPU 完成一帧
 *
 */

class VulkanSync
{
public:
    VulkanSync();

    ~VulkanSync();

public:
    /**
     * @brief 创建同步对象
     * @param device 逻辑设备
     * @param maxFrames 最大并行帧数（通常为 2）
     */
    bool Init(VkDevice device, uint32_t maxFrames = 2);

    /**
     * @brief 销毁所有同步对象
     */
    void Destroy();

    /// 当前帧可用信号量（image acquired）
    VkSemaphore GetImageAvailable(uint32_t frame) const;

    /// 当前帧渲染完成信号量（render finished）
    VkSemaphore GetRenderFinished(uint32_t frame) const;

    /// 当前帧 Fence（CPU ↔ GPU 同步）
    VkFence GetInFlightFence(uint32_t frame) const;

    /// 并行帧数量
    uint32_t GetFrameCount() const
    {
        return _maxFrames;
    }

private:
    // 逻辑设备
    VkDevice _device = VK_NULL_HANDLE;

    // Frames in Flight 数量
    uint32_t _maxFrames = 0;

    // 每帧一个 ImageAvailable Semaphore
    std::vector<VkSemaphore> _imageAvailable;

    // 每帧一个 RenderFinished Semaphore
    std::vector<VkSemaphore> _renderFinished;

    // 每帧一个 Fence，用于 CPU 等待 GPU
    std::vector<VkFence> _inFlightFences;
};

} // namespace VKB

#endif // !VULKANSYNC_H_
