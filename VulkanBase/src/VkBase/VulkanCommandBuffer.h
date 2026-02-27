#ifndef VULKANCOMMANDBUFFER_H_
#define VULKANCOMMANDBUFFER_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanCommandBuffer
 *
 * 封装 VkCommandBuffer：
 * - 按 Swapchain Image 数量分配
 * - 支持基础的 RenderPass 录制
 */

class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer();

    ~VulkanCommandBuffer();

public:
    /**
     * @brief 分配 CommandBuffer
     * @param device 逻辑设备
     * @param commandPool CommandPool
     * @param count CommandBuffer 数量（通常 = Swapchain Image 数）
     */
    bool Init(VkDevice device, VkCommandPool commandPool, uint32_t count);

    /**
     * @brief 录制基础 RenderPass 命令
     * @param index 第几个 CommandBuffer
     * @param renderPass RenderPass
     * @param framebuffer 对应 Framebuffer
     * @param extent 渲染区域大小
     * @param extent 渲染管线
     */
    bool Record(uint32_t index, VkRenderPass renderPass,
                VkFramebuffer framebuffer, VkExtent2D extent,
                VkPipeline pipeline, VkBuffer vertexBuffer,
                uint32_t vertexCount);

    bool Record(uint32_t index, VkRenderPass renderPass,
                VkFramebuffer framebuffer, VkExtent2D extent,
                VkPipeline pipeline, VkPipelineLayout pipelineLayout,
                std::vector<VkDescriptorSet> &descriptorSets,
                VkBuffer vertexBuffer, VkBuffer indexBuffer,
                uint32_t indexCount);

    void Destroy();

    /**
     * @brief 获取命令池缓冲区
     */
    VkCommandBuffer Get(uint32_t index) const
    {
        return _commandBuffers[index];
    }

    /**
     * @brief 获取命令池缓冲区数量
     */
    uint32_t GetCount() const
    {
        return static_cast<uint32_t>(_commandBuffers.size());
    }

private:
    glm::vec3 _backColor = glm::vec3(0.0f);

    // 逻辑设备
    VkDevice _device = VK_NULL_HANDLE;

    // 命令池
    VkCommandPool _commandPool = VK_NULL_HANDLE;

    // 命令缓冲区
    std::vector<VkCommandBuffer> _commandBuffers;
};

} // namespace VKB

#endif // !VULKANCOMMANDBUFFER_H_
