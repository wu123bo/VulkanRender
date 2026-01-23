#include <string>

#include "PrintMsg.h"

#include "VkBase/VulkanBase.h"

int main()
{
    // 1. 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    // 告诉 GLFW：我们不用 OpenGL，用 Vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // 2. 创建窗口
    GLFWwindow *window =
        glfwCreateWindow(800, 600, "VulkanBase Test", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    // 3. 创建 Vulkan 总控
    VKB::VulkanBase vulkan;

    // 4. 初始化 Vulkan（测试 Instance + Surface）
    if (!vulkan.InitVulkan(window)) {
        std::cerr << "Vulkan init failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // 5. 主循环（暂时不做渲染）
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // 6. 释放 Vulkan（顺序由 VulkanBase 内部保证）
    vulkan.Shutdown();

    // 7. 销毁窗口和 GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}