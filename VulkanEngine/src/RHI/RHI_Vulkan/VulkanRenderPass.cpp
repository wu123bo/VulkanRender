#include "VulkanRenderPass.h"

#include "PrintMsg.h"
#include "VulkanUtils.h"

namespace RHI
{
VulkanRenderPass::~VulkanRenderPass()
{
    Destroy();
}

bool VulkanRenderPass::Init(VkDevice device, VulkanAttachment *attachment)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建渲染通道失败：逻辑设备为空!");
        return false;
    }

    // 创建子通道
    VkSubpassDescription subpass{};
    bool ret = createSubpassDescription(subpass, attachment);
    if (!ret) {
        PSG::PrintError("创建渲染通道失败 缺少基础颜色附件!");
    }

    // ---------- 子通道依赖（同步） ----------
    VkSubpassDependency dependency{};
    createSubpassDependency(dependency);

    // ---------- 附件列表 ----------
    auto attachments = attachment->GetAllAttachments();

    // RenderPass 创建信息
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // 附件数量
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    // 子通道数量
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    // 创建 RenderPass
    VkResult vkret =
        vkCreateRenderPass(device, &renderPassInfo, nullptr, &_renderPass);
    if (vkret != VK_SUCCESS) {
        PSG::PrintError("创建渲染通道失败!");
        return false;
    }

    _device = device;
    return true;
}

void VulkanRenderPass::Destroy()
{
    // 销毁渲染通道
    if (_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(_device, _renderPass, nullptr);
        _renderPass = VK_NULL_HANDLE;
    }
}

bool VulkanRenderPass::createSubpassDescription(VkSubpassDescription &subpass,
                                                VulkanAttachment *attachment)
{
    // 子通道描述
    const auto &attachRefs = attachment->GetAttachmentMap();

    if (attachRefs.find(AttachmentType::COLOR) == attachRefs.end()) {
        return false;
    }

    auto colorRefs = attachment->GetAttachment(AttachmentType::COLOR);

    // 子通道（Subpass）
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRefs->ref;

    if (attachRefs.find(AttachmentType::DEPTH) != attachRefs.end()) {
        auto depthRefs = attachment->GetAttachment(AttachmentType::DEPTH);
        subpass.pDepthStencilAttachment = &depthRefs->ref;
    }

    if (attachRefs.find(AttachmentType::RESOLVE) != attachRefs.end()) {
        auto resolveRefs = attachment->GetAttachment(AttachmentType::RESOLVE);
        subpass.pResolveAttachments = &resolveRefs->ref;
    }

    return true;
}

void VulkanRenderPass::createSubpassDependency(VkSubpassDependency &dependency)
{
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                              | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                              | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                               | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                               | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
}

} // namespace RHI
