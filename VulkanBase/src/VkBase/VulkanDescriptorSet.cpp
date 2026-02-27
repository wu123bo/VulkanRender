#include "VulkanDescriptorSet.h"

namespace VKB
{
VulkanDescriptorSet::~VulkanDescriptorSet()
{
    // DescriptorSet 本身不销毁 Pool 的资源
}

void VulkanDescriptorSet::Init(VkDevice device, VkDescriptorSet set)
{
    _device = device;
    _set = set;
}

void VulkanDescriptorSet::UpdateBuffer(uint32_t binding,const VkBuffer &buffer,uint32_t bufferSize)
{
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = bufferSize;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = _set;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
}


} // namespace VKB
