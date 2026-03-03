#ifndef SURFACERHI_H_
#define SURFACERHI_H_

namespace RHI
{

/**
 * @brief 渲染后端类型
 *
 * 用于在 RHI 层选择具体的后端实现：
 * - Vulkan
 * - OpenGL
 * - Direct3D11 / 12
 *
 * ⚠️ 注意：
 * 这里是“选择后端”，不是 Vulkan / DX 的对象
 */
enum GraphicsAPI
{
    Vulkan = 0,
    OpenGL
};

/**
 * @brief Surface 描述（API 无关）
 *
 * 该结构用于向 RHI 提供“窗口系统信息”，
 * 而不是具体图形 API 的 Surface / SwapChain。
 *
 * 设计原则：
 * 1. 只包含平台相关数据
 * 2. 不包含任何 Vk* / ID3D* / GL* 类型
 * 3. 可被所有图形 API 后端解释
 */
struct SurfaceDescRHI
{
    /// 当前使用的图形 API
    GraphicsAPI api;

#if defined(_WIN32)
    /// Win32 窗口句柄（HWND）
    void *hwnd = nullptr;

    /// Win32 实例句柄（HINSTANCE）
    void *hinstance = nullptr;

#elif defined(__linux__)
    /// XCB 连接
    void *xcb_connection = nullptr;

    /// XCB 窗口
    void *xcb_window = nullptr;
#endif
};

} // namespace RHI

#endif // SURFACERHI_H_