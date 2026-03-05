#ifndef VULKANRHI_H_
#define VULKANRHI_H_

#include "IRHI.h"

#include "RHI_Vulkan/VulkanContext.h"
#include "RHI_Vulkan/VulkanResourceManager.h"

namespace RHI
{
class VulkanRHI : public IRHI
{
public:
    VulkanRHI() = default;

    ~VulkanRHI();

    virtual bool Init(const SurfaceDescRHI &surfaceDesc, int width,
                      int height) override;

    virtual void Shutdown() override;

    virtual void BeginFrame() override;

    virtual void EndFrame() override;

private:
    // 管理 Vulkan 基础对象
    VulkanContext *_context = nullptr;

    // 管理 Buffers / Textures / DescriptorSet
    VulkanResourceManager *_resourceManager = nullptr;
};
} // namespace RHI

#endif // !VULKANRHI_H_
