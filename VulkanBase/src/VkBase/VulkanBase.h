#ifndef VULKANBASE_H_
#define VULKANBASE_H_

#include "MacroHead.h"

#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"

namespace VKB
{
class VulkanBase
{
public:
    VulkanBase();

    virtual ~VulkanBase();

    virtual int InitVulkan(GLFWwindow *window);

    void Shutdown();

private:
    bool createInstance();

    bool createSurface(GLFWwindow *window);

private:
    VulkanInstance *_instance = nullptr;

    VulkanSurface *_surface = nullptr;

    VulkanPhysicalDevice *_physicalDevice = nullptr;

    VulkanDevice *_device = nullptr;

    VulkanSwapchain *_swapchain = nullptr;

    VulkanRenderPass *_renderPass = nullptr;

    VulkanFramebuffer *_framebuffer = nullptr;

private:
    int _width = 0;

    int _height = 0;

    bool _initialized = false;
};
} // namespace VKB

#endif // !VULKANBASE_H_
