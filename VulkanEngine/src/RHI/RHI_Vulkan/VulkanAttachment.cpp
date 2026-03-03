#include "VulkanAttachment.h"

namespace RHI
{
void VulkanAttachment::AddAttachment(
    VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
    VkAttachmentStoreOp storeOp, VkImageLayout initialLayout,
    VkImageLayout finalLayout, AttachmentType type)
{
    Attachment attach{};
    // ------------------ 填充 Attachment 描述 ------------------
    attach.desc.format = format;                                 // 像素格式
    attach.desc.samples = samples;                               // 多重采样数量
    attach.desc.loadOp = loadOp;                                 // Load 操作
    attach.desc.storeOp = storeOp;                               // Store 操作
    attach.desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 模板缓冲忽略
    attach.desc.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;      // 模板缓冲忽略
    attach.desc.initialLayout = initialLayout; // 渲染开始前的布局
    attach.desc.finalLayout = finalLayout;     // 渲染结束后的布局

    // ------------------ 填充 Subpass 使用的引用 ------------------
    attach.ref.attachment = static_cast<int>(
        type); // Subpass 引用索引（Attachment 在 RenderPass 中的位置）

    // 根据附件类型选择 Subpass 内使用的布局
    switch (type) {
        case AttachmentType::COLOR:   // 普通颜色附件
        case AttachmentType::RESOLVE: // MSAA Resolve 附件
            attach.ref.layout =
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Subpass
                                                          // 内的最优布局
            break;

        case AttachmentType::DEPTH: // 深度 / 模板附件
            attach.ref.layout =
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // Subpass
                                                                  // 内的深度布局
            break;

        default:                             // 其他附件类型（如自定义）
            attach.ref.layout = finalLayout; // 使用最终布局即可
            break;
    }

    // ------------------ 存入附件映射表 ------------------
    _attachmentMap[type] = attach; // 根据类型存储，方便在 RenderPass 构建时引用
}

const std::map<AttachmentType, Attachment> &VulkanAttachment::GetAttachmentMap()
{
    return _attachmentMap;
}

Attachment *VulkanAttachment::GetAttachment(AttachmentType type)
{
    auto it = _attachmentMap.find(type);
    if (it != _attachmentMap.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<VkAttachmentDescription> VulkanAttachment::GetAllAttachments() const
{
    std::vector<VkAttachmentDescription> attachments;
    for (const auto &pair : _attachmentMap) {
        attachments.push_back(pair.second.desc);
    }
    return attachments;
}

std::vector<VkAttachmentReference>
VulkanAttachment::GetAllAttachmentRefs(AttachmentType typeFilter) const
{
    std::vector<VkAttachmentReference> refs;
    for (const auto &pair : _attachmentMap) {
        if (pair.first == typeFilter) {
            refs.push_back(pair.second.ref);
        }
    }
    return refs;
}
} // namespace RHI
