#include "VulkanRHI.h"

namespace RHI
{

VulkanRHI::~VulkanRHI()
{
    SDelete(_context);
}

bool VulkanRHI::Init(const SurfaceDescRHI &surfaceDesc, int width, int height)
{
    _context = new (std::nothrow) VulkanContext();
    if (nullptr == _context) {
        return false;
    }

    bool ret = _context->Init(surfaceDesc, width, height);
    if (!ret) {
        return false;
    }

    // 2. 创建 ResourceManager
    _resourceManager = new (std::nothrow) VulkanResourceManager();
    ret = _resourceManager->Init(_context, 2);
    return ret;
}

void VulkanRHI::Shutdown()
{
    if (nullptr != _resourceManager) {
        SDelete(_resourceManager);
    }

    if (nullptr != _context) {
        SDelete(_context);
    }
}

void VulkanRHI::BeginFrame()
{
}

void VulkanRHI::EndFrame()
{
}

} // namespace RHI
