#ifndef VULKANPIPELINELAYOUT_H_
#define VULKANPIPELINELAYOUT_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanPipelineLayout
 *
 * 描述 Pipeline 可访问的资源布局：
 * - DescriptorSetLayout
 * - PushConstant
 */

class VulkanPipelineLayout
{
public:
    VulkanPipelineLayout();

    ~VulkanPipelineLayout();

public:
    /**
     * @brief 创建 PipelineLayout
     * @param device 逻辑设备
     * @param setLayouts DescriptorSetLayout（可为空）
     * @param pushConstants PushConstant 范围（可为空）
     */
    bool Init(VkDevice device,
              const std::vector<VkDescriptorSetLayout> &setLayouts = {},
              const std::vector<VkPushConstantRange> &pushConstants = {});

    void Destroy();

    VkPipelineLayout Get() const
    {
        return _pipelineLayout;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANPIPELINELAYOUT_H_
