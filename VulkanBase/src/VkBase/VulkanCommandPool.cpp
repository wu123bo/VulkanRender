#include "VulkanCommandPool.h"

#include "PrintMsg.h"

namespace VKB
{
VulkanCommandPool::VulkanCommandPool()
{
}

VulkanCommandPool::~VulkanCommandPool()
{
    Destroy();
}

bool VulkanCommandPool::Init(VkDevice device, uint32_t queueFamilyIndex)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建帧缓冲对象：逻辑设备为空!");
        return false;
    }

    // 创建命令池信息
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    // 图形命令队列族
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    // 允许单个 CommandBuffer reset
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    // 创建命令池
    VkResult ret =
        vkCreateCommandPool(device, &poolInfo, nullptr, &_commandPool);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建命令池失败!");
    }

    _device = device;
    return true;
}

void VulkanCommandPool::Destroy()
{
    if (_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        _commandPool = VK_NULL_HANDLE;
    }
}

} // namespace VKB