#ifndef FACTORYRHI_H_
#define FACTORYRHI_H_

#include "IRHI.h"

namespace RHI
{

/**
 * @brief 创建并初始化 RHI 实例（工厂函数）
 *
 * ⚠️ 这是引擎中：
 * - 唯一允许 include VulkanRHI / OpenGLRHI 的地方
 * - Renderer 不应该知道任何具体后端类型
 *
 * @param api     使用的图形 API
 * @param surface 平台窗口描述（API 无关）
 * @param width   渲染宽度
 * @param height  渲染高度
 *
 * @return 成功返回 IRHI 实例，失败返回 nullptr
 */
IRHI *CreateRHI(GraphicsAPI api, const SurfaceDescRHI &surface, int width,
                int height);

} // namespace RHI

#endif // !FACTORYRHI_H_