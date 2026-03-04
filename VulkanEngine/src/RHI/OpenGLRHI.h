#ifndef OPENGLRHI_H_
#define OPENGLRHI_H_

#include "IRHI.h"

namespace RHI
{
class OpenGLRHI : public IRHI
{
public:
    OpenGLRHI();

    ~OpenGLRHI();

    virtual bool Init(const SurfaceDescRHI &surfaceDesc, int width,
                      int height) override;

    virtual void Shutdown() override;

    virtual void BeginFrame() override;

    virtual void EndFrame() override;
};
} // namespace RHI

#endif // !OPENGLRHI_H_
