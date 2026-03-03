#ifndef VULKANRHI_H_
#define VULKANRHI_H_

#include "IRHI.h"

#include "RHI_Vulkan/VulkanContext.h"

namespace RHI
{
class VulkanRHI : public IRHI
{
public:
    VulkanRHI();

    ~VulkanRHI();

    virtual int Init(const SurfaceDescRHI &surfaceDesc, int width,
                     int height) override;

    virtual void Shutdown() override;

    virtual void BeginFrame() override;

    virtual void Render() override;

    virtual void EndFrame() override;

private:
    VulkanContext *_context = nullptr;
};
} // namespace RHI

#endif // !VULKANRHI_H_
