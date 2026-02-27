#include "VulkanDescriptorPool.h"

namespace VKB
{

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    if (_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(_device, _pool, nullptr);
    }
}

bool VulkanDescriptorPool::Init(
    VkDevice device, uint32_t maxSets,
    const std::vector<VkDescriptorPoolSize> &poolSizes)
{
    _device = device;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;

    VkResult ret = vkCreateDescriptorPool(device, &poolInfo, nullptr, &_pool);
    return ret == VK_SUCCESS;
}

bool VulkanDescriptorPool::AllocateDescriptorSets(
    const std::vector<VkDescriptorSetLayout> &layouts,
    std::vector<VkDescriptorSet> &sets)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    sets.resize(layouts.size());
    VkResult ret = vkAllocateDescriptorSets(_device, &allocInfo, sets.data());
    return ret == VK_SUCCESS;
}

} // namespace VKB
