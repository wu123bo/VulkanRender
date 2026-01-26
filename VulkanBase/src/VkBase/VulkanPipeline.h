#ifndef VULKANPIPELINE_H_
#define VULKANPIPELINE_H_

#include "VulkanHead.h"

namespace VKB
{

class VulkanPipeline
{
public:
    VulkanPipeline();

    ~VulkanPipeline();

public:
    /**
     * @brief 创建基础图形管线
     * @param device 逻辑设备
     * @param renderPass RenderPass
     * @param layout PipelineLayout
     * @param extent Swapchain extent（用于 viewport）
     * @param vertShader 顶点着色器模块
     * @param fragShader 片段着色器模块
     */
    bool Init(VkDevice device, VkRenderPass renderPass, VkPipelineLayout layout,
              VkExtent2D extent, VkShaderModule vertShader,
              VkShaderModule fragShader);

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
