#include "VulkanInstance.h"

#include <cstring>
#include <iostream>

#include "GLFW/glfw3.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanInstance::VulkanInstance()
{
}

VulkanInstance::~VulkanInstance()
{
    Destroy();
}

bool VulkanInstance::Create()
{
    // 如果启用了验证层(Validation Layer) 检查系统是否支持
    if (_enableValidationLayers && !checkValidationLayerSupport()) {
        PSG::PrintError("请求了验证层，但不可用!");
        return false;
    }

    // 创建 vulkan 实例
    bool ret = createInstance();
    if (!ret) {
        return ret;
    }

    // 设置调试消息
    ret = setupDebugMessenger();
    return ret;
}

void VulkanInstance::Destroy()
{
    // 销毁Debug Messenger
    if (_debugMessenger != VK_NULL_HANDLE) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            _instance, "vkDestroyDebugUtilsMessengerEXT");

        if (func) {
            func(_instance, _debugMessenger, nullptr);
        }
        _debugMessenger = VK_NULL_HANDLE;
    }

    // 销毁 Vulkan 实例
    if (_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
    }
}

bool VulkanInstance::createInstance()
{
    // 应用信息（可用于驱动优化和调试）
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "CustomEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // 获取必须的扩展
    auto extensions = getRequiredExtensions();

    // 创建实例信息
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Debug Messenger 在 Instance 创建阶段就注册
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (_enableValidationLayers) {

        // debug 设置验证层
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();

        // 填充 Debug Messenger 创建信息
        populateDebugCreateInfo(debugCreateInfo);
        createInfo.pNext =
            (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {

        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    VkResult ret = vkCreateInstance(&createInfo, nullptr, &_instance);
    if (VK_SUCCESS != ret) {
        PSG::PrintError("实例创建失败");
        return false;
    }

    return true;
}

bool VulkanInstance::setupDebugMessenger()
{
    if (!_enableValidationLayers) {
        return true;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugCreateInfo(createInfo);

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        _instance, "vkCreateDebugUtilsMessengerEXT");

    if (!func
        || func(_instance, &createInfo, nullptr, &_debugMessenger)
               != VK_SUCCESS) {
        PSG::PrintError("创建调试消息失败");
        return false;
    }

    return true;
}

bool VulkanInstance::checkValidationLayerSupport() const
{
    // 检索所有可用的层列表
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // 检查可用层列表是否是否存在验证层
    for (const char *layerName : _validationLayers) {

        bool layerFound = false;
        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> VulkanInstance::getRequiredExtensions() const
{
    // 获取glfw窗口扩展
    uint32_t glfwCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwCount);
    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwCount);

    // Debug 模式需要 Debug Utils 扩展
    if (_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData)
{
    PSG::PrintMsg("[Vulkan Validation] ", callbackData->pMessage);

    // 返回 VK_FALSE 表示不终止 Vulkan 调用
    return VK_FALSE;
}

void VulkanInstance::populateDebugCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    // 监听的消息级别
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    // 监听的消息类型
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = DebugCallback;
}

} // namespace VKB
