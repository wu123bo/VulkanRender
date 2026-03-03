#include "VulkanMsaaBuffer.h"

#include "VulkanUtils.h"

namespace RHI
{

VulkanMsaaBuffer::~VulkanMsaaBuffer()
{
    Destroy();
}

bool VulkanMsaaBuffer::Init(VkPhysicalDevice physicalDevice,
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

void VulkanMsaaBuffer::Destroy()
{
    _image.Destroy();
}

} // namespace RHI