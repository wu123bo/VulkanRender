#ifndef VULKANDEPTHBUFFER_H_
#define VULKANDEPTHBUFFER_H_

#include <vulkan/vulkan.h>

#include "VulkanImage.h"

namespace RHI
{

/**
 * @brief 深度缓冲封装（基于 VulkanImage）
 *
 * 语义层：
 *  - 表示一个 Depth Attachment
 *  - 不关心 RenderPass / Framebuffer
 */
class VulkanDepthBuffer
{
public:
    VulkanDepthBuffer() = default;

    ~VulkanDepthBuffer();

    /**
     * @brief 创建深度缓冲
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkExtent2D extent, VkSampleCountFlagBits samples);

    void Destroy();

    VkImageView GetImageView() const
    {
        return _image.GetImageView();
    }

    VkFormat GetFormat() const
    {
        return _format;
    }

private:
    VulkanImage _image;

    VkFormat _format = VK_FORMAT_UNDEFINED;
};

} // namespace RHI

#endif // !VULKANDEPTHBUFFER_H_