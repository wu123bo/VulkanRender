#ifndef VULKANDESCRIPTORSET_H_
#define VULKANDESCRIPTORSET_H_

#include "VulkanBuffer.h"
#include <vulkan/vulkan.h>

namespace VKB
{

class VulkanDescriptorSet
{
public:
    VulkanDescriptorSet() = default;

    ~VulkanDescriptorSet();

    void Init(VkDevice device, VkDescriptorSet set);

    // 更新 uniform buffer
    void UpdateBuffer(uint32_t binding, const VkBuffer &buffer,
                      uint32_t bufferSize);

    // 更新 uniform buffer
    void UpdateBuffer(uint32_t binding, VkImageView textureImageView,
                      VkSampler textureSampler);

    VkDescriptorSet Get() const
    {
        return _set;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkDescriptorSet _set = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANDESCRIPTORSET_H_