#include "VulkanSurface.h"
#include "PrintMsg.h"

namespace RHI
{

VulkanSurface::~VulkanSurface()
{
    Destroy();
}

bool VulkanSurface::Init(VkInstance instance, const SurfaceDescRHI &desc)
{
    if (instance == VK_NULL_HANDLE) {
        PSG::PrintError("VulkanSurface::Init 失败：VkInstance 无效");
        return false;
    }

    // 防止错误后端调用
    if (desc.api != GraphicsAPI::Vulkan) {
        PSG::PrintError("VulkanSurface::Init 失败：GraphicsAPI 不是 Vulkan");
        return false;
    }

    _instance = instance;

#if defined(_WIN32)
    createWin32Surface(desc);
#else
    // 其他平台暂未实现
    PSG::PrintError("当前平台未实现 VulkanSurface");
    return false;
#endif

    return true;
}

void VulkanSurface::Destroy()
{
    /**
     * Vulkan 资源销毁必须使用创建它的 VkInstance
     * 因此需要保存 _instance
     */
    if (_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        _surface = VK_NULL_HANDLE;
        _instance = VK_NULL_HANDLE;
    }
}

bool VulkanSurface::createWin32Surface(const SurfaceDescRHI &desc)
{
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = static_cast<HWND>(desc.hwnd);
    createInfo.hinstance = static_cast<HINSTANCE>(desc.hinstance);

    VkResult ret =
        vkCreateWin32SurfaceKHR(_instance, &createInfo, nullptr, &_surface);

    if (ret != VK_SUCCESS) {
        PSG::PrintError("vkCreateWin32SurfaceKHR 创建失败");
        return false;
    }

    return true;
}

} // namespace RHI