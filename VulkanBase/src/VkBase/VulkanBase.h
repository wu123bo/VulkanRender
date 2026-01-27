#ifndef VULKANBASE_H_
#define VULKANBASE_H_

#include "MacroHead.h"

#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"
#include "VulkanRenderPass.h"
#include "VulkanShaderModule.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include "VulkanSync.h"

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

    VulkanCommandPool *_commandPool = nullptr;

    VulkanCommandBuffer *_commandBuffer = nullptr;

    VulkanPipelineLayout *_pipelineLayout = nullptr;

    VulkanPipeline *_pipeline = nullptr;

    VulkanSync *_sync = nullptr;

    std::vector<VulkanShaderModule *> _shaderModule;

private:
    int _width = 0;

    int _height = 0;

    uint32_t _currentFrame = 0;

    bool _initialized = false;
};
} // namespace VKB

#endif // !VULKANBASE_H_
