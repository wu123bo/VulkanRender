#ifndef VULKANSHADERMODULE_H_
#define VULKANSHADERMODULE_H_

#include "VulkanHead.h"

#include <string>

namespace VKB
{

/**
 * @brief VulkanShaderModule
 *
 * Vulkan Shader 模块封装：
 * - 从 SPIR-V 文件创建 VkShaderModule
 * - 提供 Pipeline 使用的 ShaderStage 信息
 */

class VulkanShaderModule
{
public:
    VulkanShaderModule();

    ~VulkanShaderModule();

public:
    /**
     * @brief 创建 ShaderModule
     * @param device 逻辑设备
     * @param filename SPIR-V 文件路径
     * @param stage Shader 阶段（vertex / fragment）
     */
    bool Init(VkDevice device, const std::string &filename,
              VkShaderStageFlagBits stage);

    void Destroy();

    /// 获取 ShaderModule 句柄
    VkShaderModule Get() const
    {
        return _shaderModule;
    }

    /// 获取 Pipeline 使用的 ShaderStage 信息
    VkPipelineShaderStageCreateInfo GetStageInfo() const;

private:
    bool loadFile(const std::string &filename);

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkShaderModule _shaderModule = VK_NULL_HANDLE;

    VkShaderStageFlagBits _stage = VK_SHADER_STAGE_VERTEX_BIT;

private:
    std::vector<char> _code; // SPIR-V 二进制
};

} // namespace VKB

#endif // !VULKANSHADERMODULE_H_
