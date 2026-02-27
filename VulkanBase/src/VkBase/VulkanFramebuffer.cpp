#include "VulkanFramebuffer.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanFramebuffer::VulkanFramebuffer()
{
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    Destroy();
}

bool VulkanFramebuffer::Init(VkDevice device, VkRenderPass renderPass,
                             const std::vector<VkImageView> &colorImageViews,
                             const VkImageView &depthImageView,
                             VkExtent2D extent)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建帧缓冲对象：逻辑设备为空!");
        return false;
    }

    _framebuffers.resize(colorImageViews.size());
    for (size_t i = 0; i < colorImageViews.size(); i++) {

        // 颜色资源视图 0 colorImageViews  1  depthImageView
        std::vector<VkImageView> attachments{colorImageViews[i],
                                             depthImageView};

        // 创建帧缓冲区信息
        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = renderPass;
        fbInfo.attachmentCount = attachments.size();
        fbInfo.pAttachments = attachments.data();
        fbInfo.width = extent.width;
        fbInfo.height = extent.height;
        fbInfo.layers = 1;

        // 创建帧缓冲区
        VkResult ret =
            vkCreateFramebuffer(device, &fbInfo, nullptr, &_framebuffers[i]);
        if (ret != VK_SUCCESS) {
            PSG::PrintError("创建帧缓冲区失败：逻辑设备为空!");
            return false;
        }
    }

    _device = device;
    return true;
}

void VulkanFramebuffer::Destroy()
{
    // 销毁帧缓冲区
    for (auto fb : _framebuffers) {
        vkDestroyFramebuffer(_device, fb, nullptr);
    }

    _framebuffers.clear();
}

} // namespace VKB
