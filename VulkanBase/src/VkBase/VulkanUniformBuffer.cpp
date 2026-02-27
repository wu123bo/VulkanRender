#include "VulkanUniformBuffer.h"

namespace VKB
{

VulkanUniformBuffer::~VulkanUniformBuffer()
{
    Destroy();
}

bool VulkanUniformBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                               VkDeviceSize size)
{
    _size = size;
    return _buffer.Init(physicalDevice, device, size,
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VulkanUniformBuffer::Update(const void *data, VkDeviceSize size)
{
    void *mapped = _buffer.Map();
    memcpy(mapped, data, static_cast<size_t>(size));
    _buffer.Unmap();
}

void VulkanUniformBuffer::Destroy()
{
    _buffer.Destroy();
}

} // namespace VKB
