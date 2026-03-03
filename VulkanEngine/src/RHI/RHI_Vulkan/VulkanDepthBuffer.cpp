#include "VulkanDepthBuffer.h"

#include "VulkanUtils.h"

namespace RHI
{

VulkanDepthBuffer::~VulkanDepthBuffer()
{
    Destroy();
}

bool VulkanDepthBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                             VkExtent2D extent, VkSampleCountFlagBits samples)
{
    // 查找物理设备支持的深度格式
    _format = FindDepthFormat(physicalDevice);

    return _image.Init(physicalDevice, device, extent.width, extent.height,
                       _format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                       VK_IMAGE_ASPECT_DEPTH_BIT, samples);
}

void VulkanDepthBuffer::Destroy()
{
    _image.Destroy();
}

} // namespace RHI