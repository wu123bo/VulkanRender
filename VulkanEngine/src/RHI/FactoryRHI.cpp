#include "FactoryRHI.h"

#include <memory>

#include "OpenGLRHI.h"
#include "VulkanRHI.h"

namespace RHI
{

IRHI *CreateRHI(GraphicsAPI api, const SurfaceDescRHI &surface, int width,
                int height)
{
    IRHI *rhi = nullptr;

    // 根据 API 类型选择具体后端
    switch (api) {
        case GraphicsAPI::Vulkan:
            rhi = new (std::nothrow) VulkanRHI();
            break;

        case GraphicsAPI::OpenGL:
            rhi = new (std::nothrow) OpenGLRHI();
            break;

        default:
            return nullptr;
    }

    if (nullptr == rhi) {
        return nullptr;
    }

    // 统一初始化入口
    if (rhi->Init(surface, width, height)) {
        return rhi;
    }

    SDelete(rhi);
    return nullptr;
}

} // namespace RHI
