#ifndef VULKANRENDERPASS_H_
#define VULKANRENDERPASS_H_

#include "VulkanSwapchain.h"

namespace VKB
{

/**
 * @brief VulkanRenderPass
 *
 * 只负责描述渲染流程：
 * - 有哪些附件
 * - Subpass 如何使用这些附件
 *
 * 不依赖 Swapchain，不创建 Framebuffer
 */

class VulkanRenderPass
{
public:
    VulkanRenderPass();

    ~VulkanRenderPass();

public:
    /**
     * @brief 初始化 RenderPass 和 Framebuffer
     * @param device 逻辑设备
     * @param swapchain Swapchain 用于获取图像
     */
    bool Init(VkDevice device, const VulkanSwapchain *swapchain);

    /**
     * @brief 销毁 RenderPass 和 Framebuffer
     */
    void Destroy();

    /**
     * @brief 获取渲染通道
     */
    VkRenderPass Get() const
    {
        return _renderPass;
    }

private:
    // 创建附件
    VkAttachmentDescription
    createAttachment(VkFormat format, VkSampleCountFlagBits samples,
                     VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
                     VkAttachmentLoadOp stencilLoadOp,
                     VkAttachmentStoreOp stencilStoreOp,
                     VkImageLayout initialLayout, VkImageLayout finalLayout);

    // 创建附件引用
    VkAttachmentReference makeAttachmentRef(uint32_t attachmentIndex,
                                            VkImageLayout layout);

private:
    // 逻辑设备句柄（用于创建和销毁所有 Vulkan 资源）
    VkDevice _device = VK_NULL_HANDLE;

    // 渲染通道（Render Pass）
    // 渲染通道定义了一系列渲染操作的顺序及目标（颜色、深度等）
    // 主要用于管理图像的读取、写入、依赖关系等
    // 每个渲染通道对应一个渲染阶段，包括输入、输出、依赖等
    VkRenderPass _renderPass = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANRENDERPASS_H_
