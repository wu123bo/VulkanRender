#ifndef VULKANSWAPCHAIN_H_
#define VULKANSWAPCHAIN_H_

#include "VulkanHead.h"

namespace VKB
{

/**
 * @brief VulkanSwapchain
 *
 * 负责 Swapchain 及其 Image / ImageView 的创建与销毁
 *
 * 依赖：
 * - VulkanPhysicalDevice
 * - VkDevice
 * - VkSurfaceKHR
 */

class VulkanSwapchain
{
public:
    VulkanSwapchain();

    ~VulkanSwapchain();

public:
    /**
     * @brief 初始化 Swapchain
     */
    bool Init(VkPhysicalDevice physicalDevice, VkDevice device,
              VkSurfaceKHR surface, uint32_t graphicsQueueFamily,
              uint32_t presentQueueFamily, uint32_t width, uint32_t height);

    /**
     * @brief 销毁 Swapchain
     */
    void Destroy();

    /**
     * @brief 获取交换链
     */
    VkSwapchainKHR Get() const
    {
        return _swapchain;
    }

    /**
     * @brief 交换链图像格式
     */
    VkFormat GetFormat() const
    {
        return _format;
    }

    /**
     * @brief 获取交换链分辨率
     */
    VkExtent2D GetExtent() const
    {
        return _extent;
    }

    /**
     * @brief 交换链图像对应的图像视图
     */
    const std::vector<VkImageView> &GetImageViews() const
    {
        return _imageViews;
    }

private:
    /**
     * @brief 查询物理设备 + Surface 对 Swapchain 的支持情况
     *
     * 这是创建 Swapchain 的第一步，Vulkan 要求我们在创建前
     * 必须明确知道以下三类信息：
     *
     * 1. SurfaceCapabilities
     *    - 支持的最小 / 最大 Image 数量
     *    - 支持的分辨率范围
     *    - 变换方式（旋转、翻转）
     *
     * 2. SurfaceFormats
     *    - Swapchain Image 的格式（RGBA / BGRA）
     *    - 颜色空间（SRGB / LINEAR）
     *
     * 3. PresentModes
     *    - 显示模式（FIFO / MAILBOX / IMMEDIATE）
     *
     * 这些信息 **与具体 GPU + 窗口系统强相关**
     */
    SupportDetails querySupport(VkPhysicalDevice device, VkSurfaceKHR surface);

    /**
     * @brief 从支持的 SurfaceFormat 中选择一个最合适的格式
     *
     * Vulkan 通常推荐：
     * - VK_FORMAT_B8G8R8A8_SRGB
     * - VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
     *
     * 这是最常见、也是最安全的窗口显示格式
     *
     * 如果设备不支持，则回退使用第一个可用格式
     */
    VkSurfaceFormatKHR
    chooseFormat(const std::vector<VkSurfaceFormatKHR> &formats);

    /**
     * @brief 选择 Swapchain 的 Present Mode（呈现方式）
     *
     * 各模式含义：
     * - FIFO       ：垂直同步（一定支持）
     * - MAILBOX    ：三缓冲，低延迟（优先）
     * - IMMEDIATE  ：可能撕裂（不推荐）
     */
    VkPresentModeKHR
    choosePresentMode(const std::vector<VkPresentModeKHR> &modes);

    /**
     * @brief 决定 Swapchain Image 的最终分辨率
     *
     * Vulkan 的规则：
     * - 如果 currentExtent != UINT32_MAX
     *   → 说明窗口系统已经固定了分辨率（必须使用）
     *
     * - 否则：
     *   → 使用窗口 framebuffer 大小
     *   → 并 clamp 到设备允许的最小 / 最大范围内
     *
     * 这样可以保证：
     * - 在不同平台（Win / Linux / Wayland）行为一致
     * - Resize 时不会创建非法 Swapchain
     */
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                            uint32_t width, uint32_t height);

    /**
     * @brief 创建交换链图像视图
     */
    bool createImageViews(VkDevice device, VkFormat format);

private:
    // 逻辑设备句柄（用于创建和销毁所有 Vulkan 资源）
    VkDevice _device = VK_NULL_HANDLE;

    // 交换链（负责管理用于呈现到窗口的图像队列）
    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

    // 交换链图像格式（颜色格式，例如 VK_FORMAT_B8G8R8A8_SRGB）
    VkFormat _format{};

    // 交换链分辨率（宽高，通常等于窗口尺寸）
    VkExtent2D _extent{};

    // 交换链中的图像列表（由 Vulkan 创建，不需要手动分配）
    std::vector<VkImage> _images;

    // 与交换链图像一一对应的图像视图（用于作为渲染目标）
    std::vector<VkImageView> _imageViews;
};

} // namespace VKB

#endif // !VULKANSWAPCHAIN_H_
