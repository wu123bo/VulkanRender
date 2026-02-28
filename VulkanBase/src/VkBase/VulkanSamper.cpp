#include "VulkanSamper.h"

#include "PrintMsg.h"

namespace VKB
{
VulkanSampler::~VulkanSampler()
{
    if (_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(_device, _sampler, nullptr);
        _sampler = VK_NULL_HANDLE; // 避免重复销毁
    }
}

bool VulkanSampler::Init(VkPhysicalDevice physicalDevice, VkDevice device,
                         VkFilter filter, VkSamplerAddressMode addrMode)
{
    _device = device;

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    // VK_FILTER_LINEAR 线性过滤（默认）
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;

    // VK_SAMPLER_ADDRESS_MODE_REPEAT 重复（默认）
    samplerInfo.addressModeU = addrMode;
    samplerInfo.addressModeV = addrMode;
    samplerInfo.addressModeW = addrMode;

    samplerInfo.anisotropyEnable = VK_TRUE;

    // 查询物理设备基本属性
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    samplerInfo.minLod = 0.0f;
    // 强制采样器不使用最低的 mip 层级
    //  samplerInfo.minLod = static_cast<float>(_mipLevels / 2);
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    samplerInfo.mipLodBias = 0.0f;

    VkResult ret = vkCreateSampler(device, &samplerInfo, nullptr, &_sampler);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建纹理采样器失败!");
        return false;
    }

    return true;
}
} // namespace VKB