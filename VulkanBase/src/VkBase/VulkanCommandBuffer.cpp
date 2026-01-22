#ifndef VULKANBASE_H_
#define VULKANBASE_H_

#include "MacroHead.h"

namespace VKB
{
class VulkanBase
{
public:
    VulkanBase();

    virtual ~VulkanBase();

    virtual int CreateInstance();

protected:
    // vulkan 实例
    VkInstance _instance = VK_NULL_HANDLE;
};
} // namespace VKB

#endif // !VULKANBASE_H_
