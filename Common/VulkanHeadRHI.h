#ifndef VULKANHEADOHI
#define VULKANHEADOHI

#define VK_USE_PLATFORM_WIN32_KHR

#include <optional>

#include "MacroHead.h"
#include "vulkan/vulkan.h"

namespace RHI
{

// 队列族索引
struct QueueFamilyIndices
{
    // 支持图形操作的队列族索引
    std::optional<uint32_t> graphicsFamily;

    // 支持呈现操作的队列族索引
    std::optional<uint32_t> presentFamily;

    // 重置
    void Reset()
    {
        graphicsFamily.reset();
        presentFamily.reset();
    }

    // 通用检查
    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// Swapchain 支持信息（由 物理设备 + Surface 决定）
struct SupportDetails
{
    // Surface 能力：图像数量、分辨率范围、变换方式等
    VkSurfaceCapabilitiesKHR capabilities;

    // 支持的 Swapchain 图像格式（格式 + 颜色空间）
    std::vector<VkSurfaceFormatKHR> formats;

    // 支持的呈现模式（FIFO / MAILBOX / IMMEDIATE 等）
    std::vector<VkPresentModeKHR> presentModes;
};

// 附件类型
enum class AttachmentType
{
    COLOR = 0,
    DEPTH,
    RESOLVE
    // 可以以后扩展 HDR、GBuffer 等类型
};

// Vulkan 附件描述结构体
struct Attachment
{
    // 附件描述
    VkAttachmentDescription desc;

    // 附件引用
    VkAttachmentReference ref;
};

// 小型 Push Constant 结构体 （通常不超过 128 字节）
struct PushObject
{
    alignas(16) MAT_4 model;
    alignas(16) PTF_3D color;
};

} // namespace RHI

#endif // !VULKANHEADOHI
