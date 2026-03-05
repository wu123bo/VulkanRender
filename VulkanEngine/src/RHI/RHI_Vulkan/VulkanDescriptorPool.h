#ifndef VULKAN_DESCRIPTOR_POOL_H_
#define VULKAN_DESCRIPTOR_POOL_H_

#include "VulkanHeadRHI.h"
#include <vector>

namespace RHI
{
/*
 * VulkanDescriptorPool
 * --------------------------------
 * 职责：
 *  * 创建 VkDescriptorPool
 *  * 支持多帧 in-flight 分配 DescriptorSet
 *  * 提供 Allocate / Free 接口
 *
 * 不负责：
 *  * 定义 DescriptorSetLayout（交给 VulkanDescriptorSetLayout）
 *  * 更新 DescriptorSet（交给 ResourceManager 或 Renderer）
 */
class VulkanDescriptorPool
{
public:
    VulkanDescriptorPool() = default;

    ~VulkanDescriptorPool();

    /*
     * 初始化 DescriptorPool
     *
     * @param Vulkan device
     * @param framesInFlight: 每帧需要多少 DescriptorSet
     */
    bool Init(VkDevice device, uint32_t framesInFlight);

    /*
     * 分配 DescriptorSet
     */
    bool AllocateDescriptorSets(VkDescriptorSetLayout layout,
                                std::vector<VkDescriptorSet> &outSets,
                                uint32_t count);

    /*
     * 释放 DescriptorSet（可选，依赖 FREE_DESCRIPTOR_SET_BIT）
     */
    void FreeDescriptorSets(const std::vector<VkDescriptorSet> &sets);

    /*
     * 销毁 DescriptorPool
     */
    void Destroy();

    VkDescriptorPool Get() const
    {
        return _descriptorPool;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
};

} // namespace RHI

#endif // VULKAN_DESCRIPTOR_POOL_H_
