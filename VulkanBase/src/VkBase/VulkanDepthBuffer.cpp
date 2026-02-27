#include "VulkanDepthBuffer.h"
#include "PrintMsg.h"
#include "VulkanUtils.h"

namespace VKB
{

VKB::VulkanDepthBuffer::~VulkanDepthBuffer()
{
    Destroy();
}

bool VulkanDepthBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                             VkExtent2D extent)
{
    _device = device;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = _format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(_device, &imageInfo, nullptr, &_image) != VK_SUCCESS) {
        PSG::PrintError("创建深度图像失败!");
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device, _image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(physicalDevice, memRequirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(_device, &allocInfo, nullptr, &_memory)
        != VK_SUCCESS) {
        PSG::PrintError("分配深度图像内存失败!");
        return false;
    }

    vkBindImageMemory(_device, _image, _memory, 0);

    // 创建 ImageView
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(_device, &viewInfo, nullptr, &_imageView)
        != VK_SUCCESS) {
        PSG::PrintError("创建深度 ImageView 失败!");
        return false;
    }

    return true;
}

void VulkanDepthBuffer::Destroy()
{
    if (_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(_device, _imageView, nullptr);
        _imageView = VK_NULL_HANDLE;
    }
    if (_image != VK_NULL_HANDLE) {
        vkDestroyImage(_device, _image, nullptr);
        _image = VK_NULL_HANDLE;
    }
    if (_memory != VK_NULL_HANDLE) {
        vkFreeMemory(_device, _memory, nullptr);
        _memory = VK_NULL_HANDLE;
    }
}

} // namespace VKB