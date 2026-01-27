#include "VulkanShaderModule.h"

#include <fstream>
#include <iostream>

#include "PrintMsg.h"

namespace VKB
{

VulkanShaderModule::VulkanShaderModule()
{
}

VulkanShaderModule::~VulkanShaderModule()
{
    Destroy();
}

bool VulkanShaderModule::Init(VkDevice device, const std::string &filename,
                              VkShaderStageFlagBits stage)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建着色器失败：逻辑设备为空!");
        return false;
    }

    if (!loadFile(filename)) {
        return false;
    }

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = _code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(_code.data());
    VkResult ret =
        vkCreateShaderModule(device, &createInfo, nullptr, &_shaderModule);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建着色器模块失败!");
        return false;
    }

    _stage = stage;
    _device = device;
    return true;
}

void VulkanShaderModule::Destroy()
{
    // 销毁着色器模块
    if (_shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_device, _shaderModule, nullptr);
        _shaderModule = VK_NULL_HANDLE;
    }

    _code.clear();
}

VkPipelineShaderStageCreateInfo VulkanShaderModule::GetStageInfo() const
{
    VkPipelineShaderStageCreateInfo stageInfo{};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = _stage;
    stageInfo.module = _shaderModule;
    stageInfo.pName = "main"; // Shader 入口函数

    return stageInfo;
}

bool VulkanShaderModule::loadFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        PSG::PrintError("着色器文件打开失败!");
        return false;
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    _code.resize(fileSize);

    file.seekg(0);
    file.read(_code.data(), fileSize);
    file.close();

    return true;
}

} // namespace VKB
