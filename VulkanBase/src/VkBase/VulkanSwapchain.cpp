#include "VulkanSwapchain.h"

#include <algorithm>
#include <set>

#include "PrintMsg.h"

namespace VKB
{

VulkanSwapchain::VulkanSwapchain()
{
}

VulkanSwapchain::~VulkanSwapchain()
{
    Destroy();
}

bool VulkanSwapchain::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                           VkSurfaceKHR surface, uint32_t graphicsQueueFamily,
                           uint32_t presentQueueFamily, uint32_t width,
                           uint32_t height)
{
    if (physicalDevice == VK_NULL_HANDLE || device == VK_NULL_HANDLE) {
        PSG::PrintError("创建交换链失败 物理或者逻辑设备为空!");
        return false;
    }

    // 查询物理设备 + Surface 对 Swapchain 的支持情况
    SupportDetails support = querySupport(physicalDevice, surface);

    // 判断图像格式支持和呈现模型支持
    if (support.formats.empty() || support.presentModes.empty()) {
        PSG::PrintError("创建交换链失败 当前物理设备不支持任何 "
                        "SurfaceFormat 或 PresentMode");

        return false;
    }

    // 选择最合适的图像格式
    VkSurfaceFormatKHR surfaceFormat = chooseFormat(support.formats);

    // 选择最优的呈现模式
    VkPresentModeKHR presentMode = choosePresentMode(support.presentModes);

    // 选择最终的分辨率
    VkExtent2D extent = chooseExtent(support.capabilities, width, height);

    // 交换链图像数量
    uint32_t imageCount = support.capabilities.minImageCount;

    // 如果交换链图像数量大于支持的最大数量，那么等于支持的最大数量
    if (support.capabilities.maxImageCount > 0
        && imageCount > support.capabilities.maxImageCount) {
        imageCount = support.capabilities.maxImageCount;
    }

    // 交换链创建信息
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // 队列族索引
    uint32_t queueFamilyIndices[] = {graphicsQueueFamily, presentQueueFamily};

    // 判断图形队列和呈现队列是否是同一个队列族
    if (graphicsQueueFamily != presentQueueFamily) {

        // Graphics 与 Present 队列不同族 需要使用并发访问模式 (CONCURRENT)
        // Vulkan 会自动处理队列族间的图像所有权转移
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

        // 指定两个队列族的索引
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {

        // Graphics 与 Present 队列是同一个族 使用独占模式 (EXCLUSIVE)，性能最佳
        // GPU 无需在队列族间转移图像所有权
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    // 指定 Swapchain 图像的变换方式（旋转/翻转）
    createInfo.preTransform = support.capabilities.currentTransform;

    // 指定 Swapchain 的透明度混合方式（Composite Alpha）
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // 指定 Swapchain 呈现模式
    createInfo.presentMode = presentMode;

    // 创建交换链
    VkResult ret =
        vkCreateSwapchainKHR(device, &createInfo, nullptr, &_swapchain);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建交换链失败!");
    }

    // 获取交换链图像
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(device, _swapchain, &count, nullptr);

    _images.resize(count);
    vkGetSwapchainImagesKHR(device, _swapchain, &count, _images.data());

    // 创建交换链图像视图
    if (!createImageViews(device, surfaceFormat.format)) {
        return false;
    }

    // 保存 逻辑设备 分辨率 和 图像格式
    _device = device;
    _extent = extent;
    _format = surfaceFormat.format;
    return true;
}

void VulkanSwapchain::Destroy()
{
    // 销毁图像视图
    for (auto view : _imageViews) {
        vkDestroyImageView(_device, view, nullptr);
    }
    _imageViews.clear();

    // 销毁交换链
    if (_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
        _swapchain = VK_NULL_HANDLE;
    }
}

SupportDetails VulkanSwapchain::querySupport(VkPhysicalDevice device,
                                             VkSurfaceKHR surface)
{
    // 查询 Surface 能力（capabilities）
    SupportDetails details{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    // 查询 Surface 支持的格式（formats）
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         nullptr);
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());

    // 查询呈现(Present) 模式（presentModes）
    uint32_t presentCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount,
                                              nullptr);
    details.presentModes.resize(presentCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount,
                                              details.presentModes.data());

    return details;
}

VkSurfaceFormatKHR
VulkanSwapchain::chooseFormat(const std::vector<VkSurfaceFormatKHR> &formats)
{
    for (const auto &format : formats) {

        // 当前默认使用 VK_COLOR_SPACE_SRGB_NONLINEAR_KHR 格式
        if (format.format == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    // 没有找到返回索引0的格式
    return formats[0];
}

VkPresentModeKHR
VulkanSwapchain::choosePresentMode(const std::vector<VkPresentModeKHR> &modes)
{
    for (const auto &mode : modes) {

        // 优先选择 MAILBOX (低延迟 Triple-buffering 风格 高帧率渲染)
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode;
        }
    }

    // Vulkan 保证所有设备都支持
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
VulkanSwapchain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                              uint32_t width, uint32_t height)
{
    // 窗口系统已经固定了分辨率必须使用
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    // 可变尺寸情况 防止创建 Swapchain 时尺寸超出 GPU 或 Surface 限制
    VkExtent2D extent{};
    extent.width = std::clamp(width, capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);

    extent.height = std::clamp(height, capabilities.minImageExtent.height,
                               capabilities.maxImageExtent.height);

    return extent;
}

bool VulkanSwapchain::createImageViews(VkDevice device, VkFormat format)
{
    if (_images.empty()) {
        PSG::PrintError("创建交换链图像视图失败 图像为空!");
        return false;
    }

    uint32_t count = static_cast<uint32_t>(_images.size());

    // 创建交换链图像视图
    _imageViews.resize(count);
    for (size_t i = 0; i < count; i++) {

        // 创建交换链图像视图信息
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;

        VkResult ret =
            vkCreateImageView(device, &viewInfo, nullptr, &_imageViews[i]);
        if (ret != VK_SUCCESS) {
            PSG::PrintError("交换链图像视图失败!");
        }
    }

    return true;
}

} // namespace VKB