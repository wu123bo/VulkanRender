#include "VulkanBase.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanBase::VulkanBase()
{
    _instance = new VulkanInstance();
    _surface = new VulkanSurface();
    _physicalDevice = new VulkanPhysicalDevice();
    _device = new VulkanDevice();
    _swapchain = new VulkanSwapchain();
    _renderPass = new VulkanRenderPass();
    _framebuffer = new VulkanFramebuffer();
    _commandPool = new VulkanCommandPool();
    _commandBuffer = new VulkanCommandBuffer();
}

VulkanBase::~VulkanBase()
{
    Shutdown();
}

int VulkanBase::InitVulkan(GLFWwindow *window)
{
    if (_initialized) {
        return true;
    }

    bool ret = createInstance();
    if (!ret) {
        return false;
    }

    ret = createSurface(window);
    if (!ret) {
        return false;
    }

    ret = _physicalDevice->Pick(_instance->Get(), _surface->Get());
    if (!ret) {
        return false;
    }

    ret = _device->Init(_physicalDevice);
    if (!ret) {
        return false;
    }

    glfwGetFramebufferSize(window, &_width, &_height);

    ret = _swapchain->Init(
        _physicalDevice->Get(), _device->Get(), _surface->Get(),
        _physicalDevice->GetGraphicsQueueFamily(),
        _physicalDevice->GetPresentQueueFamily(), _width, _height);
    if (!ret) {
        return false;
    }

    ret = _renderPass->Init(_device->Get(), _swapchain);
    if (!ret) {
        return false;
    }

    ret = _framebuffer->Init(_device->Get(), _renderPass->Get(),
                             _swapchain->GetImageViews(),
                             _swapchain->GetExtent());
    if (!ret) {
        return false;
    }

    ret = _commandPool->Init(_device->Get(),
                             _physicalDevice->GetGraphicsQueueFamily());
    if (!ret) {
        return false;
    }

    // 创建 CommandBuffer（每个 Swapchain Image 一个）
    ret = _commandBuffer->Init(_device->Get(), _commandPool->Get(),
                               _swapchain->GetImageViewCount());

    // 录制每个 CommandBuffer
    for (uint32_t i = 0; i < _commandBuffer->GetCount(); ++i) {
        _commandBuffer->Record(i, _renderPass->Get(), _framebuffer->Get()[i],
                               _swapchain->GetExtent());
    }

    _initialized = true;

    return true;
}

void VulkanBase::Shutdown()
{
    /* 销毁顺序不能乱*/

    // 命令池缓冲区
    SDelete(_commandBuffer);

    // 命令池
    SDelete(_commandPool);

    // 帧缓冲区
    SDelete(_framebuffer);

    // 渲染通道
    SDelete(_renderPass);

    // 交换链
    SDelete(_swapchain);

    // 逻辑设备
    SDelete(_device);

    // 物理设备
    SDelete(_physicalDevice);

    // 窗口表面
    SDelete(_surface);

    // vulkan 实例
    SDelete(_instance);
}

bool VulkanBase::createInstance()
{
    return _instance->Create();
}

bool VulkanBase::createSurface(GLFWwindow *window)
{
    return _surface->Create(_instance->Get(), window);
}

} // namespace VKB
