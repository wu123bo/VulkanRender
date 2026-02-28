#include "VulkanTexture.h"

#include <cstring>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "VulkanUtils.h"

namespace VKB
{

VulkanTexture::~VulkanTexture()
{
    Destroy();
}

bool VulkanTexture::InitFromFile(VkPhysicalDevice physicalDevice,
                                 VkDevice device, VkCommandPool commandPool,
                                 VkQueue graphicsQueue,
                                 const std::string &filename)
{
    _device = device;

    // =========================
    // 1. 加载图片到 CPU
    // =========================
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(filename.c_str(), &texWidth, &texHeight,
                                &texChannels, STBI_rgb_alpha);

    if (!pixels) {
        return false;
    }

    VkDeviceSize imageSize = texWidth * texHeight * 4; // RGBA8

    // =========================
    // 2. staging buffer（CPU 可写）
    // =========================
    VulkanBuffer stagingBuffer;
    stagingBuffer.Init(physicalDevice, device, imageSize,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                           | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data = stagingBuffer.Map();
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    stagingBuffer.Unmap();

    stbi_image_free(pixels);

    // =========================
    // 3. 创建 VkImage（device local）
    // =========================
    createImage(physicalDevice, device, texWidth, texHeight,
                VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                _image, _memory);

    // =========================
    // 4. layout: UNDEFINED → TRANSFER_DST
    // =========================
    transitionLayout(commandPool, graphicsQueue, VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // =========================
    // 5. staging buffer → image
    // =========================
    copyFromBuffer(commandPool, graphicsQueue, stagingBuffer.Get(), texWidth,
                   texHeight);

    // =========================
    // 6. layout: TRANSFER_DST → SHADER_READ
    // =========================
    transitionLayout(commandPool, graphicsQueue,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stagingBuffer.Destroy();

    // =========================
    // 7. ImageView
    // =========================
    createImageView(device, VK_FORMAT_R8G8B8A8_UNORM);
    return true;
}

void VulkanTexture::Destroy()
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

//
// =========================
// 内部工具函数
// =========================
//

bool VulkanTexture::createImage(VkPhysicalDevice physicalDevice,
                                VkDevice device, uint32_t width,
                                uint32_t height, VkFormat format,
                                VkImageUsageFlags usage, VkImage &image,
                                VkDeviceMemory &memory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateImage(device, &imageInfo, nullptr, &image);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(device, image, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(physicalDevice, memReq.memoryTypeBits,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkAllocateMemory(device, &allocInfo, nullptr, &memory);
    vkBindImageMemory(device, image, memory, 0);

    return true;
}

void VulkanTexture::transitionLayout(VkCommandPool commandPool, VkQueue queue,
                                     VkImageLayout oldLayout,
                                     VkImageLayout newLayout)
{
    VkCommandBuffer cmd = BeginSingleTimeCommand(_device, commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _image;

    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage{};
    VkPipelineStageFlags dstStage{};

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
               && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier);

    EndSingleTimeCommand(_device, commandPool, queue, cmd);
}

void VulkanTexture::copyFromBuffer(VkCommandPool commandPool, VkQueue queue,
                                   VkBuffer buffer, uint32_t width,
                                   uint32_t height)
{
    VkCommandBuffer cmd = BeginSingleTimeCommand(_device, commandPool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(cmd, buffer, _image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSingleTimeCommand(_device, commandPool, queue, cmd);
}

void VulkanTexture::createImageView(VkDevice device, VkFormat format)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;

    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkResult ret = vkCreateImageView(device, &viewInfo, nullptr, &_imageView);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建纹理视图失败!");
    }
}

} // namespace VKB