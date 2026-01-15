#ifndef MACROHEAD_H_
#define MACROHEAD_H_

#include <array>

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

#ifndef SDelete
#define SDelete(ptr)                                                           \
    do {                                                                       \
        if (nullptr != ptr) {                                                  \
            delete ptr;                                                        \
            ptr = nullptr;                                                     \
        }                                                                      \
    } while (0)
#endif

#ifndef SDeleteArray
#define SDeleteArray(ptr)                                                      \
    do {                                                                       \
        if (nullptr != ptr) {                                                  \
            delete[] ptr;                                                      \
            ptr = nullptr;                                                     \
        }                                                                      \
    } while (0)
#endif

typedef glm::vec2 PTF_2D;
typedef glm::vec3 PTF_3D;
typedef glm::vec4 PTF_4D;

typedef glm::ivec2 PTI_2D;
typedef glm::ivec3 PTI_3D;
typedef glm::ivec4 PTI_4D;

typedef glm::mat2 MAT_2;
typedef glm::mat3 MAT_3;
typedef glm::mat4 MAT_4;

// 顶点坐标颜色结构体
struct VERTEX
{
    PTF_2D pos;
    PTF_3D color;

    // 绑定描述
    //  顶点绑定描述了在整个顶点中从内存加载数据的速率
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        // 指定绑定数组中绑定的索引
        bindingDescription.binding = 0;
        // 指定从一个条目到下一个条目的字节数
        bindingDescription.stride = sizeof(VERTEX);
        // 在每个顶点之后移动到下一个数据条目
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    // 属性描述
    // 属性描述结构描述了如何从来自绑定描述的顶点数据块中提取顶点属性
    static std::array<VkVertexInputAttributeDescription, 2>
    getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2>
            attributeDescriptions{};
        // 告诉 Vulkan 每个顶点的数据来自哪个绑定
        attributeDescriptions[0].binding = 0;
        // 引用顶点着色器中输入的 location 指令
        attributeDescriptions[0].location = 0;
        // 描述属性的数据类型
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        // 定自每个顶点数据的开始读取的字节数
        attributeDescriptions[0].offset = offsetof(VERTEX, pos);

        // format例如
        // float：VK_FORMAT_R32_SFLOAT
        // vec2：VK_FORMAT_R32G32_SFLOAT

        // 颜色属性相同设置
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VERTEX, color);

        return attributeDescriptions;
    }
};

#endif // !MACROHEAD_H_
