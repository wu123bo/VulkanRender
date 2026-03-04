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
    return ret;
}

void VulkanRHI::Shutdown()
{
}

void VulkanRHI::BeginFrame()
{
}

void VulkanRHI::EndFrame()
{
}

} // namespace RHI
