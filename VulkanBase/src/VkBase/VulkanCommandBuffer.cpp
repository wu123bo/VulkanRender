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
                                 VkFramebuffer framebuffer, VkExtent2D extent,
                                 VkPipeline pipeline)
{
    VkCommandBuffer cmd = _commandBuffers[index];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{1.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    vkCmdDraw(cmd, 3, 1, 0, 0); // gl_VertexIndex 绘制三角形

    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);

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