#include "VulkanRHI.h"

namespace RHI
{
VulkanRHI::VulkanRHI()
{
    _context = new VulkanContext();
}

VulkanRHI::~VulkanRHI()
{
    SDelete(_context);
}

int VulkanRHI::Init(const SurfaceDescRHI &surfaceDesc, int width, int height)
{
    if (nullptr == _context) {
        return 1;
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

void VulkanRHI::Render()
{
}

void VulkanRHI::EndFrame()
{
}

} // namespace RHI
