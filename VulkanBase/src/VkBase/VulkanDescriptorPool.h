#ifndef VULKANDESCRIPTORPOOL_H_
#define VULKANDESCRIPTORPOOL_H_

#include <vector>
#include <vulkan/vulkan.h>

namespace VKB
{

class VulkanDescriptorPool
{
public:
    VulkanDescriptorPool() = default;

    ~VulkanDescriptorPool();

    bool Init(VkDevice device, uint32_t maxSets,
              const std::vector<VkDescriptorPoolSize> &poolSizes);

    // 分配 descriptor sets
    bool
    AllocateDescriptorSets(const std::vector<VkDescriptorSetLayout> &layouts,
                           std::vector<VkDescriptorSet> &sets);

    VkDescriptorPool Get() const
    {
        return _pool;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;

    VkDescriptorPool _pool = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANDESCRIPTORPOOL_H_