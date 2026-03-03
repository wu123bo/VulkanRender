#ifndef VULKANMSAABUFFER_H_
#define VULKANMSAABUFFER_H_

#include "VulkanImage.h"

namespace RHI
{

/**
 * @brief 多重采样颜色缓冲（基于 VulkanImage）
 *
 * 特点：
 *  - samples > 1
 *  - 仅用于 render pass 中间结果
 *  - 不可直接 present / sample
 */
class VulkanMsaaBuffer
{
public:
    VulkanMsaaBuffer() = default;

    ~VulkanMsaaBuffer();

    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkFormat colorFormat, VkExtent2D extent,
              VkSampleCountFlagBits samples);

    void Destroy();

    VkImageView GetImageView() const
    {
        return _image.GetImageView();
    }

private:
    VulkanImage _image;
};

} // namespace RHI

#endif // !VULKANMSAACOLORBUFFER_H_