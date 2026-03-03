#include "VulkanImage.h"

#include "PrintMsg.h"
#include "VulkanUtils.h"

namespace RHI
{

VulkanImage::~VulkanImage()
{
    Destroy();
}

bool VulkanImage::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                       uint32_t width, uint32_t height, VkFormat format,
                       VkImageUsageFlags usage, VkImageAspectFlags aspectFlags,
                       VkSampleCountFlagBits samples)
{
    _device = device;
    _format = format;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = {width, height, 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = samples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult ret = vkCreateImage(device, &imageInfo, nullptr, &_image);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建纹理图像视图失败!");
        return false;
    }

    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(device, _image, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(physicalDevice, memReq.memoryTypeBits,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkAllocateMemory(device, &allocInfo, nullptr, &_memory);
    vkBindImageMemory(device, _image, _memory, 0);

    return createImageView(aspectFlags);
}

void VulkanImage::TransitionLayout(VkCommandPool commandPool, VkQueue queue,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout)
{
    VkCommandBuffer cmd = BeginSingleTimeCommand(_device, commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = _image;
    barrier.subresourceRange.aspectMask =
        (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            ? VK_IMAGE_ASPECT_DEPTH_BIT
            : VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage = 0;
    VkPipelineStageFlags dstStage = 0;

    // UNDEFINED -> TRANSFER_DST
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    // TRANSFER_DST -> SHADER_READ_ONLY
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
             && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT; // ✅ 关键修正
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::runtime_error("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier);

    EndSingleTimeCommand(_device, commandPool, queue, cmd);
}

void VulkanImage::CopyFromBuffer(VkCommandPool commandPool, VkQueue queue,
                                 VkBuffer buffer, uint32_t width,
                                 uint32_t height)
{
    VkCommandBuffer cmd = BeginSingleTimeCommand(_device, commandPool);

    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(cmd, buffer, _image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSingleTimeCommand(_device, commandPool, queue, cmd);
}

bool VulkanImage::createImageView(VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    VkResult ret = vkCreateImageView(_device, &viewInfo, nullptr, &_imageView);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建纹理图像视图失败!");
        return false;
    }

    return true;
}

void VulkanImage::Destroy()
{
    if (_imageView) {
        vkDestroyImageView(_device, _imageView, nullptr);
    }

    if (_image) {
        vkDestroyImage(_device, _image, nullptr);
    }

    if (_memory) {
        vkFreeMemory(_device, _memory, nullptr);
    }

    _imageView = VK_NULL_HANDLE;
    _image = VK_NULL_HANDLE;
    _memory = VK_NULL_HANDLE;
}

} // namespace RHI