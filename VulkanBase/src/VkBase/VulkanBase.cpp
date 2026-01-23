#include "VulkanBase.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanBase::VulkanBase()
{
    _instance = new VulkanInstance();
    _surface = new VulkanSurface();
    _physicalDevice = new VulkanPhysicalDevice();
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

    _initialized = true;

    return true;
}

void VulkanBase::Shutdown()
{
    // 销毁顺序不能乱
    SDelete(_surface);

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
