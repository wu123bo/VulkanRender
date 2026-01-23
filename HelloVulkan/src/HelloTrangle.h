#ifndef HELLOTRANGLE_H_
#define HELLOTRANGLE_H_

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include "MacroHead.h"
#include "VulkanHead.h"

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

    // 创建渲染通道
    void createRenderPass();

    // 创建描述符集布局
    void createDescriptorSetLayout();

    // 创建图形管线
    void createGraphicsPipeline();

    // 创建帧缓冲区
    void createFramebuffers();

    // 创建命令池
    void createCommandPool();

    // 创建多重采样颜色资源
    void createColorResources();

    // 创建深度资源
    void createDepthResources();

    // 创建纹理资源
    void createTextureImage();

    // 创建纹理图像视图
    void createTextureImageView();

    // 创建纹理采样器
    void createTextureSampler();

    // 加载模型
    void loadModel();

    // 创建顶点缓冲区
    void createVertexBuffer();

    // 创建索引缓冲区
    void createIndexBuffer();

    // 创建uniform缓冲区
    void createUniformBuffers();

    // 创建描述符池
    void createDescriptorPool();

    // 创建描述符集合
    void createDescriptorSets();

    // 创建纹理图像
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                     VkSampleCountFlagBits numSamples, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VkImage &image,
                     VkDeviceMemory &imageMemory);

    // 创建图像视图
    VkImageView createImageView(VkImage image, VkFormat format,
                                VkImageAspectFlags aspectFlags,
                                uint32_t mipLevels);

    // 创建缓冲区
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    // 布局转换
    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t mipLevels);

    // 生成多级渐远纹理
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                         int32_t texHeight, uint32_t mipLevels);

    // 将缓冲区复制到图像
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                           uint32_t height);

    // 拷贝缓冲区
    void copyBuffer(VkBuffer srcBuffer, VkBuffer destBuffer, VkDeviceSize size);

    // 创建附件
    VkAttachmentDescription CreateAttachment(
        VkFormat format, VkSampleCountFlagBits samples,
        VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
        VkImageLayout finalLayout,
        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

    // 创建附件引用
    VkAttachmentReference makeAttachmentRef(uint32_t attachmentIndex,
                                            VkImageLayout layout);

    // 创建描述符集布局绑定
    VkDescriptorSetLayoutBinding
    makeDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType type,
                                   VkShaderStageFlags stageFlags,
                                   uint32_t count = 1);

    // 创建写描述符集UBO  用于将一个 Uniform Buffer（如 MVP、颜色、灯光等）
    VkWriteDescriptorSet
    makeWriteDescriptorSetUBO(VkDescriptorSet dstSet, uint32_t binding,
                              const VkDescriptorBufferInfo *bufferInfo);

    // 创建写描述符集UBO  用于将纹理（ImageView + Sampler）
    VkWriteDescriptorSet
    makeWriteDescriptorSetSampler(VkDescriptorSet dstSet, uint32_t binding,
                                  const VkDescriptorImageInfo *imageInfo);

    // 创建命令缓冲区记录 并绑定 开始单次命令
    VkCommandBuffer beginSingleTimeCommands();

    // 停止命令缓冲区记录 结束单次命令
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    // 查找内存类型
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);

    // 查找支持的格式
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    // 检查格式是否包含模板组件
    bool hasStencilComponent(VkFormat format);

    // 查找深度格式
    VkFormat findDepthFormat();

    // 获取最大可用采样数
    VkSampleCountFlagBits getMaxUsableSampleCount();

    // 创建分配命令缓冲区
    void createCommandBuffers();

    // 创建同步对象
    void createSyncObjects();

    // 命令缓冲区记录
    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                             uint32_t imageIndex);

    // 清理交换链
    void cleanupSwapChain();

    // 重新创建交换链
    void recreateSwapChain();

    // 渲染和呈现
    void drawFrame();

    // 更新uniform缓冲区
    void updateUniformBuffer(uint32_t currentImage);

    // 创建着色器模块
    VkShaderModule createShaderModule(const std::vector<char> &code);

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

    // 找到合适的物理设备队列族索引
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

    // glfw 窗口大小函数回调
    static void framebufferResizeCallback(GLFWwindow *window, int width,
                                          int height);

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

    // 窗口表面
    VkSurfaceKHR _surface;

