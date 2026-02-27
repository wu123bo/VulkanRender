#ifndef VULKANATTACHMENTDESC_H_
#define VULKANATTACHMENTDESC_H_

#include <map>

#include "VulkanHead.h"

namespace VKB
{

class VulkanAttachmentDesc
{
public:
    VulkanAttachmentDesc() = default;

    ~VulkanAttachmentDesc() = default;

    // =========================
    // 添加附件
    // =========================
    void AddAttachment(VkFormat format, VkSampleCountFlagBits samples,
                       VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
                       VkImageLayout initialLayout, VkImageLayout finalLayout,
                       AttachmentType type);

    // =========================
    // 获取附件
    // =========================
    AttachmentDesc *GetAttachment(AttachmentType type);

    // =========================
    // 获取所有附件列表（按索引顺序）
    // =========================
    std::vector<VkAttachmentDescription> GetAllAttachments() const;

    // 获取所有引用（按索引顺序）
    std::vector<VkAttachmentReference> GetAllAttachmentRefs(
        AttachmentType typeFilter = AttachmentType::COLOR) const;

private:
    std::map<AttachmentType, AttachmentDesc> _attachmentMap;
};

} // namespace VKB

#endif // VULKANATTACHMENTDESC_H_