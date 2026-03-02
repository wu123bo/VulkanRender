#ifndef VULKANMSAACOLORBUFFER_H_
#define VULKANMSAACOLORBUFFER_H_

#include "VulkanImage.h"

namespace VKB
{

/**
 * @brief 多重采样颜色缓冲（MSAA Color Attachment）
 *
 * 特点：
 *  - samples > 1
 *  - 仅用于 render pass 中间结果
 *  - 不可直接 present / sample
 */
class VulkanMsaaColorBuffer
{
public:
    VulkanMsaaColorBuffer() = default;

    ~VulkanMsaaColorBuffer();

    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkFormat colorFormat, VkExtent2D extent,
              VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

    void Destroy();

    VkImageView GetImageView() const
    {
        return _image.GetImageView();
    }

private:
    VulkanImage _image;
};

} // namespace VKB

#endif // !VULKANMSAACOLORBUFFER_H_