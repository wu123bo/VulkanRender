#ifndef VULKANRESOURCEMANAGER_H_
#define VULKANRESOURCEMANAGER_H_

#include "VulkanContext.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanIndexBuffer.h"
#include "VulkanSampler.h"
#include "VulkanTexture.h"
#include "VulkanUniformBuffer.h"
#include "VulkanVertexBuffer.h"
#include <cassert>
#include <vector>

namespace RHI
{

/*
 * VulkanResourceManager
 * --------------------------------
 * 职责：
 *  * 管理 Vulkan GPU 资源的创建与销毁
 *  * 统一管理 DescriptorSetLayout / DescriptorPool / Sampler
 *  * 支持多帧 in-flight UniformBuffer 和 DescriptorSet
 *  * 自动在 Swapchain 重建时刷新相关资源
 *
 * 不负责：
 *  * RenderPass / Pipeline
 *  * CommandBuffer 录制
 *  * Draw 调度
 *  * Scene / Mesh / Material 逻辑
 */
class VulkanResourceManager
{
public:
    VulkanResourceManager();

    ~VulkanResourceManager();

    // 初始化资源管理器
    bool Init(VulkanContext *context, uint32_t framesInFlight);

    // 释放所有 GPU 资源
    void Shutdown();

    // -------- Buffer / Texture 创建接口 --------
    VulkanVertexBuffer *CreateVertexBuffer(const void *data, uint32_t size,
                                           uint32_t stride);

    VulkanIndexBuffer *CreateIndexBuffer(const void *data, uint32_t size,
                                         VkIndexType indexType);

    VulkanUniformBuffer *CreateUniformBuffer(uint32_t size, uint32_t binding);

    VulkanTexture *CreateTexture(const char *filePath, bool generateMipmaps);

    // -------- Descriptor 访问 --------
    VulkanSampler *GetDefaultSampler() const
    {
        return _defaultSampler;
    }

    VulkanDescriptorSetLayout *GetDescriptorSetLayout() const
    {
        return _descriptorSetLayout;
    }

    VulkanDescriptorPool *GetDescriptorPool() const
    {
        return _descriptorPool;
    }

    // 当 Swapchain 重建时调用
    void OnSwapchainRecreated(uint32_t framesInFlight);

private:
    // 更新 UniformBuffer / Texture 的 DescriptorSet
    void UpdateUniformBufferDescriptor(VulkanUniformBuffer *ub,
                                       uint32_t binding);

    void UpdateTextureDescriptor(VulkanTexture *tex, uint32_t binding);

private:
    VulkanContext *_context = nullptr;
    uint32_t _framesInFlight = 0;

    // -------- Descriptor --------
    VulkanDescriptorSetLayout *_descriptorSetLayout = nullptr;
    VulkanDescriptorPool *_descriptorPool = nullptr;
    VulkanSampler *_defaultSampler = nullptr;

    // -------- Resource Storage --------
    std::vector<VulkanVertexBuffer *> _vertexBuffers;
    std::vector<VulkanIndexBuffer *> _indexBuffers;
    std::vector<VulkanUniformBuffer *> _uniformBuffers;
    std::vector<VulkanTexture *> _textures;
};

} // namespace RHI

#endif // !VULKANRESOURCEMANAGER_H_
