#ifndef VULKANBASE_H_
#define VULKANBASE_H_

#include "MacroHead.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

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

private:
    bool _initialized = false;
};
} // namespace VKB

#endif // !VULKANBASE_H_
