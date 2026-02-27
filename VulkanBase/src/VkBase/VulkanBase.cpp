#include "VulkanBase.h"

#include "PrintMsg.h"

#include <chrono>

namespace VKB
{

VulkanBase::VulkanBase()
{
    _instance = new VulkanInstance();
    _surface = new VulkanSurface();
    _physicalDevice = new VulkanPhysicalDevice();
    _device = new VulkanDevice();
    _swapchain = new VulkanSwapchain();
    _renderPass = new VulkanRenderPass();
    _framebuffer = new VulkanFramebuffer();
    _commandPool = new VulkanCommandPool();
    _commandBuffer = new VulkanCommandBuffer();
    _pipelineLayout = new VulkanPipelineLayout();
    _pipeline = new VulkanPipeline();
    _sync = new VulkanSync();

    _shaderModule.resize(2);
    _shaderModule[0] = new VulkanShaderModule();
    _shaderModule[1] = new VulkanShaderModule();

    _vertexBuffer = new VulkanVertexBuffer();
    _indexBuffer = new VulkanIndexBuffer();

    _descriptorSetLayout = new VulkanDescriptorSetLayout();

    _descriptorPool = new VulkanDescriptorPool();
}

VulkanBase::~VulkanBase()
{
    Shutdown();
}

int VulkanBase::InitVulkan(GLFWwindow *window)
{
    if (_initialized) {
        return false;
    }

    if (!createInstance()) {
        return false;
    }

    if (!createSurface(window)) {
        return false;
    }

    _window = window;

    if (!_physicalDevice->Pick(_instance->Get(), _surface->Get())) {
        return false;
    }

    if (!_device->Init(_physicalDevice)) {
        return false;
    }

    glfwGetFramebufferSize(window, &_width, &_height);

    if (!_swapchain->Init(
            _physicalDevice->Get(), _device->Get(), _surface->Get(),
            _physicalDevice->GetGraphicsQueueFamily(),
            _physicalDevice->GetPresentQueueFamily(), _width, _height)) {
        return false;
    }

    if (!_renderPass->Init(_device->Get(), _swapchain)) {
        return false;
    }

    if (!_framebuffer->Init(_device->Get(), _renderPass->Get(),
                            _swapchain->GetImageViews(),
                            _swapchain->GetExtent())) {
        return false;
    }

    if (!_commandPool->Init(_device->Get(),
                            _physicalDevice->GetGraphicsQueueFamily())) {
        return false;
    }

    // =========================
    // 创建 VertexBuffer
    // =========================
    _vertexCount = static_cast<uint32_t>(_vertices.size());
    if (!_vertexBuffer->Init(_physicalDevice->Get(), _device->Get(),
                             _commandPool->Get(), _device->GetGraphicsQueue(),
                             _vertices.data(),
                             sizeof(VerCor) * _vertices.size())) {
        return false;
    }

    // =========================
    // 创建 IndexBuffer
    // =========================
    _indexCount = static_cast<uint32_t>(_indices.size());
    if (!_indexBuffer->Init(_physicalDevice->Get(), _device->Get(),
                            _commandPool->Get(), _device->GetGraphicsQueue(),
                            _indices.data(),
                            sizeof(uint32_t) * _indices.size())) {
        return false;
    }

    if (!_commandBuffer->Init(_device->Get(), _commandPool->Get(),
                              _swapchain->GetImageViewCount())) {
        return false;
    }

    // 创建描述符集布局绑定
    VkDescriptorSetLayoutBinding uboMvpBindind = _descriptorSetLayout->Make(
        0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    VkDescriptorSetLayoutBinding uboColorBindind = _descriptorSetLayout->Make(
        1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    // 描述符数组
    std::vector<VkDescriptorSetLayoutBinding> bindings = {uboMvpBindind,
                                                          uboColorBindind};

    // 描述符集布局创建信息
    _descriptorSetLayout->Init(_device->Get(), bindings);
    std::vector<VkDescriptorSetLayout> setLayouts;
    setLayouts.push_back(_descriptorSetLayout->Get());
    if (!_pipelineLayout->Init(_device->Get(), setLayouts)) {
        return false;
    }

    // =========================
    // 创建 UniformBuffer
    // =========================
    _uniformMVPBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformColorBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _uniformMVPBuffer[i].Init(_physicalDevice->Get(), _device->Get(),
                                  sizeof(MvpMatrix));
        _uniformColorBuffer[i].Init(_physicalDevice->Get(), _device->Get(),
                                    sizeof(AlphaColor));
    }

    // 描述符池创建信息
    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // 两个 UBO（MVP + Color） 所以 * 2
    poolSizes[0].descriptorCount =
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 2);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (!_descriptorPool->Init(_device->Get(), MAX_FRAMES_IN_FLIGHT,
                               poolSizes)) {
        return false;
    }

    // 分配描述符集合信息
    std::vector<VkDescriptorSetLayout> layouts(
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
        _descriptorSetLayout->Get());

    if (!_descriptorPool->AllocateDescriptorSets(layouts, _descriptorSets)) {
        return false;
    }

    // 配置描述符类
    VulkanDescriptorSet descriptorSet;
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        // 配置描述符
        descriptorSet.Init(_device->Get(), _descriptorSets[i]);
        descriptorSet.UpdateBuffer(0, _uniformMVPBuffer[i].Get(),
                                   _uniformMVPBuffer[i].GetSize());

        descriptorSet.Init(_device->Get(), _descriptorSets[i]);
        descriptorSet.UpdateBuffer(1, _uniformColorBuffer[i].Get(),
                                   _uniformColorBuffer[i].GetSize());
    }

