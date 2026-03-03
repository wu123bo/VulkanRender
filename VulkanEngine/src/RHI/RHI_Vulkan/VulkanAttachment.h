#ifndef VULKANATTACHMENT_H_
#define VULKANATTACHMENT_H_

#include <map>

#include "VulkanHeadRHI.h"

namespace RHI
{

class VulkanAttachment
{
public:
    VulkanAttachment() = default;

    ~VulkanAttachment() = default;

    /**
     * @brief 初始化 Vulkan 附件（Attachment）并存入 _attachmentMap
     *
     * @param format        附件的像素格式，例如 VK_FORMAT_B8G8R8A8_UNORM
     * @param samples       多重采样级别，例如 VK_SAMPLE_COUNT_1_BIT
     * @param loadOp        渲染开始时附件的加载操作（Load）
     * @param storeOp       渲染结束时附件的存储操作（Store）
     * @param initialLayout 附件在渲染开始前的布局
     * @param finalLayout   附件在渲染结束后的布局
     * @param type          附件类型（COLOR / DEPTH / RESOLVE 等）
     */
    void AddAttachment(VkFormat format, VkSampleCountFlagBits samples,
                       VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
                       VkImageLayout initialLayout, VkImageLayout finalLayout,
                       AttachmentType type);

    const std::map<AttachmentType, Attachment> &GetAttachmentMap();

    // =========================
    // 获取附件
    // =========================
    Attachment *GetAttachment(AttachmentType type);

    // =========================
    // 获取所有附件列表（按索引顺序）
    // =========================
    std::vector<VkAttachmentDescription> GetAllAttachments() const;

    // 获取所有引用（按索引顺序）
    std::vector<VkAttachmentReference> GetAllAttachmentRefs(
        AttachmentType typeFilter = AttachmentType::COLOR) const;

private:
    std::map<AttachmentType, Attachment> _attachmentMap;
};

} // namespace RHI

#endif // VULKANATTACHMENT_H_