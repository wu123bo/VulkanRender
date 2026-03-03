#ifndef VULKANCONTEXT_H_
#define VULKANCONTEXT_H_

#include "VulkanAttachment.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDepthBuffer.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanInstance.h"
#include "VulkanMsaaBuffer.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"

namespace RHI
{
class VulkanContext
{
public:
    VulkanContext();

    ~VulkanContext();

    int Init(const SurfaceDescRHI &surfaceDesc, int width, int height);

private:
    /**
     * @brief 选择物理设备并创建逻辑设备
     */
    bool createDevice();

    /**
     * @brief 创建附件描述并初始化 渲染通道
     */
    bool createRenderPass();

    /**
     * @brief 创建 VulkanAttachment 附件描述
     *
     * 包括颜色、深度和 MSAA Resolve 附件
     */
    bool createAttachmentDesc(VulkanAttachment *attachment);

    /**
     * @brief 创建视图缓冲区 帧缓冲区
     */
    bool createFrameBuffer();

    /**
     * @brief 创建命令池和命令缓冲区
     */
    bool createCommandBuffer();

private:
    // vulkan实例
    VulkanInstance *_instance = nullptr;

    // vulkan 表面
    VulkanSurface *_surface = nullptr;

    // 物理设备
    VulkanPhysicalDevice *_physicalDevice = nullptr;

    // 逻辑设备
    VulkanDevice *_device = nullptr;

    // 交换链
    VulkanSwapchain *_swapchain = nullptr;

    // 附件存储
    VulkanAttachment *_attachment = nullptr;

    // 渲染通道
    VulkanRenderPass *_renderPass = nullptr;

    // 多重采样视图
    VulkanMsaaBuffer *_msaaBuffer = nullptr;

    // 深度视图
    VulkanDepthBuffer *_depthBuffer = nullptr;

    // 帧缓冲区
    VulkanFramebuffer *_frameBuffer = nullptr;

    // 命令池
    VulkanCommandPool *_commandPool = nullptr;

    // 命令缓冲区
    VulkanCommandBuffer *_commandBuffer = nullptr;
};
} // namespace RHI

#endif // !VULKANCONTEXT_H_
