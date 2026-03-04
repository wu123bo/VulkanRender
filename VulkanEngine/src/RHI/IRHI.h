#ifndef IRHI_H_
#define IRHI_H_

#include "SurfaceRHI.h"

namespace RHI
{

class IRHI
{
public:
    IRHI() = default;

    virtual ~IRHI() = default;

    virtual bool Init(const SurfaceDescRHI &surfaceDesc, int width,
                      int height) = 0;

    virtual void Shutdown() = 0;

    virtual void BeginFrame() = 0;

    virtual void EndFrame() = 0;
};

} // namespace RHI

#endif // !IRHI_H_
