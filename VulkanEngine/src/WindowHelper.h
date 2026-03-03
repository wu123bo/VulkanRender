#include "RHI/SurfaceRHI.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

inline int CreateWin32Window(RHI::SurfaceDescRHI &surfaceDesc, void *&windows,
                             int width = 600, int height = 400,
                             const char *title = "Vulkan RHI Test")
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    // 注册自定义窗口类
    const char *className = "MyGLWindow";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.style = CS_OWNDC; // 保证 OpenGL 可用

    RegisterClass(&wc);

    // 使用自定义类名创建窗口（可以正常拖动）
    HWND hwnd = CreateWindowEx(
        0, className, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, width,
        height, nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) {
        DWORD err = GetLastError();
        return -1; // 打印 err 调试
    }

    ShowWindow(hwnd, SW_SHOW);

    // 填充 SurfaceDescRHI
    surfaceDesc.hwnd = hwnd;
    surfaceDesc.hinstance = hInstance;
    return 0;
}

inline int CreateGlfwWindow(RHI::SurfaceDescRHI &surfaceDesc, void *&windows,
                            int width = 600, int height = 400,
                            const char *title = "Vulkan RHI Test")
{
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window =
        glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        return -1;
    }

#if defined(_WIN32)
    surfaceDesc.hwnd = glfwGetWin32Window(window);
    surfaceDesc.hinstance = GetModuleHandle(nullptr);
#endif

    windows = window;
    return 0;
}
