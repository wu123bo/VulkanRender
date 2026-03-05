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
    _commandPool = new VulkanCommandPool();
    _sync = new VulkanSync();
}

VulkanContext::~VulkanContext()
{
    Shutdown();
}

bool VulkanContext::Init(const SurfaceDescRHI &surfaceDesc, int width,
                         int height)
{
    // 创建 Vulkan Instance
    bool ret = _instance->Create();
    if (!ret) {
        return false;
    }

    // 创建 Vulkan Surface
    ret = _surface->Init(_instance->Get(), surfaceDesc);
    if (!ret) {
        return false;
    }

    // 选择物理设备
    ret = _physicalDevice->Init(_instance->Get(), _surface->Get());
    if (!ret) {
        return false;
    }

    // 初始化逻辑设备
    ret = _device->Init(_physicalDevice);
    if (!ret) {
        return false;
    }

    // 对象引用
    const auto &device = _device->Get();
    const auto &graphics = _physicalDevice->GetGraphicsQueueFamily();

    // 初始化交换链
    ret = _swapchain->Init(_physicalDevice, device, _surface->Get(), width,
                           height);
    if (!ret) {
        return false;
    }

    // 命令池初始化
    ret = _commandPool->Init(device, graphics);
    if (!ret) {
        return false;
    }

    // 同步对象
    ret = _sync->Init(device, _swapchain->GetImageViewCount());
    return ret;
}

void VulkanContext::Shutdown()
{
}

void VulkanContext::WaitIdle()
{
}

void VulkanContext::RecreateSwapchain(int width, int height)
{
}

} // namespace RHI
