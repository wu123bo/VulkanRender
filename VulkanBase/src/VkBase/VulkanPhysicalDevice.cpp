#include "VulkanPhysicalDevice.h"

#include "PrintMsg.h"

namespace VKB
{

VulkanPhysicalDevice::VulkanPhysicalDevice()
{
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}

bool VulkanPhysicalDevice::Pick(VkInstance instance, VkSurfaceKHR surface)
{
    // 获取可用物理设备列表
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        PSG::PrintError("未找到物理设备!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // 设备评分
    int bestScore = -1;
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

    // 遍历所有物理设备，选择一个合适的
    for (const auto &device : devices) {

        // 判断当前物理设备是否合适
        if (!isDeviceSuitable(device, surface)) {
            continue;
        }

        // 设备评分
        int score = RateDevice(device);
        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
        }
    }

    if (bestDevice == VK_NULL_HANDLE) {
        PSG::PrintError("未找到合适的物理设备!");
        return false;
    }

    // 评分高的设备
    _physicalDevice = bestDevice;

    // 填充队列族索引
    findQueueFamilies(_physicalDevice, surface);

    // 获取物理设备的基本属性
    vkGetPhysicalDeviceProperties(_physicalDevice, &_properties);

    PSG::PrintMsg("选择的物理设备", _properties.deviceName);
    return true;
}

bool VulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device,
                                            VkSurfaceKHR surface)
{
    // 查找队列族
    findQueueFamilies(device, surface);

    // 判断当前物理设备是否找到需要的队列族
    if (!_queueFamilyIndices.isComplete()) {
        return false;
    }

    // 检查交换链 (Swapchain) 扩展
    if (!checkDeviceExtensionSupport(device)) {
        return false;
    }

    return true;
}

void VulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device,
                                             VkSurfaceKHR surface)
{
    _queueFamilyIndices.Reset();

    // 获取物理物理设置支持的队列族
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             families.data());

    // 至少有一个支持图形队列族(VK_QUEUE_GRAPHICS_BIT) 的队列族
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        const auto &family = families[i];

        // 图形队列族
        if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            _queueFamilyIndices.graphicsFamily = i;
        }

        // 呈现队列族 (Present Queue 依赖 Surface)
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                             &presentSupport);
        if (presentSupport) {
            _queueFamilyIndices.presentFamily = i;
        }

        // 找到所需的队列族
        if (_queueFamilyIndices.isComplete()) {
            break;
        }
    }
}

bool VulkanPhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    // 获取物理设备支持的扩展列表
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         extensions.data());

    // 判断物理设备支持的扩展列表是否包含必需的扩展
    for (const char *required : _requiredExtensions) {

        bool found = false;
        for (const auto &ext : extensions) {
            if (strcmp(required, ext.extensionName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

int VulkanPhysicalDevice::RateDevice(VkPhysicalDevice device)
{
    // 获取物理设备的基本属性(硬件信息、驱动信息、能力上限等)
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    // 获取物理设备支持的 Vulkan 功能特性是否支持(GeometryShader等)
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    int score = 0;
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        // 独立显卡
        score += 1000;
    } else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        // 集成显卡
        score += 100;
    } else {
        score += 10;
    }

    // 队列数量越多加分
    uint32_t queueCount = 0;
    // 获取 物理设备支持的队列数量
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
    score += queueCount * 10;

    // 特性加分
    if (features.geometryShader) {
        score += 50;
    }

    return score;
}

} // namespace VKB
