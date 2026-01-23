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

    if (!createInstance()) {
        return false;
    }

    if (!createSurface(window)) {
        return false;
    }

    if (!_physicalDevice->Pick(_instance->Get(), _surface->Get())) {
        return false;
    }

    if (!_device->Init(_physicalDevice)) {
        return false;
    }

    _initialized = true;

    return true;
}

void VulkanBase::Shutdown()
{
    /* 销毁顺序不能乱*/

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
