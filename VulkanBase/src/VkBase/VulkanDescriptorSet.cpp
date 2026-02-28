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

void VulkanDescriptorSet::UpdateBuffer(uint32_t binding, const VkBuffer &buffer,
                                       uint32_t bufferSize)
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

void VulkanDescriptorSet::UpdateBuffer(uint32_t binding,
                                       VkImageView textureImageView,
                                       VkSampler textureSampler)
{
    // 图像信息
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = textureSampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

    // 目标描述符集
    descriptorWrite.dstSet = _set;

    // 对应 shader 中的 binding = X
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;

    // 描述符类型：组合图像采样器
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;

    // 指向纹理采样器信息
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
}

} // namespace VKB
