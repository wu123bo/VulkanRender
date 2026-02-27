#include "VulkanBuffer.h"
#include "PrintMsg.h"

#include "VulkanUtils.h"

namespace VKB
{

VulkanBuffer::~VulkanBuffer()
{
    Destroy();
}

bool VulkanBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                        VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties)
{
    _physicalDevice = physicalDevice;
    _device = device;
    _size = size;

    // =========================
    // 创建 VkBuffer
    // =========================
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &_buffer) != VK_SUCCESS) {
        PSG::PrintError("创建 VkBuffer 失败");
        return false;
    }

    // =========================
    // 查询内存需求
    // =========================
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, _buffer, &memRequirements);

    // =========================
    // 分配内存
    // =========================
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        _physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &_memory) != VK_SUCCESS) {
        PSG::PrintError("分配 Buffer 内存失败");
        return false;
    }

    // =========================
    // 绑定 Buffer 与内存
    // =========================
    vkBindBufferMemory(device, _buffer, _memory, 0);

    return true;
}

void VulkanBuffer::Destroy()
{
    if (_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(_device, _buffer, nullptr);
        _buffer = VK_NULL_HANDLE;
    }

    if (_memory != VK_NULL_HANDLE) {
        vkFreeMemory(_device, _memory, nullptr);
        _memory = VK_NULL_HANDLE;
    }
}

void *VulkanBuffer::Map()
{
    void *data = nullptr;
    vkMapMemory(_device, _memory, 0, _size, 0, &data);
    return data;
}

void VulkanBuffer::Unmap()
{
    vkUnmapMemory(_device, _memory);
}

void VulkanBuffer::CopyFrom(VulkanBuffer &src, VkCommandPool commandPool,
                            VkQueue queue)
{
    // =========================
    // 分配临时 CommandBuffer
    // =========================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    // =========================
    // 录制拷贝命令
    // =========================
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = src.GetSize();
    vkCmdCopyBuffer(commandBuffer, src.Get(), _buffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    // =========================
    // 提交并等待完成
    // =========================
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    // =========================
    // 释放 CommandBuffer
    // =========================
    vkFreeCommandBuffers(_device, commandPool, 1, &commandBuffer);
}

} // namespace VKB