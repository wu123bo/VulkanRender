#include "Renderer.h"
#include "RHI/FactoryRHI.h"

#include "MacroHead.h"

namespace RHI
{

Renderer::~Renderer()
{
    Shutdown();
}

bool Renderer::Init(const SurfaceDescRHI &surface, int width, int height)
{
    // Renderer 不直接创建具体 RHI，而是通过工厂
    _rhi = CreateRHI(surface.api, surface, width, height);
    return _rhi != nullptr;
}

void Renderer::Shutdown()
{
    if (nullptr != _rhi) {
        _rhi->Shutdown();
        SDelete(_rhi);
    }
}

void Renderer::RenderFrame()
{
    if (nullptr == _rhi) {
        return;
    }

    _rhi->BeginFrame(); 

    _rhi->EndFrame(); 
}

} // namespace RHI