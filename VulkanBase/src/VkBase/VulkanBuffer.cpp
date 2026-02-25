#include "VulkanBuffer.h"
#include <cstring>
#include <iostream>

namespace VKB
{
VulkanBuffer::VulkanBuffer()
{
}

VulkanBuffer::~VulkanBuffer()
{
    Destroy();
}

bool VulkanBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                        VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties)
{
    m_physicalDevice = physicalDevice;
    m_device = device;
    m_size = size;

    // 1. 创建 Buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        std::cerr << "[VulkanBuffer] Create buffer failed\n";
        return false;
    }

    // 2. 查询内存需求
    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(device, m_buffer, &memReq);

    // 3. 分配内存
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(memReq.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory)
        != VK_SUCCESS) {
        std::cerr << "[VulkanBuffer] Allocate memory failed\n";
        return false;
    }

    // 4. 绑定内存
    vkBindBufferMemory(device, m_buffer, m_memory, 0);

    return true;
}

void VulkanBuffer::Destroy()
{
    if (m_mapped) {
        vkUnmapMemory(m_device, m_memory);
        m_mapped = nullptr;
    }

    if (m_buffer) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
    }

    if (m_memory) {
        vkFreeMemory(m_device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

    m_device = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
    m_size = 0;
}

void *VulkanBuffer::Map()
{
    if (!m_mapped) {
        vkMapMemory(m_device, m_memory, 0, m_size, 0, &m_mapped);
    }
    return m_mapped;
}

void VulkanBuffer::Unmap()
{
    if (m_mapped) {
        vkUnmapMemory(m_device, m_memory);
        m_mapped = nullptr;
    }
}

uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter,
                                      VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProps{};
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i))
            && (memProps.memoryTypes[i].propertyFlags & properties)
                   == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
}

} // namespace VKB