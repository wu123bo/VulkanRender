#ifndef VULKANBASE_H_
#define VULKANBASE_H_

#include "MacroHead.h"

#include "VulkanAttachmentDesc.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDepthBuffer.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"
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
#include "VulkanUniformBuffer.h"
#include "VulkanUtils.h"
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

    // 更新uniform缓冲区
    void updateUniformBuffer(uint32_t currentImage);

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

    VulkanDescriptorSetLayout *_descriptorSetLayout = nullptr;

    std::vector<VulkanUniformBuffer> _uniformMVPBuffer;

    std::vector<VulkanUniformBuffer> _uniformColorBuffer;

    VulkanDescriptorPool *_descriptorPool = nullptr;

    std::vector<VkDescriptorSet>
        _descriptorSets; // 存放所有分配的 DescriptorSet

    VulkanDepthBuffer *_depthBuffer = nullptr;

    VulkanAttachmentDesc _attachmentDesc; // 附件描述

private:
    MvpMatrix _MVP;

private:
    GLFWwindow *_window = nullptr;

    bool _framebufferResized = false;

private:
    uint32_t _vertexCount = 0;

    const std::vector<VerCor> _vertices = {
        // Front (+Z)
        {{-0.5f, -0.5f, 0.5f}, {1, 0, 0}},
        {{0.5f, -0.5f, 0.5f}, {1, 0, 0}},
        {{0.5f, 0.5f, 0.5f}, {1, 0, 0}},
        {{-0.5f, 0.5f, 0.5f}, {1, 0, 0}},

        // Back (-Z)
        {{-0.5f, -0.5f, -0.5f}, {0, 1, 0}},
        {{0.5f, -0.5f, -0.5f}, {0, 1, 0}},
        {{0.5f, 0.5f, -0.5f}, {0, 1, 0}},
        {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}},

        // Left (-X)
        {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}},
        {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}},
        {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}},
        {{-0.5f, 0.5f, -0.5f}, {0, 0, 1}},

        // Right (+X)
        {{0.5f, -0.5f, -0.5f}, {1, 1, 0}},
        {{0.5f, -0.5f, 0.5f}, {1, 1, 0}},
        {{0.5f, 0.5f, 0.5f}, {1, 1, 0}},
        {{0.5f, 0.5f, -0.5f}, {1, 1, 0}},

        // Top (+Y)
        {{-0.5f, 0.5f, -0.5f}, {1, 0, 1}},
        {{0.5f, 0.5f, -0.5f}, {1, 0, 1}},
        {{0.5f, 0.5f, 0.5f}, {1, 0, 1}},
        {{-0.5f, 0.5f, 0.5f}, {1, 0, 1}},

        // Bottom (-Y)
        {{-0.5f, -0.5f, -0.5f}, {0, 1, 1}},
        {{0.5f, -0.5f, -0.5f}, {0, 1, 1}},
        {{0.5f, -0.5f, 0.5f}, {0, 1, 1}},
        {{-0.5f, -0.5f, 0.5f}, {0, 1, 1}},
    };

    uint32_t _indexCount = 0;

    const std::vector<uint32_t> _indices = {
        0,  1,  2,  2,  3,  0,  // Front
        4,  5,  6,  6,  7,  4,  // Back
        8,  9,  10, 10, 11, 8,  // Left
        12, 13, 14, 14, 15, 12, // Right
        16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20  // Bottom
    };

private:
    int _width = 0;
    int _height = 0;
    bool _initialized = false;

    uint32_t _currentFrame = 0;
    uint32_t MAX_FRAMES_IN_FLIGHT = 2;

private:
    PTF_3D _cameraPos = PTF_3D(0.0f, 5.0f, 0.0f);
};
} // namespace VKB

#endif // !VULKANBASE_H_
