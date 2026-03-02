#include "VulkanMsaaColorBuffer.h"

#include "VulkanUtils.h"

namespace VKB
{

VulkanMsaaColorBuffer::~VulkanMsaaColorBuffer()
{
    Destroy();
}

bool VulkanMsaaColorBuffer::Init(VkPhysicalDevice physicalDevice,
                                 VkDevice device, VkFormat colorFormat,
                                 VkExtent2D extent,
                                 VkSampleCountFlagBits samples)
{
    return _image.Init(physicalDevice, device, extent.width, extent.height,
                       colorFormat,
                       VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
                           | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                       VK_IMAGE_ASPECT_COLOR_BIT, samples);
}

void VulkanMsaaColorBuffer::Destroy()
{
    _image.Destroy();
}

} // namespace VKB