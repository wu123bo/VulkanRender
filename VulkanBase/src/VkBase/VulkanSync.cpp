#include "VulkanSync.h"

#include "PrintMsg.h"

namespace VKB
{
VulkanSync::VulkanSync()
{
}

VulkanSync::~VulkanSync()
{
    Destroy();
}

bool VulkanSync::Init(VkDevice device, uint32_t maxFrames)
{
    if (VK_NULL_HANDLE == device) {
        PSG::PrintError("创建同步对象失败：逻辑设备为空!");
        return false;
    }

    // 为每个并行帧创建一组同步对象
    _imageAvailable.resize(maxFrames);
    _renderFinished.resize(maxFrames);
    _inFlightFences.resize(maxFrames);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Fence 初始为 signaled，避免第一帧 CPU 等待
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < maxFrames; ++i) {

        // ImageAvailable + RenderFinished + InFlightFence
        VkResult ret = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                                         &_imageAvailable[i]);
        if (ret != VK_SUCCESS) {
            PSG::PrintError("创建同步对象失败!");
            return false;
        }

        ret = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                                &_renderFinished[i]);
        if (ret != VK_SUCCESS) {
            PSG::PrintError("创建同步对象失败!");
            return false;
        }

        ret = vkCreateFence(device, &fenceInfo, nullptr, &_inFlightFences[i]);
        if (ret != VK_SUCCESS) {
            PSG::PrintError("创建同步对象失败!");
            return false;
        }
    }

    _device = device;
    _maxFrames = maxFrames;
    return true;
}

void VulkanSync::Destroy()
{
    if (_device == VK_NULL_HANDLE) {
        return;
    }

    // 销毁所有同步对象
    for (uint32_t i = 0; i < _maxFrames; ++i) {
        if (_imageAvailable[i]) {
            vkDestroySemaphore(_device, _imageAvailable[i], nullptr);
        }

        if (_renderFinished[i]) {
            vkDestroySemaphore(_device, _renderFinished[i], nullptr);
        }

        if (_inFlightFences[i]) {
            vkDestroyFence(_device, _inFlightFences[i], nullptr);
        }
    }

    _maxFrames = 0;
    _imageAvailable.clear();
    _renderFinished.clear();
    _inFlightFences.clear();
}

VkSemaphore VulkanSync::GetImageAvailable(uint32_t frame) const
{
    return _imageAvailable[frame];
}

VkSemaphore VulkanSync::GetRenderFinished(uint32_t frame) const
{
    return _renderFinished[frame];
}

VkFence VulkanSync::GetInFlightFence(uint32_t frame) const
{
    return _inFlightFences[frame];
}

} // namespace VKB
