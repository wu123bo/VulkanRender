#include "VulkanBase.h"

#include "PrintMsg.h"

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
}

VulkanBase::~VulkanBase()
{
    Shutdown();
}

int VulkanBase::InitVulkan(GLFWwindow *window)
{
    if (_initialized)
        return true;

    if (!createInstance())
        return false;
    if (!createSurface(window))
        return false;

    _window = window;

    if (!_physicalDevice->Pick(_instance->Get(), _surface->Get()))
        return false;
    if (!_device->Init(_physicalDevice))
        return false;

    glfwGetFramebufferSize(window, &_width, &_height);

    if (!_swapchain->Init(
            _physicalDevice->Get(), _device->Get(), _surface->Get(),
            _physicalDevice->GetGraphicsQueueFamily(),
            _physicalDevice->GetPresentQueueFamily(), _width, _height))
        return false;

    if (!_renderPass->Init(_device->Get(), _swapchain))
        return false;
    if (!_framebuffer->Init(_device->Get(), _renderPass->Get(),
                            _swapchain->GetImageViews(),
                            _swapchain->GetExtent()))
        return false;
    if (!_commandPool->Init(_device->Get(),
                            _physicalDevice->GetGraphicsQueueFamily()))
        return false;
    if (!_commandBuffer->Init(_device->Get(), _commandPool->Get(),
                              _swapchain->GetImageViewCount()))
        return false;

    if (!_pipelineLayout->Init(_device->Get()))
        return false;

    _shaderModule[0]->Init(_device->Get(), "Res\\Shaders\\vert.spv",
                           VK_SHADER_STAGE_VERTEX_BIT);
    _shaderModule[1]->Init(_device->Get(), "Res\\Shaders\\frag.spv",
                           VK_SHADER_STAGE_FRAGMENT_BIT);

    if (!_pipeline->Init(_device->Get(), _renderPass->Get(),
                         _pipelineLayout->Get(), _shaderModule,
                         _swapchain->GetExtent()))
        return false;

    if (!_sync->Init(_device->Get()))
        return false;

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

    vkResetFences(_device->Get(), 1, &inFlightFence);

    // 4. reset + record 当前帧的 CommandBuffer
    VkCommandBuffer cb = _commandBuffer->Get(_currentFrame);
    vkResetCommandBuffer(cb, 0);

    // 录制 CommandBuffer（直接用 gl_VertexIndex，不绑定 VertexBuffer）
    _commandBuffer->Record(_currentFrame, _renderPass->Get(),
                           _framebuffer->Get()[imageIndex],
                           _swapchain->GetExtent(), _pipeline->Get());

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

void VulkanBase::cleanupSwapchain()
{
    // 销毁旧资源
    SDelete(_framebuffer);
    SDelete(_commandBuffer);
    SDelete(_swapchain);
}

} // namespace VKB
