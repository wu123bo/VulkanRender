#ifndef VULKAN_UTILS_H_
#define VULKAN_UTILS_H_

#include <stdexcept>
#include <vulkan/vulkan.h>

namespace VKB
{

/**
 * @brief 根据 typeFilter 和期望属性找到合适的内存类型索引
 *
 * @param physicalDevice Vulkan 物理设备句柄
 * @param typeFilter Vulkan 返回的 memoryTypeBits，可用内存类型位掩码
 * @param properties 期望的内存属性，如 HOST_VISIBLE、DEVICE_LOCAL 等
 * @return uint32_t 内存类型索引
 * @throws std::runtime_error 找不到合适内存类型时抛出异常
 */
inline uint32_t FindMemoryType(VkPhysicalDevice physicalDevice,
                               uint32_t typeFilter,
                               VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        // 检查 typeFilter 指定的内存类型是否可用，且属性符合要求
        if ((typeFilter & (1 << i))
            && (memProperties.memoryTypes[i].propertyFlags & properties)
                   == properties) {
            return i;
        }
    }

    throw std::runtime_error("找不到合适的内存类型!");
}

// 查找支持的格式
inline VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice,
                                    const std::vector<VkFormat> &candidates,
                                    VkImageTiling tiling,
                                    VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        // 查询当前物理设备对该格式的支持能力
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        // 如果要求使用线性布局（通常用于 CPU 访问）
        if (tiling == VK_IMAGE_TILING_LINEAR
            && (props.linearTilingFeatures & features) == features) {

            // 线性布局下，支持所需特性
            return format;
        }
        // 如果要求使用最优布局（GPU 采样 / 渲染常用）
        else if (tiling == VK_IMAGE_TILING_OPTIMAL
                 && (props.optimalTilingFeatures & features) == features) {
            // 最优布局下，支持所需特性

            return format;
        }
    }

    throw std::runtime_error("未找到支持的格式!");
}

// 查找深度格式
inline VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice)
{
    // 查找支持的格式 选择一个包含深度分量并且支持用作深度附件的格式
    return FindSupportedFormat(physicalDevice,
                               {VK_FORMAT_D32_SFLOAT,
                                VK_FORMAT_D32_SFLOAT_S8_UINT,
                                VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace VKB

#endif // VULKAN_UTILS_H_