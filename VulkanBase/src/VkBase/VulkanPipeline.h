#ifndef VULKANPIPELINE_H_
#define VULKANPIPELINE_H_

#include "VulkanHead.h"
#include "VulkanShaderModule.h"

namespace VKB
{

/**
 * @brief VulkanPipeline
 *
 * 图形管线封装：
 * - 组合固定功能状态
 * - 创建 VkPipeline
 */

class VulkanPipeline
{
public:
    VulkanPipeline();

    ~VulkanPipeline();

public:
    /**
     * @brief 创建 Graphics Pipeline
     * @param device 逻辑设备
     * @param renderPass 关联的 RenderPass
     * @param layout PipelineLayout
     * @param shaders ShaderModules（通常是 vertex + fragment）
     * @param extent 渲染区域（Swapchain extent）
     */
    bool Init(VkDevice device, VkRenderPass renderPass, VkPipelineLayout layout,
              const std::vector<VulkanShaderModule *> &shaders,
              VkExtent2D extent);

    void Destroy();

    VkPipeline Get() const
    {
        return _pipeline;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkPipeline _pipeline = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANPIPELINE_H_
