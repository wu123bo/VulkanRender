#include "RHI/IRHI.h"

#include "RHI/OpenGLRHI.h"
#include "RHI/VulkanRHI.h"

#include "PrintMsg.h"

#include "WindowHelper.h"

int main()
{
    int width = 600;
    int height = 400;
    void *window = nullptr;

    // Surface 描述
    RHI::SurfaceDescRHI surfaceDesc{};
    surfaceDesc.api = RHI::GraphicsAPI::Vulkan;

    // 创建测试窗口
    int ret = CreateGlfwWindow(surfaceDesc, window, width, height);
    if (ret) {
        PSG::PrintError("无法创建有效窗口");
        return -1;
    }

    // 渲染模块
    RHI::IRHI *RHI = nullptr;
    if (surfaceDesc.api == RHI::GraphicsAPI::Vulkan) {
        RHI = new RHI::VulkanRHI();
    } else {
        RHI = new RHI::OpenGLRHI();
    }

    ret = RHI->Init(surfaceDesc, width, height);

    if (ret) {
        PSG::PrintError("渲染模块初始化失败");
        return -1;
    }

    while (!glfwWindowShouldClose((GLFWwindow *)window)) {
        glfwPollEvents();

        RHI->BeginFrame();

        RHI->Render();

        RHI->EndFrame();
    }

    RHI->Shutdown();

    return 0;
}