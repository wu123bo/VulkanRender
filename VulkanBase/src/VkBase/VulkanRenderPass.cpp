#include "VulkanRenderPass.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanRenderPass::VulkanRenderPass()
{
}

VulkanRenderPass::~VulkanRenderPass()
{
    Destroy();
}

bool VulkanRenderPass::Init(VkDevice device, const VulkanSwapchain *swapchain)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建渲染通道失败：逻辑设备为空!");
        return false;
    }

    // 定义颜色附件（Color Attachment）
    VkAttachmentDescription colorAttachment = createAttachment(
        swapchain->GetFormat(), VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // 附件引用（Attachment Reference）
    VkAttachmentReference colorAttachmentRef =
        makeAttachmentRef(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // 子通道（Subpass）
    VkSubpassDescription subpass{};

    // 这是一个图形渲染子通道
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // 使用一个颜色附件
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // RenderPass 创建信息
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // 附件数量
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    // 子通道数量
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

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

VkAttachmentDescription VulkanRenderPass::createAttachment(
    VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
    VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
    VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
    VkImageLayout finalLayout)
{
    VkAttachmentDescription desc{};

    // 附件格式
    desc.format = format;

    // MSAA 采样数
    desc.samples = samples;

    // 颜色 / 深度加载策略
    desc.loadOp = loadOp;

    // 渲染结束后的存储策略
    desc.storeOp = storeOp;

    // 模板加载策略
    desc.stencilLoadOp = stencilLoadOp;

    // 模板存储策略
    desc.stencilStoreOp = stencilStoreOp;

    // 初始布局
    desc.initialLayout = initialLayout;

    // 最终布局
    desc.finalLayout = finalLayout;
    return desc;
}

VkAttachmentReference
VulkanRenderPass::makeAttachmentRef(uint32_t attachmentIndex,
                                    VkImageLayout layout)
{
    VkAttachmentReference ref{};

    // 引用附件ID
    ref.attachment = attachmentIndex;

    // Subpass 中使用的图像布局
    ref.layout = layout;

    return ref;
}

} // namespace VKB
