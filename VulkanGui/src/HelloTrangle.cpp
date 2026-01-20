#include "HelloTrangle.h"
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 此结构应传递给 vkCreateDebugUtilsMessengerEXT 函数以创建
// VkDebugUtilsMessengerEXT 对象
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// 清理 VkDebugUtilsMessengerEXT 对象
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void HelloTrangle::Run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void HelloTrangle::initWindow()
{
    // 创建窗口
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _window = glfwCreateWindow(_width, _height, "Vulkan", nullptr, nullptr);

    // 注册glfw窗口回调函数
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

void HelloTrangle::initVulkan()
{
    // 创建实例
    createInstance();

    // 设置调试信息
    setupDebugMessenger();

    // 创建窗口表面
    createSurface();

    // 选择物理设备
    pickPhysicalDevice();

    // 创建逻辑设备
    createLogicalDevice();

    // 创建交换链
    createSwapChain();

    // 创建图像视图
    createImageViews();

    // 创建渲染通道
    createRenderPass();

    // 创建描述符集布局
    createDescriptorSetLayout();

    // 创建图形管线
    createGraphicsPipeline();

    // 创建帧缓冲区
    createFramebuffers();

    // 创建命令池
    createCommandPool();

    // 创建纹理资源
    createTextureImage();

    // 创建纹理图像视图
    createTextureImageView();

    // 创建纹理采样器
    createTextureSampler();

    // 创建顶点数据缓冲区
    createVertexBuffer();

    // 创建索引缓冲区
    createIndexBuffer();

    // 创建uniform缓冲区
    createUniformBuffers();

    // 创建描述符池
    createDescriptorPool();

    // 创建描述符集合
    createDescriptorSets();

    // 创建分配命令缓冲区
    createCommandBuffers();

    // 创建同步对象
    createSyncObjects();
}

void HelloTrangle::mainLoop()
{
    while (!glfwWindowShouldClose(_window)) {
        // 窗口事件
        glfwPollEvents();

        // 渲染和呈现
        drawFrame();
    }

    // 等待 GPU 完全空闲
    vkDeviceWaitIdle(_device);
}

void HelloTrangle::cleanup()
{
    // 清理交换链 帧缓冲区 图像视图
    cleanupSwapChain();

    // 清理渲染管线
    vkDestroyPipeline(_device, _graphicsPipeline, nullptr);

    // 清理管线布局
    vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);

    // 清理渲染通道
    vkDestroyRenderPass(_device, _renderPass, nullptr);

    // 清理uniform缓冲区
    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
        // MVP
        vkDestroyBuffer(_device, _uniformBuffers[i], nullptr);
        vkFreeMemory(_device, _uniformBuffersMemory[i], nullptr);

        // Color + Alpha
        vkDestroyBuffer(_device, _uniformApColorBuffers[i], nullptr);
        vkFreeMemory(_device, _uniformApColorBuffersMemory[i], nullptr);
    }

    // 清理描述符池句柄
    vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);

    // 清理纹理采样器
    vkDestroySampler(_device, _textureSampler, nullptr);

    // 清理纹理图像视图
    vkDestroyImageView(_device, _textureImageView, nullptr);

    // 清理纹理图像
    vkDestroyImage(_device, _textureImage, nullptr);

    // 清理纹理图像缓冲区内存
    vkFreeMemory(_device, _textureImageMemory, nullptr);

    // 清理描述符集布局
    vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);

    // 清理顶点数据缓冲区
    vkDestroyBuffer(_device, _vertexBuffer, nullptr);

    // 释放顶点数据缓冲区内存
    vkFreeMemory(_device, _vertexBufferMemory, nullptr);

    // 清理索引数据缓冲区
    vkDestroyBuffer(_device, _indexBuffer, nullptr);

    // 释放索引数据缓冲区内存
    vkFreeMemory(_device, _indexBufferMemory, nullptr);

    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
        // 清理信号量
        vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);

        // 清理栅栏
        vkDestroyFence(_device, _inFlightFences[i], nullptr);
    }

    // 清理命令池
    // 当我们释放命令池时，命令缓冲区会被释放，因此对于命令缓冲区清理，无需执行任何额外操作
    vkDestroyCommandPool(_device, _commandPool, nullptr);

    // 清理逻辑设备
    vkDestroyDevice(_device, nullptr);

    // 如果开启验证层
    if (_enableValidationLayers) {
        // 清理调试信息对象
        DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
    }

    // 清理窗口表面资源
    vkDestroySurfaceKHR(_instance, _surface, nullptr);

    // 清理vulkan实例
    vkDestroyInstance(_instance, nullptr);

    // 清理窗口资源
    glfwDestroyWindow(_window);

    // 关闭窗口
    glfwTerminate();
}

void HelloTrangle::createInstance()
{
    // Debug模式下 检查可用层是否存在验证层
    if (_enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("请求了验证层，但不可用!");
    }

    // 应用程序信息
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Trangle";
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // 创建实例信息
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // 获取窗口扩展列表 如果开启验证层里面会包含验证层扩展
    auto extensions = getRequiredExtensions();

    // 扩展列表
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // 检查 Instance 扩展列表
    if (enumerateInstanceExtensions() == 0) {
        throw std::runtime_error("没有支持Vulkan Instance扩展层");
    }

    // 设置验证层 和 调试信息
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (_enableValidationLayers) {
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext =
            (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    // 创建实例
    VkResult ret = vkCreateInstance(&createInfo, nullptr, &_instance);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("vulkan 示例创建失败");
    }
}

void HelloTrangle::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // 严重性类型
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    // 允许您筛选通知回调的消息类型
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // 指定回调函数的指针
    createInfo.pfnUserCallback = debugCallback;

    createInfo.pUserData = nullptr; // 可选
}

void HelloTrangle::setupDebugMessenger()
{
    if (!_enableValidationLayers) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    // 创建 _debugMessenger 调试信息对象
    if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr,
                                     &_debugMessenger)
        != VK_SUCCESS) {
        throw std::runtime_error("无法设置调试消息器!");
    }
}

void HelloTrangle::createSurface()
{
    // glfw 创建表面窗口对象
    VkResult ret =
        glfwCreateWindowSurface(_instance, _window, nullptr, &_surface);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建窗口表面失败!");
    }
}

void HelloTrangle::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    if (0 == deviceCount) {
        throw std::runtime_error("未找到支持 Vulkan 的 GPU!");
    }

    // 获取可用物理设备列表
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    // 找到合适的物理设备 TODO后续使用评分
    for (const auto &device : devices) {
        if (isDeviceSuitable(device)) {

            // 选择该物理设备
            _physicalDevice = device;

            // 获取物理设备属性信息
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);

            std::cout << "选择的GPU: " << deviceProperties.deviceName
                      << std::endl;
            break;
        }
    }

    if (_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("未能找到合适的 GPU!");
    }
}

void HelloTrangle::createLogicalDevice()
{
    // 找到合适的物理设备队列族索引
    QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

    // 逻辑设备队列创建信息数组
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

    // 队列优先级(0.0f - 1.0f)
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {

        // 逻辑设备队列创建信息
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // 使用 vkGetPhysicalDeviceFeatures 查询对纹理压缩、64
    // 位浮点数和多视口渲染（对 VR 有用）等可选功能的支持情况
    VkPhysicalDeviceFeatures deviceFeatures{};

    // 各向异性过滤实际上是一个可选的设备特性
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    // 逻辑设备创建信息
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // 添加指向队列创建信息
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());

    // 设备功能结构的指针
    createInfo.pEnabledFeatures = &deviceFeatures;

    // 特定于物理设备的扩展
    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

    // 逻辑设备不与实例直接交互
    VkResult ret =
        vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建逻辑设备失败!");
    }

    // 检索每个队列族的队列句柄
    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0,
                     &_graphicsQueue);
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
}

void HelloTrangle::createSwapChain()
{
    // 查询物理支持的交换链信息
    SwapChainSupportDetails swapChainSupport =
        querySwapChainSupport(_physicalDevice);

    // 选择交换表面格式
    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);

    // 选择交换呈现模式
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);

    // 选择交换范围
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // 存储交换链图像选择的格式和范围存储
    _swapChainExtent = extent;
    _swapChainImageFormat = surfaceFormat.format;

    // 必须决定在交换链中想要拥有多少个图像 该实现指定其运行所需的最小数量
    // TODO 这里改成不加1 就不会报错了
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // 确保在此过程中不要超过最大图像数量，其中 0 是一个特殊值，表示没有最大值
    if (swapChainSupport.capabilities.maxImageCount > 0
        && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // 创建交换链信息结构
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    // 指定交换链应绑定到的表面
    createInfo.surface = _surface;

    // 指定交换链图像的详细信息
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;

    // 指定每个图像包含的层数(除非您正在开发立体 3D 应用程序，否则此值始终为 1)
    createInfo.imageArrayLayers = 1;

    // 指定我们将在交换链中使用图像进行哪种类型的操作
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // 获取设备队列族
    QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    // 指定如何处理将在多个队列族之间使用的交换链图像
    // 并发模式要求你指定至少两个不同的队列族
    if (indices.graphicsFamily != indices.presentFamily) {
        // 图像可以在多个队列族之间使用，无需显式的所有权转移
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        // 一个图像一次只能由一个队列族拥有，并且必须在另一个队列族中使用它之前显式地转移所有权
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // 对交换链中的图像应用某些变换
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    // 是否应使用 alpha 通道与窗口系统中的其他窗口进行混合
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // 交换呈现模式
    createInfo.presentMode = presentMode;

    // clipped 成员设置为 VK_TRUE，则意味着我们不关心被遮挡像素的颜色
    createInfo.clipped = VK_TRUE;

    // 从头开始重新创建交换链，并且必须在此字段中指定对旧交换链的引用
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // 创建交换链
    VkResult ret =
        vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建交换链失败!");
    }

    // 仅在交换链中指定了最少数量的图像，因此允许实现创建具有更多图像的交换链
    // 所以先查询图像的最终数量，然后调整容器大小，最后再次调用它以检索句柄
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount,
                            _swapChainImages.data());
}

void HelloTrangle::createImageViews()
{
    // 调整列表的大小，以适应我们将要创建的所有图像视图
    _swapChainImageViews.resize(_swapChainImages.size());

    // 便利所有交换链对象
    for (size_t i = 0; i < _swapChainImageViews.size(); i++) {

        // 创建图像视图
        _swapChainImageViews[i] =
            createImageView(_swapChainImages[i], _swapChainImageFormat);
    }
}

void HelloTrangle::createRenderPass()
{
    // 颜色缓冲区附件
    VkAttachmentDescription colorAttachment{};
    // 颜色附件的 format 应该与交换链图像的格式匹配
    colorAttachment.format = _swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // 决定在渲染之前和渲染之后如何处理附件中的颜色和深度数据
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // 决定在渲染之前和渲染之后如何处理附件中的模板数据
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // 内存中像素的布局
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // 附件引用
    VkAttachmentReference colorAttachmentRef{};
    // 通过附件描述数组中的索引指定要引用的附件
    colorAttachmentRef.attachment = 0;
    // 使用附件作为颜色缓冲区
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // 子过程
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // 创建渲染通道信息
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    // 颜色缓冲区附件
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;

    // 子过程
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    // 子通道依赖
    VkSubpassDependency dependency{};
    // 指定依赖项和被依赖子通道的索引
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    // 指定要等待的操作以及这些操作发生的阶段
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    // 等待此操作的操作位于颜色附件阶段，并且涉及颜色附件的写入
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // 指定依赖项数组
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult ret =
        vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建渲染通道失败!");
    }
}

void HelloTrangle::createDescriptorSetLayout()
{
    // 描述符集布局信息
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    // shader里面的绑定ID
    uboLayoutBinding.binding = 0;
    // 描述符的类型
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // 缓冲区对象的数组中值的个数
    uboLayoutBinding.descriptorCount = 1;
    // 指定在哪些着色器阶段将引用描述符
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // 字段仅与图像采样相关的描述符相关
    uboLayoutBinding.pImmutableSamplers = nullptr; // 可选

    // 采样器布局绑定
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    // shader里面的绑定ID
    samplerLayoutBinding.binding = 1;
    // 缓冲区对象的数组中值的个数
    samplerLayoutBinding.descriptorCount = 1;

    // 描述符的类型
    samplerLayoutBinding.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    // 字段仅与图像采样相关的描述符相关
    samplerLayoutBinding.pImmutableSamplers = nullptr; // 可选

    // 指定在哪些着色器阶段将引用描述符
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // 描述符集布局信息
    VkDescriptorSetLayoutBinding uboApColorLayoutBinding{};
    // shader里面的绑定ID
    uboApColorLayoutBinding.binding = 2;
    // 描述符的类型
    uboApColorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // 缓冲区对象的数组中值的个数
    uboApColorLayoutBinding.descriptorCount = 1;
    // 指定在哪些着色器阶段将引用描述符
    uboApColorLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    // 字段仅与图像采样相关的描述符相关
    uboApColorLayoutBinding.pImmutableSamplers = nullptr; // 可选

    // 描述符数组
    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
        uboLayoutBinding, samplerLayoutBinding, uboApColorLayoutBinding};

    // 描述符集布局创建信息
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    // 创建描述符集布局
    VkResult ret = vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr,
                                               &_descriptorSetLayout);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建描述符集布局失败!");
    }
}

void HelloTrangle::createGraphicsPipeline()
{
    // 加载shader文件
    auto vertShaderCode = readShaderFile("Res/Shaders/vertInVertexMVPTex.spv");
    auto fragShaderCode =
        readShaderFile("Res/Shaders/vertInVertexMVPTexFrag4.spv");

    // 创建着色器模块
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // 管线创建信息(顶点)
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // shader类型
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    // 管线创建信息(片元)
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    // 定义一个包含这两个结构的数组
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    // 管线顶点输入
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // 绑定描述
    auto bindingDescription = VERTEX::getBindingDescription();
    // 属性描述
    auto attributeDescriptions = VERTEX::getAttributeDescriptions();

    // 坐标数据绑定描述
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

    // 坐标数据属性描述
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    // 已准备好接受vertices容器格式的顶点数据，并将其传递给我们的顶点着色器

    // 输入汇编
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    // 绘制方式
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // 管线创建时指定它们的计数
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // 光栅化器
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // 超出近平面和远平面的片段将被钳制到它们，而不是被丢弃
    rasterizer.depthClampEnable = VK_FALSE;
    // 设置为 VK_TRUE 则几何体永远不会通过光栅化器阶段
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // 确定如何为几何图形生成片段
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // 线条的粗细
    rasterizer.lineWidth = 1.0f;
    // 面剔除类型
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    // 正面的面的顶点顺序，可以是顺时针或逆时针
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    // 光栅化器可以通过添加常量值或根据片段的斜率来偏移深度值
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // 多重采样(保持禁用状态)
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    // 深度和模板测试(目前没用)

    // 颜色混合
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // 设置为VK_FALSE，则来自片段着色器的新颜色将直接传递不作修改。否则,两个混合操作以计算新颜色
    colorBlendAttachment.blendEnable = VK_FALSE;

    // 颜色混合
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // 视口和剪裁状态设置为动态
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount =
        static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // 创建管线布局信息
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    // 绑定描述符集布局
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    // 创建管线布局
    VkResult ret = vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr,
                                          &_pipelineLayout);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建管线布局失败!");
    }

    // 创建图形管线信息
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    // shader数组
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    // 顶点输入
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    // 输入汇编
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    // 管线创建时指定它们的计数
    pipelineInfo.pViewportState = &viewportState;
    // 光栅化器
    pipelineInfo.pRasterizationState = &rasterizer;
    // 多重采样
    pipelineInfo.pMultisampleState = &multisampling;
    // 深度和模板测试(目前没用)
    pipelineInfo.pDepthStencilState = nullptr;
    // 颜色混合
    pipelineInfo.pColorBlendState = &colorBlending;
    // 视口和剪裁状态设置为动态
    pipelineInfo.pDynamicState = &dynamicState;
    // 管线布局
    pipelineInfo.layout = _pipelineLayout;
    // 渲染通道
    pipelineInfo.renderPass = _renderPass;
    // 引用渲染通道以及将使用此图形管线的子通道的索引
    pipelineInfo.subpass = 0;
    // Vulkan 允许您通过派生自现有管线来创建新的图形管线
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // 可选
    pipelineInfo.basePipelineIndex = -1;              // 可选

    // 创建渲染管线
    ret = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                    nullptr, &_graphicsPipeline);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建图形管线失败!");
    }

    // 清理着色器模块
    vkDestroyShaderModule(_device, vertShaderModule, nullptr);
    vkDestroyShaderModule(_device, fragShaderModule, nullptr);
}

void HelloTrangle::createFramebuffers()
{
    // 调整容器大小以保存所有帧缓冲
    _swapChainFramebuffers.resize(_swapChainImageViews.size());

    // 迭代图像视图并从中创建帧缓冲
    for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {_swapChainImageViews[i]};

        // 创建帧缓冲信息
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = _swapChainExtent.width;
        framebufferInfo.height = _swapChainExtent.height;
        framebufferInfo.layers = 1;

        // 创建帧缓冲
        VkResult ret = vkCreateFramebuffer(_device, &framebufferInfo, nullptr,
                                           &_swapChainFramebuffers[i]);
        if (ret != VK_SUCCESS) {
            throw std::runtime_error("创建帧缓冲失败!");
        }
    }
}

void HelloTrangle::createCommandPool()
{
    // 找到合适的物理设备队列族索引
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);

    // 创建命令池信息
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // 允许单独重新记录命令缓冲区，如果没有此标志，则必须一起重置所有命令缓冲区
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    // 图形命令队列族
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    // 创建命令池
    VkResult ret =
        vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建命令池失败!");
    }
}

void HelloTrangle::createTextureImage()
{
    // 加载纹理图片
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load("Res/Image/statue.jpg", &texWidth, &texHeight,
                                &texChannels, STBI_rgb_alpha);

    // 像素数组大小
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (nullptr == pixels) {
        throw std::runtime_error("加载纹理图像失败!");
    }

    // 暂存缓冲区
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // 创建暂存缓冲区
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                     | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    // 映射内存
    void *data = nullptr;
    vkMapMemory(_device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));

    // 取消映射
    vkUnmapMemory(_device, stagingBufferMemory);

    // 释放像素数组
    stbi_image_free(pixels);

    // 创建纹理图像
    createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage,
                _textureImageMemory);

    // 布局转换
    transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // 将缓冲区复制到图像
    copyBufferToImage(stagingBuffer, _textureImage,
                      static_cast<uint32_t>(texWidth),
                      static_cast<uint32_t>(texHeight));
    // 布局转换
    transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // 将数据从暂存缓冲区复制到设备缓冲区后，清理暂存缓冲区
    vkDestroyBuffer(_device, stagingBuffer, nullptr);
    vkFreeMemory(_device, stagingBufferMemory, nullptr);
}

void HelloTrangle::createTextureImageView()
{
    // 创建图像视图
    _textureImageView = createImageView(_textureImage, VK_FORMAT_R8G8B8A8_SRGB);
}

void HelloTrangle::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    // 指定如何插值放大或缩小的纹素
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    // 按轴指定寻址模式
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    // 指定是否应使用各向异性过滤
    samplerInfo.anisotropyEnable = VK_TRUE;

    // 查询物理设备基本属性
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

    // 最大质量
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    // 指定当使用钳位到边界寻址模式采样超出图像范围时返回的颜色
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    // 指定你想要使用哪个坐标系来寻址图像中的纹素
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    // 如果启用了比较函数，则纹素将首先与一个值进行比较，并且该比较的结果用于过滤操作。
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    // mipmapping 设置信息
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    // 创建逻辑设备中的 deviceFeatures.samplerAnisotropy = VK_TRUE;
    // 除了强制要求各向异性过滤的可用性之外，也可以通过有条件地设置以下代码来简单地不使用它
    // samplerInfo.anisotropyEnable = VK_FALSE;
    // samplerInfo.maxAnisotropy = 1.0f;

    VkResult ret =
        vkCreateSampler(_device, &samplerInfo, nullptr, &_textureSampler);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建纹理采样器失败!");
    }
}

void HelloTrangle::createVertexBuffer()
{
    // 计算顶点数据内存大小
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();

    // 暂存缓冲区
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // 创建暂存缓冲区
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                     | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    // 填充顶点缓冲区
    // 映射内存的指针
    void *data = nullptr;
    // 映射内存
    vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);

    // 将顶点数据 memcpy 到映射内存
    memcpy(data, _vertices.data(), (size_t)bufferSize);

    // 取消映射
    vkUnmapMemory(_device, stagingBufferMemory);

    // 创建缓冲区
    createBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT
                     | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer,
                 _vertexBufferMemory);

    // 拷贝缓冲区
    copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

    // 将数据从暂存缓冲区复制到设备缓冲区后，清理暂存缓冲区
    vkDestroyBuffer(_device, stagingBuffer, nullptr);
    vkFreeMemory(_device, stagingBufferMemory, nullptr);
}

void HelloTrangle::createIndexBuffer()
{
    // 计算索引数据内存大小
    VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

    // 暂存缓冲区
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // 创建暂存缓冲区
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                     | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    // 映射内存的指针
    void *data = nullptr;
    // 映射内存
    vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);

    // 将索引数据 memcpy 到映射内存
    memcpy(data, _indices.data(), (size_t)bufferSize);

    // 取消映射
    vkUnmapMemory(_device, stagingBufferMemory);

    // 创建缓冲区
    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

    // 拷贝缓冲区
    copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

    // 将数据从暂存缓冲区复制到设备缓冲区后，清理暂存缓冲区
    vkDestroyBuffer(_device, stagingBuffer, nullptr);
    vkFreeMemory(_device, stagingBufferMemory, nullptr);
}

void HelloTrangle::createUniformBuffers()
{
    // 计算结构内存大小
    VkDeviceSize bufferSize = sizeof(MVPMATRIX);
    VkDeviceSize apColorBufferSize = sizeof(ALPHACOLOR);

    // 分配处理帧数量大小
    _uniformBuffers.resize(_MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMemory.resize(_MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMapped.resize(_MAX_FRAMES_IN_FLIGHT);

    _uniformApColorBuffers.resize(_MAX_FRAMES_IN_FLIGHT);
    _uniformApColorBuffersMemory.resize(_MAX_FRAMES_IN_FLIGHT);
    _uniformApColorBuffersMapped.resize(_MAX_FRAMES_IN_FLIGHT);

    // 循环创建uniform缓冲区
    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
        // 创建uniform缓冲区
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                         | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     _uniformBuffers[i], _uniformBuffersMemory[i]);

        // 映射内存
        // 该缓冲区在应用程序的整个生命周期中都保持映射到此指针。此技术称为“持久映射”
        vkMapMemory(_device, _uniformBuffersMemory[i], 0, bufferSize, 0,
                    &_uniformBuffersMapped[i]);

        // 创建apColor uniform缓冲区
        createBuffer(apColorBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                         | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     _uniformApColorBuffers[i],
                     _uniformApColorBuffersMemory[i]);

        // 映射内存
        vkMapMemory(_device, _uniformApColorBuffersMemory[i], 0,
                    apColorBufferSize, 0, &_uniformApColorBuffersMapped[i]);
    }
}

void HelloTrangle::createDescriptorPool()
{
    // 描述符池创建信息
    std::array<VkDescriptorPoolSize, 2> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // 两个 UBO（MVP + Color） 所以 * 2
    poolSizes[0].descriptorCount =
        static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT * 2);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT);

    // 为每一帧分配一个描述池
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // 用的单个描述符的最大数量
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    // 指定可以分配的最大描述符集数量
    poolInfo.maxSets = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT);

    // 创建描述符池
    VkResult ret =
        vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPool);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建描述符池失败!");
    }
}

void HelloTrangle::createDescriptorSets()
{
    // 描述符集合
    std::vector<VkDescriptorSetLayout> layouts(
        static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT), _descriptorSetLayout);

    // 分配描述符集合信息
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    // 指定要从中分配的描述符池
    allocInfo.descriptorPool = _descriptorPool;

    // 分配的描述符集数量
    allocInfo.descriptorSetCount = static_cast<uint32_t>(_MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    // 分配描述符集合
    _descriptorSets.resize(_MAX_FRAMES_IN_FLIGHT);
    VkResult ret =
        vkAllocateDescriptorSets(_device, &allocInfo, _descriptorSets.data());
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("分配描述符集合失败!");
    }

    // 配置描述符
    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {

        // 指定缓冲区以及其中包含描述符数据的区域
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = _uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(MVPMATRIX);

        // 图像信息
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = _textureImageView;
        imageInfo.sampler = _textureSampler;

        // 颜色透明度信息
        VkDescriptorBufferInfo bufferApColorInfo{};
        bufferApColorInfo.buffer = _uniformApColorBuffers[i];
        bufferApColorInfo.offset = 0;
        bufferApColorInfo.range = sizeof(ALPHACOLOR);

        // 描述符配置
        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        // 更新的描述符集
        descriptorWrites[0].dstSet = _descriptorSets[i];

        // 统一缓冲区绑定索引 0
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;

        // 再次指定描述符的类型
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        // 指定新的数组元素的数量
        descriptorWrites[0].descriptorCount = 1;

        // 配置了描述
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        descriptorWrites[0].pImageInfo = nullptr;       // 可选
        descriptorWrites[0].pTexelBufferView = nullptr; // 可选

        // 配置纹理图像采样器描述符
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
        descriptorWrites[1].pBufferInfo = nullptr;      // 可选
        descriptorWrites[1].pTexelBufferView = nullptr; // 可选

        // 配置颜色透明度 ubo
        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = _descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &bufferApColorInfo;
        descriptorWrites[2].pImageInfo = nullptr;       // 可选
        descriptorWrites[2].pTexelBufferView = nullptr; // 可选

        // 应用更新描述符
        vkUpdateDescriptorSets(_device,
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void HelloTrangle::createImage(uint32_t width, uint32_t height, VkFormat format,
                               VkImageTiling tiling, VkImageUsageFlags usage,
                               VkMemoryPropertyFlags properties, VkImage &image,
                               VkDeviceMemory &imageMemory)
{
    // 创建图像信息
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    // 图像类型
    imageInfo.imageType = VK_IMAGE_TYPE_2D;

    // 图像宽高深
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;

    //
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    // 图像格式
    imageInfo.format = format;
    imageInfo.tiling = tiling;

    // GPU 不可用，并且第一次转换将丢弃纹素
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // 图像用作传输目标和着色器采样
    imageInfo.usage = usage;

    // 图像只能由一个队列族使用
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // 图像将作为颜色目标使用，但不使用多重采样
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // 可选

    // 创建图像
    VkResult ret = vkCreateImage(_device, &imageInfo, nullptr, &image);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建纹理图像失败!");
    }

    // 查询其内存需求
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device, image, &memRequirements);

    // 内存分配信息
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    // 合适的内存类型
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    ret = vkAllocateMemory(_device, &allocInfo, nullptr, &imageMemory);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("分配图像内存失败!");
    }

    // 分配成功 将此内存与图像关联起来
    vkBindImageMemory(_device, image, imageMemory, 0);
}

VkImageView HelloTrangle::createImageView(VkImage image, VkFormat format)
{
    // 创建纹理图像视图信息
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    VkResult ret = vkCreateImageView(_device, &viewInfo, nullptr, &imageView);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建纹理图像视图失败!");
    }

    return imageView;
}

void HelloTrangle::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    // 顶点缓冲区创建信息
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // 为单位指定缓冲区的大小
    bufferInfo.size = size;

    // 指示缓冲区中的数据将用于哪些目的
    bufferInfo.usage = usage;

    // 缓冲区也可以由特定的队列族拥有，或者同时在多个队列族之间共享
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // 创建顶点缓冲区创建信息
    VkResult ret = vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建顶点缓冲区失败!");
    }

    // 查询其内存需求
    VkMemoryRequirements memRequirements;
    // size：所需内存量的字节大小，可能与 bufferInfo.size 不同。
    // alignment：缓冲区在内存分配区域中开始的字节偏移量，取决于 bufferInfo
    // .usage 和 bufferInfo.flags。
    //  memoryTypeBits：适用于该缓冲区的内存类型的位域。
    vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

    // 内存分配信息
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // 内存量的字节大小
    allocInfo.allocationSize = memRequirements.size;
    // 合适的内存类型
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits, properties);

    // 顶点缓冲区分配内存
    ret = vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("分配顶点缓冲区内存失败!");
    }

    // 分配成功 将此内存与缓冲区关联起来
    vkBindBufferMemory(_device, buffer, bufferMemory, 0);
}

void HelloTrangle::transitionImageLayout(VkImage image, VkFormat format,
                                         VkImageLayout oldLayout,
                                         VkImageLayout newLayout)
{
    // 开始单次命令记录
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    // 设置图像内存屏障
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    // 指定布局转换前后状态
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    // 队列族索引（不跨队列）
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    // 指定要转换的图像
    barrier.image = image;

    // 指定要转换的子资源（mipmap + layer）
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    // 设置访问掩码，控制同步
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
               && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("不支持的布局过渡!");
    }

    // 执行管线屏障，实现布局转换
    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                         nullptr, 0, nullptr, 1, &barrier);

    // 结束命令记录并提交
    endSingleTimeCommands(commandBuffer);
}

void HelloTrangle::copyBufferToImage(VkBuffer buffer, VkImage image,
                                     uint32_t width, uint32_t height)
{
    // 开始单次命令记录
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    // 指定缓冲区到图像的拷贝区域
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    // 指定拷贝的 image 子资源
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    // 指定拷贝位置和大小
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    // 执行缓冲区到图像的拷贝
    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // 结束命令记录并提交
    endSingleTimeCommands(commandBuffer);
}

void HelloTrangle::copyBuffer(VkBuffer srcBuffer, VkBuffer destBuffer,
                              VkDeviceSize size)
{
    // 创建命令缓冲区记录 并绑定 开始单次命令
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    // 命令传输
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.srcOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, destBuffer, 1, &copyRegion);

    // 停止记录  清理用于传输操作的命令缓冲区
    endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer HelloTrangle::beginSingleTimeCommands()
{
    // 内存传输操作使用命令缓冲区执行，就像绘制命令一样。
    // 因此，我们必须首先分配一个临时命令缓冲区

    // 创建分配命令缓冲区信息
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    // 创建分配命令缓冲区
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // 创建命令缓冲区记录
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void HelloTrangle::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    // 停止命令缓冲区记录
    vkEndCommandBuffer(commandBuffer);

    // 提交命令缓冲区
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // 将命令缓冲区提交到图形队列
    vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    // 等待传输队列使用 vkQueueWaitIdle 变为闲置状态
    vkQueueWaitIdle(_graphicsQueue);

    // 清理用于传输操作的命令缓冲区
    vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
}

uint32_t HelloTrangle::findMemoryType(uint32_t typeFilter,
                                      VkMemoryPropertyFlags properties)
{
    // 查询有关可用内存类型的信息
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

    // 找到一种适合缓冲区本身的内存类型
    for (size_t i = 0; i < memProperties.memoryTypeCount; i++) {

        // 遍历物理设备支持的所有内存类型（通常 0~31）
        // 找到一个既被驱动允许、又满足属性要求的内存类型
        // 返回该内存类型索引，用于 vkAllocateMemory
        if ((typeFilter & (1 << i))
            && ((memProperties.memoryTypes[i].propertyFlags & properties)
                == properties)) {
            return i;
        }
    }

    throw std::runtime_error("未能找到合适的内存类型!");
    return 0;
}

void HelloTrangle::createCommandBuffers()
{
    // 根据同时处理帧数 分配
    _commandBuffers.resize(_MAX_FRAMES_IN_FLIGHT);

    // 创建分配命令缓冲区信息
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    // 命令池
    allocInfo.commandPool = _commandPool;

    // 指定分配的命令缓冲区是主命令缓冲区还是辅助命令缓冲区
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    // 分配命令缓冲区数量
    allocInfo.commandBufferCount = _commandBuffers.size();

    // 创建分配命令缓冲区
    VkResult ret =
        vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data());
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("分配命令缓冲区失败!");
    }
}

void HelloTrangle::createSyncObjects()
{
    // 根据同时处理帧数 分配
    _imageAvailableSemaphores.resize(_MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(_MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(_MAX_FRAMES_IN_FLIGHT);

    // 创建信号量信息
    VkSemaphoreCreateInfo semaphoreInfo{};
    // 除了 sType 之外没有任何必需的字段
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // 创建栅栏信息
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // 创建处于已发出信号状态的栅栏
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // 创建_MAX_FRAMES_IN_FLIGHT个信号量和栅栏
    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {

        // 创建图像可用信号量
        VkResult ret = vkCreateSemaphore(_device, &semaphoreInfo, nullptr,
                                         &_imageAvailableSemaphores[i]);
        if (ret != VK_SUCCESS) {
            throw std::runtime_error("创建信号量失败!");
        }

        // 创建渲染完成信号量
        ret = vkCreateSemaphore(_device, &semaphoreInfo, nullptr,
                                &_renderFinishedSemaphores[i]);
        if (ret != VK_SUCCESS) {
            throw std::runtime_error("创建信号量失败!");
        }

        // 栅栏对象
        ret = vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]);
        if (ret != VK_SUCCESS) {
            throw std::runtime_error("创建栅栏对象失败!");
        }
    }
}

void HelloTrangle::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                       uint32_t imageIndex)
{
    // 创建命令缓冲区记录信息
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // 可选

    // 仅与辅助命令缓冲区相关
    beginInfo.pInheritanceInfo = nullptr; // 可选

    // 创建命令缓冲区记录
    VkResult ret = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("无法开始录制命令缓冲区!");
    }

    // 启动渲染通道
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    // 渲染通道
    renderPassInfo.renderPass = _renderPass;
    // 渲染通道本身和要绑定的附件 绘制的交换链图像绑定帧缓冲
    renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];

    // 渲染区域的大小
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapChainExtent;

    // 参数定义了用于 VK_ATTACHMENT_LOAD_OP_CLEAR 的清除值
    VkClearValue clearColor = {{{1.0f, 1.0f, 1.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    // 渲染通道开始
    //  VK_SUBPASS_CONTENTS_INLINE
    //  渲染通道命令将嵌入到主命令缓冲区本身中，并且不会执行辅助命令缓冲区
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    // 绑定图形管线
    // 第二个参数指定管线对象是图形管线还是计算管线
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _graphicsPipeline);

    // 为此管线指定了视口和剪刀状态为动态 发出绘制命令之前在命令缓冲区中设置它们
    VkViewport viewport{};
    viewport.x = 0.0f; // 左上角 X 坐标
    viewport.y = 0.0f; // 左上角 Y 坐标
    viewport.width = static_cast<float>(_swapChainExtent.width);
    viewport.height = static_cast<float>(_swapChainExtent.height);
    viewport.minDepth = 0.0f; // 深度最小值
    viewport.maxDepth = 1.0f; // 深度最大值

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};           // 剪裁矩形左上角
    scissor.extent = _swapChainExtent; // 宽高与交换链一致
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // 绑定顶点缓冲区
    VkBuffer vertexBuffers[] = {_vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // 使用描述符集
    // 与顶点缓冲区和索引缓冲区不同，描述符集对于图形管线不是唯一的。
    // 因此，我们需要指定是否要将描述符集绑定到图形管线或计算管线。
    // 然后是开始索引 个数 绑定的描述符集合
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            _pipelineLayout, 0, 1,
                            &_descriptorSets[_currentFrame], 0, nullptr);

    // 绑定索引缓冲区
    vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    // 顶点数量
    // 用于实例化渲染，如果不这样做，则使用 1
    // 用作顶点缓冲区的偏移量，定义了 gl_VertexIndex 的最小值
    // 用作实例化渲染的偏移量，定义了 gl_InstanceIndex 的最小值
    // vkCmdDraw(commandBuffer, static_cast<uint32_t>(_vertices.size()), 1, 0,
    // 0);

    // 索引绘制
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1,
                     0, 0, 0);

    // 结束渲染通道
    vkCmdEndRenderPass(commandBuffer);

    ret = vkEndCommandBuffer(commandBuffer);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("无法记录命令缓冲区!");
    }
}

void HelloTrangle::cleanupSwapChain()
{
    // 清理帧缓冲区
    for (auto framebuffer : _swapChainFramebuffers) {
        vkDestroyFramebuffer(_device, framebuffer, nullptr);
    }

    // 清理图像视图
    for (auto imageView : _swapChainImageViews) {
        vkDestroyImageView(_device, imageView, nullptr);
    }

    // 清理交换链
    vkDestroySwapchainKHR(_device, _swapChain, nullptr);
}

void HelloTrangle::recreateSwapChain()
{
    // 处理最小化的情况
    int width = 0, height = 0;
    glfwGetFramebufferSize(_window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window, &width, &height);
        glfwWaitEvents();
    }

    // 等待 GPU 完全空闲
    vkDeviceWaitIdle(_device);

    // 清理交换链
    cleanupSwapChain();

    // 创建交换链
    createSwapChain();

    // 创建图像视图
    createImageViews();

    // 创建帧缓冲区
    createFramebuffers();
}

void HelloTrangle::drawFrame()
{
    // 等待前一帧完成，以便可以使用命令缓冲区和信号量
    vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;

    // 从交换链获取图像
    // 返回值判断交换链是否无法再用于渲染
    VkResult ret = vkAcquireNextImageKHR(
        _device, _swapChain, UINT64_MAX,
        _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    // VK_ERROR_OUT_OF_DATE_KHR
    // 交换链已与表面不兼容，无法再用于渲染。通常在窗口调整大小后发生

    //  VK_SUBOPTIMAL_KHR
    // 交换链仍然可以成功呈现到表面，但表面属性不再完全匹配。
    if (ret == VK_ERROR_OUT_OF_DATE_KHR) {
        // 重建交换链
        recreateSwapChain();
        return;
    } else if (ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("获取交换链图像失败!");
    }

    // 更新uniform缓冲区
    updateUniformBuffer(_currentFrame);

    // 调用手动将栅栏重置为未发出信号的状态
    // 只有在提交工作时才重置围栏
    vkResetFences(_device, 1, &_inFlightFences[_currentFrame]);

    // 记录命令缓冲区
    vkResetCommandBuffer(_commandBuffers[_currentFrame], 0);
    recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex);

    // 提交命令缓冲区
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    // 指定在执行开始之前要等待哪些信号量以及在管道的哪个阶段等待
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    // 指定要实际提交执行的命令缓冲区
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];

    // 在命令缓冲区执行完成后要发出信号的信号量
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // 将命令缓冲区提交到图形队列
    ret = vkQueueSubmit(_graphicsQueue, 1, &submitInfo,
                        _inFlightFences[_currentFrame]);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("提交绘制命令缓冲区失败!");
    }

    // 将结果提交回交换链
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // 指定在演示发生之前要等待的信号量，就像 VkSubmitInfo 一样
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    // 指定要向其显示图像的交换链以及每个交换链的图像索引
    VkSwapchainKHR swapChains[] = {_swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    // 可选的参数允许指定一个 VkResult
    // 值数组，以检查每个单独的交换链演示是否成功
    presentInfo.pResults = nullptr; // Optional

    // 提交将图像呈现给交换链的请求(和vkAcquireNextImageKHR函数返回相同的值)
    ret = vkQueuePresentKHR(_presentQueue, &presentInfo);

    // 如果交换链是次优的
    if (ret == VK_ERROR_OUT_OF_DATE_KHR || ret == VK_SUBOPTIMAL_KHR) {
        // 是否发生了调整大小的操作
        _framebufferResized = false;

        // 重建交换链
        recreateSwapChain();
    } else if (ret != VK_SUCCESS) {
        throw std::runtime_error("无法呈现交换链图像!");
    }

    // 前进到下一帧
    _currentFrame = (_currentFrame + 1) % _MAX_FRAMES_IN_FLIGHT;
}

void HelloTrangle::updateUniformBuffer(uint32_t currentImage)
{
    // 计算自渲染开始以来以秒为单位的时间
    static auto startTime = std::chrono::high_resolution_clock::now();

    // 当前时间
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     currentTime - startTime)
                     .count();

    /**************更新 MVP ubo***************************/

    MVPMATRIX ubo{};
    // 模型矩阵沿Z轴每秒旋转90°
    ubo.model = glm::rotate(MAT_4(1.0f), time * glm::radians(90.0f),
                            PTF_3D(0.0f, 0.0f, 1.0f));

    // 视图矩阵
    ubo.view = glm::lookAt(PTF_3D(2.0f, 2.0f, 2.0f), PTF_3D(0.0f),
                           PTF_3D(0.0f, 0.0f, 1.0f));

    // 投影矩阵
    float sceen =
        (float)_swapChainExtent.width / (float)_swapChainExtent.height;
    ubo.proj = glm::perspective(glm::radians(45.0f), sceen, 0.1f, 10.0f);

    // glm裁剪坐标Y是反的
    ubo.proj[1][1] *= -1;

    // 拷贝数据到缓冲区内存
    memcpy(_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

    /**************更新 颜色透明度 ubo***************************/
    static bool firstCall = true;
    if (firstCall) {
        std::srand(
            static_cast<unsigned int>(std::time(nullptr))); // 只初始化一次
        firstCall = false;
    }

    ALPHACOLOR colorUbo{};
    colorUbo.color = glm::vec3(static_cast<float>(std::rand()) / RAND_MAX,
                               static_cast<float>(std::rand()) / RAND_MAX,
                               static_cast<float>(std::rand()) / RAND_MAX);
    colorUbo.alpha = static_cast<float>(std::rand()) / RAND_MAX;

    memcpy(_uniformApColorBuffersMapped[currentImage], &colorUbo,
           sizeof(colorUbo));
}

VkShaderModule HelloTrangle::createShaderModule(const std::vector<char> &code)
{
    // 着色器模块创建信息
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    // 创建着色器模块
    VkShaderModule shaderMoule;
    VkResult ret =
        vkCreateShaderModule(_device, &createInfo, nullptr, &shaderMoule);
    if (ret != VK_SUCCESS) {
        throw std::runtime_error("创建着色器模块失败!");
    }

    return shaderMoule;
}

bool HelloTrangle::isDeviceSuitable(VkPhysicalDevice device)
{
    // 查询物理设备基本属性
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // 查询物理设备支持的所有功能
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    // 找到合适的物理设备队列族索引
    QueueFamilyIndices indices = findQueueFamilies(device);

    // 检查是否支持物理设备扩展支持
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    // 检查是否交换链支持
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty()
                            && !swapChainSupport.presentModes.empty()
                            && supportedFeatures.samplerAnisotropy;
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool HelloTrangle::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         nullptr);

    // 查询物理设备支持的设备级扩展列表
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(_deviceExtensions.begin(),
                                             _deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails
HelloTrangle::querySwapChainSupport(VkPhysicalDevice device)
{
    // 此函数在确定支持的功能时会考虑指定的 VkPhysicalDevice 和 VkSurfaceKHR
    // 窗口表面。所有支持查询函数都将这两个作为第一个参数，因为它们是交换链的核心组件。

    SwapChainSupportDetails details;

    // 查询是否支持基本表面功能
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface,
                                              &details.capabilities);

    // 查询是否支持基本表面格式
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount,
                                         nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount,
                                             details.formats.data());
    }

    // 查询支持的演示模式的工作方式与 vkGetPhysicalDeviceSurfacePresentModesKHR
    // 完全相同
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface,
                                              &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, _surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR HelloTrangle::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    // 查询色彩空间组合是否可用
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace
                   == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    // 可用格式的“好”坏程度对它们进行排名，但在大多数情况下，只需使用指定的第一个格式即可
    return availableFormats[0];
}

VkPresentModeKHR HelloTrangle::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    // 查询是否可用VK_PRESENT_MODE_MAILBOX_KHR 一个非常好的折中方案
    // 它允许我们避免撕裂，同时通过渲染尽可能最新的新图像，直到垂直消隐，仍然保持相当低的延迟
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // 交换链是一个队列，显示器会在刷新时从队列的前面获取图像，程序会将渲染的图像插入队列的后面。
    // 如果队列已满，则程序必须等待。 这与现代游戏中发现的垂直同步最相似。
    // 显示器刷新的时刻称为“垂直消隐
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
HelloTrangle::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    // 根据 Surface 能力选择交换链图像尺寸（Extent）

    // 如果 Surface 已经给出了固定的尺寸（如某些平台）
    // 直接使用该尺寸，不能自行修改
    if (capabilities.currentExtent.width
        != (std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    } else {
        // 否则（Windows / X11 等平台）
        // 从 GLFW 获取当前帧缓冲区实际像素大小
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

int HelloTrangle::rateDeviceSuitability(VkPhysicalDevice device)
{
    return 10;
}

QueueFamilyIndices HelloTrangle::findQueueFamilies(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    // 获取物理物理设置支持的队列族
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    // 至少一个支持 VK_QUEUE_GRAPHICS_BIT 的队列族
    uint32_t i = 0;
    QueueFamilyIndices indices;
    for (const auto &queueFamily : queueFamilies) {

        // 图形命令队列族
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // 呈现队列族索引
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface,
                                             &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        // 如果有可用的队列提前退出
        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

std::vector<const char *> HelloTrangle::getRequiredExtensions()
{
    const char **glfwExtensions;
    uint32_t glfwExtensionCount = 0;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // 获取窗口扩展列表
    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);
    // 如果开启验证层把验证层扩展放入列表
    if (_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool HelloTrangle::checkValidationLayerSupport()
{
    //  检索所有可用的层列表
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // 检查验证层是否在可用层列表里面
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

int HelloTrangle::enumerateInstanceExtensions()
{
    // 检索支持的所有 Vulkan Instance 扩展列表
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (0 == extensionCount) {
        return extensionCount;
    }

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());

    std::cout << "Vulkan Instance 可用扩展:\n";
    for (const auto &extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    return extensionCount;
}

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTrangle::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    // messageSeverity
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT：诊断消息
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT：信息性消息，例如资源的创建
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT：关于不一定是错误的行为的消息，但很可能是应用程序中的错误
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT：关于无效且可能导致崩溃的行为的消息

    std::cerr << "验证层:" << pCallbackData->pMessage << std::endl;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    }

    // messageType
    // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT：发生了一些与规范或性能无关的事件
    // VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT：发生了一些违反规范或指示可能错误的事情
    // VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT：Vulkan 的潜在非最佳使用

    // pCallbackData
    // 参数是指一个VkDebugUtilsMessengerCallbackDataEXT 结构
    // 其中包含消息本身的详细信息，最重要的成员是
    // pMessage：作为空终止字符串的调试消息
    // pObjects：与消息相关的 Vulkan 对象句柄数组
    // objectCount：数组中的对象数

    // pUserData
    // 参数包含一个在回调设置期间指定的指针，允许您将自己的数据传递给它。

    return VK_SUCCESS;
}

void HelloTrangle::framebufferResizeCallback(GLFWwindow *window, int width,
                                             int height)
{
    auto app =
        reinterpret_cast<HelloTrangle *>(glfwGetWindowUserPointer(window));

    if (nullptr != app) {
        app->_framebufferResized = true;
    }
}
