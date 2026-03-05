#include "VulkanUniformBuffer.h"

namespace RHI
{
VulkanUniformBuffer::VulkanUniformBuffer(VkPhysicalDevice physicalDevice,
                                         VkDevice device, VkDeviceSize size,
                                         uint32_t binding)
{
    Init(physicalDevice, device, size, binding);
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
    Destroy();
}

bool VulkanUniformBuffer::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                               VkDeviceSize size, uint32_t binding)
{
    _size = size;
    _binding = binding;
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

} // namespace RHI
