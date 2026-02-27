#include "VulkanDescriptorSetLayout.h"

namespace VKB
{

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    if (_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
    }
}

VkDescriptorSetLayoutBinding
VulkanDescriptorSetLayout::Make(uint32_t binding, VkDescriptorType type,
                                VkShaderStageFlags stageFlags, uint32_t count)
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    // 对应 shader 中的 layout(binding = X)
    layoutBinding.binding = binding;

    // 描述符资源类型
    layoutBinding.descriptorType = type;

    // 绑定的描述符数量（非数组一般为 1）
    layoutBinding.descriptorCount = count;

    // 指定哪些 shader 阶段可以访问该资源
    layoutBinding.stageFlags = stageFlags;

    // 不使用不可变采样器（动态 sampler）
    layoutBinding.pImmutableSamplers = nullptr;

    return layoutBinding;
}

bool VulkanDescriptorSetLayout::Init(
    VkDevice device, const std::vector<VkDescriptorSetLayoutBinding> &bindings)
{
    _device = device;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult ret =
        vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &_layout);
    return ret == VK_SUCCESS;
}

} // namespace VKB
