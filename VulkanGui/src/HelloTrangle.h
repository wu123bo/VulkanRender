#ifndef HELLOTRANGLE_H_
#define HELLOTRANGLE_H_

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include "vulkan/vulkan.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

// 队列族索引
struct QueueFamilyIndices
{
    // 图形命令队列族
    std::optional<uint32_t> graphicsFamily;

    // 呈现表面队列族
    std::optional<uint32_t> presentFamily;

    // 通用检查
    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// 交换链是否支持信息
struct SwapChainSupportDetails
{
    // 基本表面功能
    VkSurfaceCapabilitiesKHR capabilities;

    // 表面格式
    std::vector<VkSurfaceFormatKHR> formats;

    // 可用的演示模式
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTrangle
{
public:
    void Run();

private:
    // 创建窗口
    void initWindow();

    // 初始化vulkan
    void initVulkan();

    // main循环
    void mainLoop();

    // 清空资源
    void cleanup();

private:
    // 创建vulkan示例
    void createInstance();

    // 填充调试消息创建信息
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    // 设置调试消息器
    void setupDebugMessenger();

    // 创建窗口表面
    void createSurface();

    // 选择物理设备
    void pickPhysicalDevice();

    // 创建逻辑设备
    void createLogicalDevice();

    // 创建交换链
    void createSwapChain();

    // 创建图像视图
    void createImageViews();

    // 物理设备是否适用
    bool isDeviceSuitable(VkPhysicalDevice device);

    // 检查物理设备扩展支持
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    // 查询交换链支持
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    // 选择交换表面格式
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);

    // 选择交换呈现模式
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);

    // 选择交换范围
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    // 物理设备评分
    int rateDeviceSuitability(VkPhysicalDevice device);

    // 队列家族索引
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    // 该函数将根据是否启用验证层来返回所需的扩展列表
    std::vector<const char *> getRequiredExtensions();

    // 检查验证层是否在可用层列表里面
    bool checkValidationLayerSupport();

    // 检索支持的所有 Vulkan Instance 扩展列表
    int enumerateInstanceExtensions();

    // 调试信息回调
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void *pUserData);

private:
    // glfw窗口
    uint32_t _width = 800;

    uint32_t _height = 600;

    GLFWwindow *_window = nullptr;

private:
    // vulkan 实例
    VkInstance _instance;

    // vulkan 调试信息
    VkDebugUtilsMessengerEXT _debugMessenger;

    // vulkan 物理设备信息
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

    // vulkan 逻辑设备信息
    VkDevice _device = VK_NULL_HANDLE;

    // 图形队列的句柄
    VkQueue _graphicsQueue;

    // 呈现队列的句柄
    VkQueue _presentQueue;

    // 窗口表面
    VkSurfaceKHR _surface;

    // 交换链
    VkSwapchainKHR _swapChain;

    // 交换范围
    VkExtent2D _swapChainExtent;

    // 交换表面格式
    VkFormat _swapChainImageFormat;

    // VkImage句柄
    std::vector<VkImage> _swapChainImages;

    // 图像视图
    std::vector<VkImageView> _swapChainImageViews;

private:
    // 物理设备支持扩展
    const std::vector<const char *> _deviceExtensions = {
        // 交换链
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

private:
    // 验证层
    const std::vector<const char *> _validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    // Debug开启验证层
#ifdef NDEBUG
    const bool _enableValidationLayers = false;
#else
    const bool _enableValidationLayers = true;
#endif
};

#endif // !HELLOTRANGLE_H_
