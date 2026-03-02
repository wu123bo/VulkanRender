#include "VulkanRenderPass.h"

#include "PrintMsg.h"
#include "VulkanUtils.h"

namespace VKB
{

VulkanRenderPass::VulkanRenderPass()
{
}

VulkanRenderPass::~VulkanRenderPass()
{
    Destroy();
}

bool VulkanRenderPass::Init(VkDevice device,
                            VulkanAttachmentDesc &attachmentDesc)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建渲染通道失败：逻辑设备为空!");
        return false;
    }

    // -------------------------
    // 子通道描述
    // -------------------------
    auto colorRefs = attachmentDesc.GetAttachment(AttachmentType::COLOR);
    auto depthRefs = attachmentDesc.GetAttachment(AttachmentType::DEPTH);
    auto resolveRefs = attachmentDesc.GetAttachment(AttachmentType::RESOLVE);

    if (nullptr == colorRefs) {
        PSG::PrintError("创建渲染通道失败!");
        return false;
    }

    // 子通道（Subpass）
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRefs->ref;

    if (depthRefs != nullptr) {
        subpass.pDepthStencilAttachment = &depthRefs->ref;
    }

    if (resolveRefs != nullptr) {
        subpass.pResolveAttachments = &resolveRefs->ref;
    }

    // ---------- 子通道依赖（同步） ----------
    VkSubpassDependency dependency{};
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


    // ---------- 附件列表 ----------
    auto attachments = attachmentDesc.GetAllAttachments();

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
    VkResult ret =
        vkCreateRenderPass(device, &renderPassInfo, nullptr, &_renderPass);
    if (ret != VK_SUCCESS) {
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

} // namespace VKB
