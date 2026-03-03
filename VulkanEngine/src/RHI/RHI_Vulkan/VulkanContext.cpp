#include "VulkanContext.h"

#include "VulkanUtils.h"

namespace RHI
{
VulkanContext::VulkanContext()
{
    _instance = new VulkanInstance();

    _surface = new VulkanSurface();

    _physicalDevice = new VulkanPhysicalDevice();

    _device = new VulkanDevice();

    _swapchain = new VulkanSwapchain();

    _attachment = new VulkanAttachment();

    _renderPass = new VulkanRenderPass();

    _msaaBuffer = new VulkanMsaaBuffer();

    _depthBuffer = new VulkanDepthBuffer();

    _frameBuffer = new VulkanFramebuffer();

    _commandPool = new VulkanCommandPool();

    _commandBuffer = new VulkanCommandBuffer();
}

VulkanContext::~VulkanContext()
{
}

int VulkanContext::Init(const SurfaceDescRHI &surfaceDesc, int width,
                        int height)
{
    // 创建 Vulkan Instance
    bool ret = _instance->Create();
    if (!ret) {
        return 1;
    }

    // 创建 Vulkan Surface
    ret = _surface->Init(_instance->Get(), surfaceDesc);
    if (!ret) {
        return 1;
    }

    // 创建物理逻辑设备
    ret = createDevice();
    if (!ret) {
        return 1;
    }

    // 初始化交换链
    ret = _swapchain->Init(_physicalDevice, _device->Get(), _surface->Get(),
                           width, height);
    if (!ret) {
        return 1;
    }

    // 创建附件 渲染通道
    ret = createRenderPass();
    if (!ret) {
        return 1;
    }

    // 创建视图缓冲区 帧缓冲区
    ret = createFrameBuffer();
    if (!ret) {
        return 1;
    }

    // 创建命令池 命令缓冲区
    ret = createCommandBuffer();
    if (!ret) {
        return 1;
    }

    return 0;
}

bool VulkanContext::createDevice()
{
    // 选择物理设备
    bool ret = _physicalDevice->Init(_instance->Get(), _surface->Get());
    if (!ret) {
        return false;
    }

    // 初始化逻辑设备
    ret = _device->Init(_physicalDevice);
    return ret;
}

bool VulkanContext::createRenderPass()
{
    // 创建附件
    bool ret = createAttachmentDesc(_attachment);
    if (!ret) {
        return 1;
    }

    // 初始化渲染通道
    ret = _renderPass->Init(_device->Get(), _attachment);
    return ret;
}

bool VulkanContext::createAttachmentDesc(VulkanAttachment *attachment)
{
    auto format = _swapchain->GetFormat();
    auto msaaCount = _physicalDevice->GetMsaaSamples();

    // 颜色附件
    _attachment->AddAttachment(
        format, msaaCount, VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, AttachmentType::COLOR);

    // 深度附件
    _attachment->AddAttachment(FindDepthFormat(_physicalDevice->Get()),
                               msaaCount, VK_ATTACHMENT_LOAD_OP_CLEAR,
                               VK_ATTACHMENT_STORE_OP_DONT_CARE,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                               AttachmentType::DEPTH);

    // 解析附件 因为多重采样的图像不能直接呈现
    _attachment->AddAttachment(
        format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, AttachmentType::RESOLVE);

    return true;
}

bool VulkanContext::createFrameBuffer()
{
    const auto &device = _device->Get();
    const auto &phyDevice = _physicalDevice->Get();
    const auto &msaaCount = _physicalDevice->GetMsaaSamples();

    const auto &format = _swapchain->GetFormat();
    const auto &extent2D = _swapchain->GetExtent();

    // 初始化多重采样视图
    bool ret =
        _msaaBuffer->Init(phyDevice, device, format, extent2D, msaaCount);
    if (!ret) {
        return false;
    }

    // 初始化深度视图
    ret = _depthBuffer->Init(phyDevice, device, extent2D, msaaCount);
    if (!ret) {
        return false;
    }

    const auto &renderPass = _renderPass->Get();
    const auto &viewImage = _swapchain->GetImageViews();
    const auto &msaaImage = _msaaBuffer->GetImageView();
    const auto &depthImage = _depthBuffer->GetImageView();

    // 创建帧缓冲区
    ret = _frameBuffer->Init(device, renderPass, viewImage, msaaImage,
                             depthImage, extent2D);
    return ret;
}

bool VulkanContext::createCommandBuffer()
{
    const auto &device = _device->Get();
    const auto &graphics = _physicalDevice->GetGraphicsQueueFamily();

    // 命令池初始化
    bool ret = _commandPool->Init(device, graphics);
    if (!ret) {
        return 1;
    }

    const auto &pool = _commandPool->Get();
    const auto &imageViewCount = _swapchain->GetImageViewCount();

    // 创建命令缓冲区
    ret = _commandBuffer->Init(device, pool, imageViewCount);
    return ret;
}

} // namespace RHI
