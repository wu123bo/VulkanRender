#ifndef VULKANSAMPER_H_
#define VULKANSAMPER_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief 纹理采样封装类
 *
 * 职责：
 *  - 创建 纹理采样VkSampler
 *
 */

class VulkanSampler
{
public:
    VulkanSampler() = default;

    ~VulkanSampler();

    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkFilter filter = VK_FILTER_LINEAR,
              VkSamplerAddressMode addrMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

    VkSampler Get() const
    {
        return _sampler;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkSampler _sampler = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANSAMPER_H_
