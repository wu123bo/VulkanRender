#include "VulkanCommandBuffer.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanCommandBuffer::VulkanCommandBuffer()
{
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    Destroy();
}

bool VulkanCommandBuffer::Init(VkDevice device, VkCommandPool commandPool,
                               uint32_t count)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("分配命令缓冲区失败：逻辑设备为空!");
        return false;
    }

    _commandBuffers.resize(count);

    // 分配命令缓冲区创建信息
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    // 分配命令缓冲区
    VkResult ret =
        vkAllocateCommandBuffers(device, &allocInfo, _commandBuffers.data());
    if (ret != VK_SUCCESS) {
        PSG::PrintError("分配命令缓冲区失败!");
        return false;
    }

    _device = device;
    _commandPool = commandPool;
    return true;
}

bool VulkanCommandBuffer::Record(uint32_t index, VkRenderPass renderPass,
                                 VkFramebuffer framebuffer, VkExtent2D extent)
{
    // 取出对应索引的命令缓冲区
    VkCommandBuffer cmd = _commandBuffers[index];

    // 开始记录命令缓冲区
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult ret = vkBeginCommandBuffer(cmd, &beginInfo);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("无法开始命令缓冲区!");
        return false;
    }

    // 设置 RenderPass 清除颜色
    VkClearValue clearColor{};
    clearColor.color = {{0.1f, 0.1f, 0.1f, 1.0f}};

    // 渲染通道 开始信息
    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    // RenderPass 开始信息
    rpInfo.renderPass = renderPass;
    // 当前帧对应的 Framebuffer
    rpInfo.framebuffer = framebuffer;
    // 渲染区域左上角
    rpInfo.renderArea.offset = {0, 0};
    // 渲染区域大小（Swapchain Extent）
    rpInfo.renderArea.extent = extent;
    // 清除值数量（与附件数量匹配）
    rpInfo.clearValueCount = 1;
    // 清除颜色
    rpInfo.pClearValues = &clearColor;

    // 开始 渲染通道
    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    // TODO 后续绑定 管线/绘制命令

    // 结束  渲染通道
    vkCmdEndRenderPass(cmd);

    // 结束命令缓冲区记录
    ret = vkEndCommandBuffer(cmd);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("无法记录命令缓冲区!");
        return false;
    }

    return true;
}

void VulkanCommandBuffer::Destroy()
{
    // 销毁命令缓冲区
    if (!_commandBuffers.empty()) {
        vkFreeCommandBuffers(_device, _commandPool,
                             static_cast<uint32_t>(_commandBuffers.size()),
                             _commandBuffers.data());

        _commandBuffers.clear();
    }
}

} // namespace VKB