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
};
} // namespace VKB

#endif // !VULKANBASE_H_
