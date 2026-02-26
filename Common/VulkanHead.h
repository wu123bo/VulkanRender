#ifndef VULKANHEAD_H_
#define VULKANHEAD_H_

#include <array>
#include <optional>
#include <unordered_map>

#include "MacroHead.h"
#include "vulkan/vulkan.h"

#include "GLFW/glfw3.h"

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

struct UniformVec
{
    // uniform 缓冲区
    std::vector<VkBuffer> buffers;

    // uniform 缓冲区内存
    std::vector<VkDeviceMemory> buffersMemory;

    // uniform 缓冲区内存映射
    std::vector<void *> buffersMapped;
};

// 矩阵信息结构体
struct MvpMatrix
{
    // alignas(16) 字节对齐 必须是16的倍数
    alignas(16) MAT_4 model;
    alignas(16) MAT_4 view;
    alignas(16) MAT_4 proj;
};

// 颜色透明度
struct AlphaColor
{
    // alignas(16) 字节对齐 必须是16的倍数
    alignas(16) float alpha;
    alignas(16) PTF_3D color;
};

// 顶点坐标颜色结构体
struct VerCor
{
    PTF_3D pos;
    PTF_3D color;

    // 绑定描述
    //  顶点绑定描述了在整个顶点中从内存加载数据的速率
    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        // 指定绑定数组中绑定的索引
        bindingDescription.binding = 0;
        // 指定从一个条目到下一个条目的字节数
        bindingDescription.stride = sizeof(VerCor);
        // 在每个顶点之后移动到下一个数据条目
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    // 属性描述
    // 属性描述结构描述了如何从来自绑定描述的顶点数据块中提取顶点属性
    static std::array<VkVertexInputAttributeDescription, 2>
    GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2>
            attributeDescriptions{};
        // 告诉 Vulkan 每个顶点的数据来自哪个绑定
        attributeDescriptions[0].binding = 0;
        // 引用顶点着色器中输入的 location 指令
        attributeDescriptions[0].location = 0;
        // 描述属性的数据类型
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        // 定自每个顶点数据的开始读取的字节数
        attributeDescriptions[0].offset = offsetof(VerCor, pos);

        // format例如
        // float：VK_FORMAT_R32_SFLOAT
        // vec2：VK_FORMAT_R32G32_SFLOAT

        // 颜色属性相同设置
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VerCor, color);

        return attributeDescriptions;
    }
};

// 顶点坐标颜色结构体
struct VerCorTex
{
    PTF_3D pos;
    PTF_3D color;
    PTF_2D texCoord;

    // 绑定描述
    //  顶点绑定描述了在整个顶点中从内存加载数据的速率
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        // 指定绑定数组中绑定的索引
        bindingDescription.binding = 0;
        // 指定从一个条目到下一个条目的字节数
        bindingDescription.stride = sizeof(VerCorTex);
        // 在每个顶点之后移动到下一个数据条目
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    // 属性描述
    // 属性描述结构描述了如何从来自绑定描述的顶点数据块中提取顶点属性
    static std::array<VkVertexInputAttributeDescription, 3>
    getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3>
            attributeDescriptions{};
        // 告诉 Vulkan 每个顶点的数据来自哪个绑定
        attributeDescriptions[0].binding = 0;
        // 引用顶点着色器中输入的 location 指令
        attributeDescriptions[0].location = 0;
        // 描述属性的数据类型
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        // 定自每个顶点数据的开始读取的字节数
        attributeDescriptions[0].offset = offsetof(VerCorTex, pos);

        // format例如
        // float：VK_FORMAT_R32_SFLOAT
        // vec2：VK_FORMAT_R32G32_SFLOAT

        // 颜色属性相同设置
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VerCorTex, color);

        // 颜色属性相同设置
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VerCorTex, texCoord);

        return attributeDescriptions;
    }

    // 用于 unordered_map 的相等运算符
    bool operator==(const VerCorTex &other) const
    {
        return pos == other.pos && color == other.color
               && texCoord == other.texCoord;
    }
};

// 特化 hash 函数 用于 unordered_map
namespace std
{
template<>
struct hash<VerCorTex>
{
    size_t operator()(VerCorTex const &vertex) const
    {
        return ((hash<glm::vec3>()(vertex.pos)
                 ^ (hash<glm::vec3>()(vertex.color) << 1))
                >> 1)
               ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
} // namespace std

#endif // !VULKANHEAD_H_
