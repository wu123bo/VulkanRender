#ifndef VULKANTEXTURE_H_
#define VULKANTEXTURE_H_

#include <string>

#include "VulkanBuffer.h"
#include "VulkanImage.h"

namespace VKB
{

/**
 * @brief 语义级纹理（Texture = Image + 数据来源）
 */
class VulkanTexture
{
public:
    VulkanTexture() = default;

    ~VulkanTexture();

    bool InitFromFile(VkPhysicalDevice physicalDevice, VkDevice device,
                      VkCommandPool commandPool, VkQueue graphicsQueue,
                      const std::string &filename);

    void Destroy();

    VkImageView GetImageView() const
    {
        return _image.GetImageView();
    }

private:
    VulkanImage _image;
};

} // namespace VKB

#endif // !VULKANTEXTURE_H_