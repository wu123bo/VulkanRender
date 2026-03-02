#ifndef VULKANIMAGE_H_
#define VULKANIMAGE_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief Vulkan Image 资源封装（Image + Memory + ImageView）
 *
 * 对应 VulkanBuffer 的 Image 版本
 */
class VulkanImage
{
public:
    VulkanImage() = default;

    ~VulkanImage();

    /**
     * @brief 创建 Image 并分配显存
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width,
              uint32_t height, VkFormat format, VkImageUsageFlags usage,
              VkImageAspectFlags aspectFlags,
              VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

    /**
     * @brief Image Layout 转换
     */
    void TransitionLayout(VkCommandPool commandPool, VkQueue queue,
                          VkImageLayout oldLayout, VkImageLayout newLayout);

    /**
     * @brief 从 Buffer 拷贝到 Image
     */
    void CopyFromBuffer(VkCommandPool commandPool, VkQueue queue,
                        VkBuffer buffer, uint32_t width, uint32_t height);

    void Destroy();

    // =========================
    // Getter
    // =========================
    VkImage GetImage() const
    {
        return _image;
    }
    VkImageView GetImageView() const
    {
        return _imageView;
    }
    VkFormat GetFormat() const
    {
        return _format;
    }

private:
    void createImageView(VkImageAspectFlags aspectFlags);

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkImage _image = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkImageView _imageView = VK_NULL_HANDLE;

    VkFormat _format = VK_FORMAT_UNDEFINED;
};

} // namespace VKB

#endif // !VULKANIMAGE_H_