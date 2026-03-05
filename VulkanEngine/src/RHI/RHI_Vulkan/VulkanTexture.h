#ifndef VULKANTEXTURE_H_
#define VULKANTEXTURE_H_

#include <string>

#include "VulkanBuffer.h"
#include "VulkanImage.h"

namespace RHI
{

/**
 * @brief 语义级纹理（Texture = Image + 数据来源）
 */
class VulkanTexture
{
public:
    VulkanTexture() = default;

    VulkanTexture(VkPhysicalDevice physicalDevice, VkDevice device,
                  VkCommandPool commandPool, VkQueue graphicsQueue,
                  const std::string &filename, VkSampleCountFlagBits samples);

    ~VulkanTexture();

    bool InitFromFile(VkPhysicalDevice physicalDevice, VkDevice device,
                      VkCommandPool commandPool, VkQueue graphicsQueue,
                      const std::string &filename,
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

#endif // !VULKANTEXTURE_H_