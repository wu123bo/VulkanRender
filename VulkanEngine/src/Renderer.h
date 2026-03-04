#ifndef RENDERER_H_
#define RENDERER_H_

#include "RHI/IRHI.h"

namespace RHI
{
/**
 * @brief Renderer（与图形 API 无关）
 *
 * Renderer 是“渲染流程的组织者”，而不是 API 封装。
 *
 * 职责：
 * 1️⃣ 持有 IRHI
 * 2️⃣ 组织一帧的渲染流程（BeginFrame / EndFrame）
 * 3️⃣ 调度 CommandList / RenderPass（后续扩展）
 *
 * 非职责（非常重要）：
 * 不 include Vulkan / OpenGL
 * 不创建 VkDevice / GL Context
 * 不关心 SwapChain 细节
 */
class Renderer
{
public:
    Renderer() = default;

    ~Renderer();

    /**
     * @brief 初始化 Renderer
     *
     * 内部会通过 RHIFactory 创建对应的 RHI 后端
     */
    bool Init(const SurfaceDescRHI &surface, int width, int height);

    /**
     * @brief 关闭 Renderer，并释放 RHI
     */
    void Shutdown();

    /**
     * @brief 渲染一帧
     *
     * 这里是“引擎级 Render()”，
     * 后续所有渲染逻辑都会集中在这里或其子模块中。
     */
    void RenderFrame();

private:
    /// 当前使用的 RHI 后端（Vulkan / OpenGL 等）
    IRHI *_rhi;
};

} // namespace RHI

#endif // RENDERER_H_