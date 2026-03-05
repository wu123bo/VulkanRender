#include "VulkanTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace RHI
{
VulkanTexture::VulkanTexture(VkPhysicalDevice physicalDevice, VkDevice device,
                             VkCommandPool commandPool, VkQueue graphicsQueue,
                             const std::string &filename,
                             VkSampleCountFlagBits samples)
{
    InitFromFile(physicalDevice, device, commandPool, graphicsQueue, filename,
                 samples);
}

VulkanTexture::~VulkanTexture()
{
    Destroy();
}

bool VulkanTexture::InitFromFile(VkPhysicalDevice physicalDevice,
                                 VkDevice device, VkCommandPool commandPool,
                                 VkQueue graphicsQueue,
                                 const std::string &filename,
                                 VkSampleCountFlagBits samples)
{
    int width, height, channels;
    stbi_uc *pixels =
        stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!pixels) {
        return false;
    }

    VkDeviceSize size = width * height * 4;

    // staging buffer
    VulkanBuffer staging;
    staging.Init(physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                     | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    memcpy(staging.Map(), pixels, size);
    staging.Unmap();
    stbi_image_free(pixels);

    // image
    _image.Init(physicalDevice, device, width, height, VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_ASPECT_COLOR_BIT, samples);

    _image.TransitionLayout(commandPool, graphicsQueue,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    _image.CopyFromBuffer(commandPool, graphicsQueue, staging.Get(), width,
                          height);

    _image.TransitionLayout(commandPool, graphicsQueue,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    staging.Destroy();
    return true;
}

void VulkanTexture::Destroy()
{
    _image.Destroy();
}

} // namespace RHI