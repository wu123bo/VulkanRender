#ifndef VULKANBASE_H_
#define VULKANBASE_H_

#include "MacroHead.h"

#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"
#include "VulkanRenderPass.h"
#include "VulkanShaderModule.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include "VulkanSync.h"
#include "VulkanVertexBuffer.h"

namespace VKB
{
class VulkanBase
{
public:
    VulkanBase();
    virtual ~VulkanBase();

    int InitVulkan(GLFWwindow *window);
    int DrawFrame();
    void Shutdown();

public:
    void SetFramebufferResized(bool resized)
    {
        _framebufferResized = resized;
    }

private:
    bool createInstance();
    bool createSurface(GLFWwindow *window);

    void recreateSwapchain();

    void cleanupSwapchain();

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
    std::vector<VulkanShaderModule *> _shaderModule;
    VulkanPipeline *_pipeline = nullptr;
    VulkanSync *_sync = nullptr;

    VulkanVertexBuffer *_vertexBuffer = nullptr;

    VulkanIndexBuffer *_indexBuffer = nullptr;

private:
    GLFWwindow *_window = nullptr;

    bool _framebufferResized = false;

private:
    uint32_t _vertexCount = 0;

    const std::vector<VerCor> _vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.f, 0.f, 0.f}},
        {{0.5f, -0.5f, 0.0f}, {0.f, 1.f, 0.f}},
        {{0.5f, 0.5f, 0.0f}, {0.f, 0.f, 1.f}},
        {{-0.5f, 0.5f, 0.0f}, {1.f, 1.f, 1.f}},
    };

    uint32_t _indexCount = 0;

    const std::vector<uint32_t> _indices = {0, 1, 2, 2, 3, 0};

private:
    int _width = 0;
    int _height = 0;
    bool _initialized = false;

    uint32_t _currentFrame = 0;
    uint32_t MAX_FRAMES_IN_FLIGHT = 2;
};
} // namespace VKB

#endif // !VULKANBASE_H_
