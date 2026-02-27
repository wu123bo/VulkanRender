#ifndef VULKANDESCRIPTORSETLAYOUT_H_
#define VULKANDESCRIPTORSETLAYOUT_H_

#include "VulkanHead.h"

namespace VKB
{

class VulkanDescriptorSetLayout
{
public:
    VulkanDescriptorSetLayout() = default;

    ~VulkanDescriptorSetLayout();

    // 创建描述符集布局绑定
    VkDescriptorSetLayoutBinding Make(uint32_t binding, VkDescriptorType type,
                                      VkShaderStageFlags stageFlags,
                                      uint32_t count = 1);

    bool Init(VkDevice device,
              const std::vector<VkDescriptorSetLayoutBinding> &bindings);

    VkDescriptorSetLayout Get() const
    {
        return _layout;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkDescriptorSetLayout _layout = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANDESCRIPTORSETLAYOUT_H_