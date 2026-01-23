#include "VulkanDevice.h"

#include "PrintMsg.h"

#include <set>

namespace VKB
{

VulkanDevice::VulkanDevice()
{
}

VulkanDevice::~VulkanDevice()
{
    Destroy();
}

bool VulkanDevice::Init(const VulkanPhysicalDevice *physicalDevice)
{
    if (nullptr == physicalDevice || physicalDevice->Get() == VK_NULL_HANDLE) {
        PSG::PrintError("逻辑设备创建失败,输入无效的物理设备");
        return false;
    }

    // 获取物理设备
    VkPhysicalDevice phyDevice = physicalDevice->Get();

    // 获取物理设备支持的队列族
    uint32_t graphicsFamily = physicalDevice->GetGraphicsQueueFamily();
    uint32_t presentFamily = physicalDevice->GetPresentQueueFamily();

    // 创建队列信息 (支持 图形/呈现 同族或不同族)
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
    std::set<uint32_t> uniqueQueueFamilies = {graphicsFamily, presentFamily};

    // 队列优先等级
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {

        // 逻辑设备队列创建信息
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamily;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.push_back(queueInfo);
    }

    // 逻辑设备创建信息
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // 逻辑设备队列创建信息
    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfo.size());
    createInfo.pQueueCreateInfos = queueCreateInfo.data();

    // 扩展启用信息
    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

    // 先不启用任何 特性(Feature可根据需要加)
    VkPhysicalDeviceFeatures deviceFeatures{};
    createInfo.pEnabledFeatures = &deviceFeatures;

    // 创建逻辑设备
    VkResult ret = vkCreateDevice(phyDevice, &createInfo, nullptr, &_device);
    if (ret != VK_SUCCESS) {
        PSG::PrintError("创建逻辑设备失败!");
        return false;
    }

    // 获取图形(Graphics) / 呈现(Present) 队列
    vkGetDeviceQueue(_device, graphicsFamily, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, presentFamily, 0, &_presentQueue);

    return true;
}

void VulkanDevice::Destroy()
{
    // 销毁逻辑设备和队列
    if (_device != VK_NULL_HANDLE) {
        vkDestroyDevice(_device, nullptr);
        _device = VK_NULL_HANDLE;
        _graphicsQueue = VK_NULL_HANDLE;
        _presentQueue = VK_NULL_HANDLE;
    }
}

} // namespace VKB