    _shaderModule[0]->Init(_device->Get(), "Res\\Shaders\\VerMVPColor.spv",
                           VK_SHADER_STAGE_VERTEX_BIT);
    _shaderModule[1]->Init(_device->Get(), "Res\\Shaders\\frag.spv",
                           VK_SHADER_STAGE_FRAGMENT_BIT);

    if (!_pipeline->Init(_device->Get(), _renderPass->Get(),
                         _pipelineLayout->Get(), _shaderModule,
                         _swapchain->GetExtent())) {
        return false;
    }

    if (!_sync->Init(_device->Get())) {
        return false;
    }

    _initialized = true;
    return true;
}

int VulkanBase::DrawFrame()
{
    // 1. 等待上一帧
    const auto &inFlightFence = _sync->GetInFlightFence(_currentFrame);
    vkWaitForFences(_device->Get(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    // 2. 获取 Swapchain Image
    uint32_t imageIndex;
    const auto &imageAvailable = _sync->GetImageAvailable(_currentFrame);
    VkResult result =
        vkAcquireNextImageKHR(_device->Get(), _swapchain->Get(), UINT64_MAX,
                              imageAvailable, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return 0;
    }

    updateUniformBuffer(_currentFrame);

    vkResetFences(_device->Get(), 1, &inFlightFence);

    // 4. reset + record 当前帧的 CommandBuffer
    VkCommandBuffer cb = _commandBuffer->Get(_currentFrame);
    vkResetCommandBuffer(cb, 0);

    // 录制 CommandBuffer 绘制
    _commandBuffer->Record(
        _currentFrame, _renderPass->Get(), _framebuffer->Get()[imageIndex],
        _swapchain->GetExtent(), _pipeline->Get(), _pipelineLayout->Get(),
        _descriptorSets, _vertexBuffer->Get(), _indexBuffer->Get(),
        _indexCount);

    // 3. 提交 CommandBuffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {imageAvailable};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    const auto &commandBuffer = _commandBuffer->Get(_currentFrame);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    const auto &renderFinished = _sync->GetRenderFinished(_currentFrame);
    VkSemaphore signalSemaphores[] = {renderFinished};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(_device->GetGraphicsQueue(), 1, &submitInfo, inFlightFence);

    // 4. Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    VkSwapchainKHR swapchains[] = {_swapchain->Get()};
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_device->GetPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
        || _framebufferResized) {
        _framebufferResized = false;
        recreateSwapchain();
    }

    // 前进到下一帧
    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    return 0;
}

void VulkanBase::Shutdown()
{
    /* 销毁顺序不能乱*/
    if (!_initialized)
        return;

    if (_device && _device->Get() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(_device->Get());
    }

    cleanupSwapchain();

    SDelete(_sync);

    SDelete(_vertexBuffer);
    SDelete(_indexBuffer);

    _uniformMVPBuffer.clear();
    _uniformColorBuffer.clear();

    SDelete(_descriptorSetLayout);
    SDelete(_descriptorPool);
    _descriptorSets.clear();

    SDelete(_pipeline);
    SDelete(_pipelineLayout);
    SDelete(_renderPass);

    for (auto *shader : _shaderModule) {
        SDelete(shader);
    }
    _shaderModule.clear();

    SDelete(_commandPool);
    SDelete(_device);
    SDelete(_physicalDevice);
    SDelete(_surface);
    SDelete(_instance);

    _initialized = false;
}

bool VulkanBase::createInstance()
{
    return _instance->Create();
}

bool VulkanBase::createSurface(GLFWwindow *window)
{
    return _surface->Create(_instance->Get(), window);
}

void VulkanBase::recreateSwapchain()
{
    vkDeviceWaitIdle(_device->Get()); // 等待 GPU 空闲

    // 获取最新窗口大小
    glfwGetFramebufferSize(_window, &_width, &_height);
    if (_width == 0 || _height == 0) {
        return; // 最小化时跳过
    }

    cleanupSwapchain();

    // 重新创建 Swapchain
    _swapchain = new VulkanSwapchain();
    _swapchain->Init(_physicalDevice->Get(), _device->Get(), _surface->Get(),
                     _physicalDevice->GetGraphicsQueueFamily(),
                     _physicalDevice->GetPresentQueueFamily(), _width, _height);

    // 重新创建 Framebuffer
    _framebuffer = new VulkanFramebuffer();
    _framebuffer->Init(_device->Get(), _renderPass->Get(),
                       _swapchain->GetImageViews(), _swapchain->GetExtent());

    // 重新创建 CommandBuffer
    _commandBuffer = new VulkanCommandBuffer();
    _commandBuffer->Init(_device->Get(), _commandPool->Get(),
                         _swapchain->GetImageViewCount());
}

void VulkanBase::updateUniformBuffer(uint32_t currentImage)
{

    // 计算自渲染开始以来以秒为单位的时间
    static auto startTime = std::chrono::high_resolution_clock::now();

    // 当前时间
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     currentTime - startTime)
                     .count();

    /**************更新 MVP ubo***************************/

    MvpMatrix ubo{};
    // 模型矩阵沿Z轴每秒旋转90°
    ubo.model = glm::rotate(MAT_4(1.0f), time * glm::radians(90.0f),
                            PTF_3D(0.0f, 0.0f, 1.0f));

    // ubo.model = MAT_4(1.0f);

    // 视图矩阵
    ubo.view = glm::lookAt(PTF_3D(2.0f, 2.0f, 2.0f), PTF_3D(0.0f),
                           PTF_3D(0.0f, 0.0f, 1.0f));

    const auto &extent = _swapchain->GetExtent();

    // 投影矩阵
    float sceen = (float)extent.width / (float)extent.height;
    ubo.proj = glm::perspective(glm::radians(45.0f), sceen, 0.1f, 10.0f);

    // glm裁剪坐标Y是反的
    ubo.proj[1][1] *= -1;

    // 拷贝数据到缓冲区内存
    _uniformMVPBuffer[currentImage].Update(&ubo, sizeof(MvpMatrix));

    /**************更新 颜色透明度 ubo***************************/
    static bool firstCall = true;
    if (firstCall) {
        std::srand(
            static_cast<unsigned int>(std::time(nullptr))); // 只初始化一次
        firstCall = false;
    }

    AlphaColor colorUbo{};
    colorUbo.color = glm::vec3(1.0f, 1.0f, 1.0f);
    colorUbo.alpha = static_cast<float>(std::rand()) / RAND_MAX;

    _uniformColorBuffer[currentImage].Update(&colorUbo, sizeof(AlphaColor));
}

void VulkanBase::cleanupSwapchain()
{
    // 销毁旧资源
    SDelete(_framebuffer);
    SDelete(_commandBuffer);
    SDelete(_swapchain);
}

} // namespace VKB
