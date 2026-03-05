#include "VulkanDescriptorPool.h"

namespace RHI
{

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    Destroy();
}

bool VulkanDescriptorPool::Init(VkDevice device, uint32_t framesInFlight)
{
    if (device == VK_NULL_HANDLE || framesInFlight == 0) {
        return false;
    }

    _device = device;

    // 预估需要的 descriptor 数量
    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16 * framesInFlight},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16 * framesInFlight},
        // 可以根据需求扩展：storage buffer / sampled image 等
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = framesInFlight * 16; // 预留每帧最多 16 个 set
    poolInfo.flags =
        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // 支持 free

    VkResult ret =
        vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPool);
    if (ret != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool VulkanDescriptorPool::AllocateDescriptorSets(
    VkDescriptorSetLayout layout, std::vector<VkDescriptorSet> &outSets,
    uint32_t count)
{
    if (_descriptorPool == VK_NULL_HANDLE) {
        return false;
    }

    outSets.resize(count);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = &layout;

    VkResult ret =
        vkAllocateDescriptorSets(_device, &allocInfo, outSets.data());
    if (ret != VK_SUCCESS) {
        return false;
    }

    return true;
}

void VulkanDescriptorPool::FreeDescriptorSets(
    const std::vector<VkDescriptorSet> &sets)
{
    if (_descriptorPool != VK_NULL_HANDLE && !sets.empty()) {
        vkFreeDescriptorSets(_device, _descriptorPool,
                             static_cast<uint32_t>(sets.size()), sets.data());
    }
}

void VulkanDescriptorPool::Destroy()
{
    if (_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
        _descriptorPool = VK_NULL_HANDLE;
    }
}

} // namespace RHI