private:
    // vulkan 物理设备信息
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

    // 采样计数
    VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    // vulkan 逻辑设备信息
    VkDevice _device = VK_NULL_HANDLE;

private:
    // 图形队列
    VkQueue _graphicsQueue;

    // 呈现队列
    VkQueue _presentQueue;

private:
    // 交换链
    VkSwapchainKHR _swapChain;

    // VkImage句柄
    std::vector<VkImage> _swapChainImages;

    // 交换表面格式
    VkFormat _swapChainImageFormat;

    // 交换范围
    VkExtent2D _swapChainExtent;

    // 图像视图
    std::vector<VkImageView> _swapChainImageViews;

    // 帧缓冲区
    std::vector<VkFramebuffer> _swapChainFramebuffers;

private:
    // 渲染通道
    VkRenderPass _renderPass;

    // 描述符集布局
    VkDescriptorSetLayout _descriptorSetLayout;

    // 管线布局
    VkPipelineLayout _pipelineLayout;

    // 渲染管线
    VkPipeline _graphicsPipeline;

private:
    // 命令池
    VkCommandPool _commandPool;

private:
    // 多重采样颜色图像
    VkImage _colorImage;

    // 多重采样颜色图像内存
    VkDeviceMemory _colorImageMemory;

    // 多重采样颜色图像视图
    VkImageView _colorImageView;

private:
    // 深度图像
    VkImage _depthImage;

    // 深度图像内存
    VkDeviceMemory _depthImageMemory;

    // 深度图像视图
    VkImageView _depthImageView;

private:
    // 纹理图像 mip 级别
    uint32_t _mipLevels;

    // 纹理图像
    VkImage _textureImage;

    // 纹理图像内存
    VkDeviceMemory _textureImageMemory;

    // 纹理图像视图
    VkImageView _textureImageView;

    // 纹理采样器
    VkSampler _textureSampler;

private:
    // 顶点缓冲区
    VkBuffer _vertexBuffer;

    // 顶点缓冲区内存
    VkDeviceMemory _vertexBufferMemory;

    // 索引缓冲区
    VkBuffer _indexBuffer;

    // 索引缓冲区内存
    VkDeviceMemory _indexBufferMemory;

private:
    // uniform 矩阵缓冲区
    UniformVec _uniformMVP;

    // uniform 颜色透明度缓冲区
    UniformVec _uniformAlphaColor;

private:
    // 描述符池句柄
    VkDescriptorPool _descriptorPool;

    // 描述符集合
    std::vector<VkDescriptorSet> _descriptorSets;

private:
    // 分配命令缓冲区
    std::vector<VkCommandBuffer> _commandBuffers;

private:
    // 当前帧索引
    uint32_t _currentFrame = 0;

    // 栅栏对象
    std::vector<VkFence> _inFlightFences;

    // 图像可用信号量
    std::vector<VkSemaphore> _imageAvailableSemaphores;

    // 渲染完成信号量
    std::vector<VkSemaphore> _renderFinishedSemaphores;

private:
    // 同时处理多少帧的数量
    int _MAX_FRAMES_IN_FLIGHT = 2;

    // 标记是否发生了调整大小的操作
    bool _framebufferResized = false;

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

private:
    const std::string _MODEL_PATH = "Res/Model/viking_room.obj";

    const std::string _TEXTURE_PATH = "Res/Model/viking_room.png";

private:
    // 索引
    std::vector<uint32_t> _indices;

    // 顶点结构：位置 + 颜色 + UV
    std::vector<VerCorTex> _vertices;
};

#endif // !HELLOTRANGLE_H_
