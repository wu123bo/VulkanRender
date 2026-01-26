#include "VulkanPipeline.h"

#include "PrintMsg.h"

namespace VKB
{
VulkanPipeline::VulkanPipeline()
{
}

VulkanPipeline::~VulkanPipeline()
{
    Destroy();
}

bool VulkanPipeline::Init(VkDevice device, VkRenderPass renderPass,
                          VkPipelineLayout layout, VkExtent2D extent,
                          VkShaderModule vertShader, VkShaderModule fragShader)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建图形管线失败：逻辑设备为空!");
        return false;
    }

    // 管线创建信息(顶点)
    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // shader类型
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertShader;
    // 入口函数名
    vertStage.pName = "main";

    // 管线创建信息(片元)
    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragShader;
    fragStage.pName = "main";

    // 定义一个包含这两个结构的数组
    VkPipelineShaderStageCreateInfo stages[] = {vertStage, fragStage};

    // 管线顶点输入
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // 顶点缓冲绑定描述（stride / input rate）
    auto bindingDescription = VerCorTex::getBindingDescription();
    // 顶点属性描述（position / color / texcoord 等）
    auto attributeDescriptions = VerCorTex::getAttributeDescriptions();

    // 顶点绑定数量
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &bindingDescription;

    // 顶点属性数量
    vertexInput.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInput.pVertexAttributeDescriptions = attributeDescriptions.data();

    // 输入汇编
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    // 绘制方式
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // ---- Viewport / Scissor ----
    VkViewport viewport{};
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = extent;

    // 管线创建时指定它们的计数
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // ----光栅化（Rasterization） ----
    VkPipelineRasterizationStateCreateInfo raster{};
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.cullMode = VK_CULL_MODE_BACK_BIT;
    raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    raster.lineWidth = 1.0f;

    // ----  多重采样（MSAA，暂不启用）----
    VkPipelineMultisampleStateCreateInfo msaa{};
    msaa.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msaa.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // ----颜色混合（Color Blend） ----
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments = &colorBlendAttachment;

    // ---- Graphics Pipeline ----
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &raster;
    pipelineInfo.pMultisampleState = &msaa;
    pipelineInfo.pColorBlendState = &colorBlend;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    // 创建图形管线
    VkResult ret = vkCreateGraphicsPipelines(
        device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline);

    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建图形管线失败!");
        return false;
    }

    _device = device;
    return true;
}

void VulkanPipeline::Destroy()
{
    // 销毁图形管线
    if (_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(_device, _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
}

} // namespace VKB
