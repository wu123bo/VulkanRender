#include "VulkanPipelineLayout.h"

#include "PrintMsg.h"

namespace VKB
{
VulkanPipelineLayout::VulkanPipelineLayout()
{
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    Destroy();
}

bool VulkanPipelineLayout::Init(
    VkDevice device, const std::vector<VkDescriptorSetLayout> &setLayouts,
    const std::vector<VkPushConstantRange> &pushConstants)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建管线布局失败：逻辑设备为空!");
        return false;
    }

    // 创建管线布局信息
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // 绑定描述符集布局
    info.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    info.pSetLayouts = setLayouts.empty() ? nullptr : setLayouts.data();
    info.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
    info.pPushConstantRanges =
        pushConstants.empty() ? nullptr : pushConstants.data();

    // 创建管线布局
    VkResult ret =
        vkCreatePipelineLayout(device, &info, nullptr, &_pipelineLayout);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建管线布局失败!");
        return false;
    }

    _device = device;
    return true;
}

void VulkanPipelineLayout::Destroy()
{
    // 销毁管线布局
    if (_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        _pipelineLayout = VK_NULL_HANDLE;
    }
}

} // namespace VKB