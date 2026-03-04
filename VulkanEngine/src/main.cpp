
#include "MacroHead.h"
#include "PrintMsg.h"
#include "Renderer.h"
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

    RHI::Renderer *render = new RHI::Renderer();
    ret = render->Init(surfaceDesc, width, height);

    if (!ret) {
        PSG::PrintError("渲染模块初始化失败");
        return -1;
    }

    while (!glfwWindowShouldClose((GLFWwindow *)window)) {
        glfwPollEvents();

        render->RenderFrame();
    }

    render->Shutdown();
    SDelete(render);

    return 0;
}