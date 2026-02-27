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
                                 VkPipeline pipeline, VkBuffer vertexBuffer,
                                 uint32_t vertexCount)
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

    VkClearValue clearColor = {
        {{_backColor.x, _backColor.y, _backColor.z, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // 为此管线指定了视口和剪刀状态为动态 发出绘制命令之前在命令缓冲区中设置它们
    VkViewport viewport{};
    viewport.x = 0.0f; // 左上角 X 坐标
    viewport.y = 0.0f; // 左上角 Y 坐标
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f; // 深度最小值
    viewport.maxDepth = 1.0f; // 深度最大值

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0}; // 剪裁矩形左上角
    scissor.extent = extent; // 宽高与交换链一致
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // =========================
    // 绑定顶点缓冲
    // =========================
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);

    vkCmdDraw(cmd, vertexCount, 1, 0, 0);

    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    return true;
}

bool VulkanCommandBuffer::Record(uint32_t index, VkRenderPass renderPass,
                                 VkFramebuffer framebuffer, VkExtent2D extent,
                                 VkPipeline pipeline,
                                 VkPipelineLayout pipelineLayout,
                                 std::vector<VkDescriptorSet> &descriptorSets,
                                 VkBuffer vertexBuffer, VkBuffer indexBuffer,
                                 uint32_t indexCount)
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

    VkClearValue clearColor = {
        {{_backColor.x, _backColor.y, _backColor.z, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // 为此管线指定了视口和剪刀状态为动态 发出绘制命令之前在命令缓冲区中设置它们
    VkViewport viewport{};
    viewport.x = 0.0f; // 左上角 X 坐标
    viewport.y = 0.0f; // 左上角 Y 坐标
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f; // 深度最小值
    viewport.maxDepth = 1.0f; // 深度最大值

    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0}; // 剪裁矩形左上角
    scissor.extent = extent; // 宽高与交换链一致
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // =========================
    // 绑定顶点缓冲
    // =========================
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);

    // 绑定索引缓冲区
    vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // 使用描述符集
    // 与顶点缓冲区和索引缓冲区不同，描述符集对于图形管线不是唯一的。
    // 因此，我们需要指定是否要将描述符集绑定到图形管线或计算管线。
    // 然后是开始索引 个数 绑定的描述符集合
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, &descriptorSets[index], 0,
                            nullptr);

    // 索引绘制
    vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);

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