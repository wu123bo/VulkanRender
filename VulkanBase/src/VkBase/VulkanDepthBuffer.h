#ifndef VULKANDEPTHBUFFER_H_
#define VULKANDEPTHBUFFER_H_

#include "VulkanHead.h"

namespace VKB
{

class VulkanDepthBuffer
{
public:
    VulkanDepthBuffer() = default;

    ~VulkanDepthBuffer();

    /**
     * @brief 初始化深度缓冲
     * @param physicalDevice Vulkan 物理设备
     * @param device Vulkan 逻辑设备
     * @param extent 交换链尺寸
     * @param commandPool 命令池，用于 Buffer 拷贝
     * @param graphicsQueue 图形队列
     * @return 是否成功
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkExtent2D extent);

    void Destroy();

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
    VkDevice _device = VK_NULL_HANDLE;

    VkImage _image = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkImageView _imageView = VK_NULL_HANDLE;

    VkFormat _format = VK_FORMAT_D32_SFLOAT; // 默认使用 32-bit 浮点深度
};

} // namespace VKB

#endif // VULKANDEPTHBUFFER_H_