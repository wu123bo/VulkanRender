#include "VulkanAttachmentDesc.h"

namespace VKB
{
void VulkanAttachmentDesc::AddAttachment(
    VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
    VkAttachmentStoreOp storeOp, VkImageLayout initialLayout,
    VkImageLayout finalLayout, AttachmentType type)
{
    AttachmentDesc attach{};
    attach.desc.format = format;
    attach.desc.samples = samples;
    attach.desc.loadOp = loadOp;
    attach.desc.storeOp = storeOp;
    attach.desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attach.desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attach.desc.initialLayout = initialLayout;
    attach.desc.finalLayout = finalLayout;

    attach.ref.attachment = (int)type;
    attach.ref.layout = finalLayout;

    // ✅ 这里根据类型选择 Subpass 内使用的布局
    if (type == AttachmentType::COLOR) {
        attach.ref.layout =
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Subpass 内必须用
                                                      // COLOR_ATTACHMENT_OPTIMAL
    } else if (type == AttachmentType::DEPTH) {
        attach.ref.layout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // Depth
                                                              // 可以直接用
                                                              // finalLayout
    }

    _attachmentMap[type] = attach;
}

AttachmentDesc *VulkanAttachmentDesc::GetAttachment(AttachmentType type)
{
    auto it = _attachmentMap.find(type);
    if (it != _attachmentMap.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<VkAttachmentDescription>
VulkanAttachmentDesc::GetAllAttachments() const
{
    std::vector<VkAttachmentDescription> attachments;
    for (const auto &pair : _attachmentMap) {
        attachments.push_back(pair.second.desc);
    }
    return attachments;
}

std::vector<VkAttachmentReference>
VulkanAttachmentDesc::GetAllAttachmentRefs(AttachmentType typeFilter) const
{
    std::vector<VkAttachmentReference> refs;
    for (const auto &pair : _attachmentMap) {
        if (pair.first == typeFilter) {
            refs.push_back(pair.second.ref);
        }
    }
    return refs;
}
} // namespace VKB
