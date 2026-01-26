#ifndef VULKANFRAMEBUFFER_H_
#define VULKANFRAMEBUFFER_H_

#include "VulkanRenderPass.h"

namespace VKB
{

/**
 * @brief VulkanFramebuffer
 *
 * 负责将 ImageView 绑定到 RenderPass
 * 强依赖 Swapchain
 */

class VulkanFramebuffer
{
public:
    VulkanFramebuffer();

    ~VulkanFramebuffer();

public:
    /**
     * @brief 创建 Framebuffer（每个 Swapchain Image 一个）
     */
    bool Init(VkDevice device, VkRenderPass renderPass,
              const std::vector<VkImageView> &colorImageViews,
              VkExtent2D extent);

    void Destroy();

    /**
     * @brief 返回帧缓冲对象集合
     */
    const std::vector<VkFramebuffer> &Get() const
    {
        return _framebuffers;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    // 帧缓冲对象集合（每个 Swapchain Image 对应一个 Framebuffer）
    std::vector<VkFramebuffer> _framebuffers;
};

} // namespace VKB

#endif // !VULKANFRAMEBUFFER_H_
