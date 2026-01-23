#include "VulkanSurface.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanSurface::VulkanSurface()
{
}

VulkanSurface::~VulkanSurface()
{
    Destroy();
}

bool VulkanSurface::Create(VkInstance instance, GLFWwindow *window)
{
    if (nullptr == window) {
        PSG::PrintError("创建表面失败 窗口为空");
        return false;
    }

    // GLFW 提供的跨平台 Surface 创建接口
    VkResult ret =
        glfwCreateWindowSurface(instance, window, nullptr, &_surface);

    if (VK_SUCCESS != ret) {
        PSG::PrintError("创建 VkSurfaceKHR 失败");
        return false;
    }

    _instance = instance;
    return true;
}

void VulkanSurface::Destroy()
{
    // 销毁 surface
    if (_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        _surface = VK_NULL_HANDLE;
        _instance = VK_NULL_HANDLE;
    }
}

} // namespace VKB
