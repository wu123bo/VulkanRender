#include "VulkanResourceManager.h"

namespace RHI
{

VulkanResourceManager::VulkanResourceManager()
    : _context(nullptr)
    , _framesInFlight(0)
    , _descriptorSetLayout(nullptr)
    , _descriptorPool(nullptr)
    , _defaultSampler(nullptr)
{
}

VulkanResourceManager::~VulkanResourceManager()
{
    Shutdown();
}

bool VulkanResourceManager::Init(VulkanContext *context,
                                 uint32_t framesInFlight)
{
    if (nullptr == context || framesInFlight == 0) {
        return false;
    }

    _context = context;
    _framesInFlight = framesInFlight;

    // ---------- 创建默认 DescriptorSetLayout ----------
    _descriptorSetLayout = new VulkanDescriptorSetLayout();

    VkDescriptorSetLayoutBinding uboBinding = _descriptorSetLayout->Make(
        0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);

    VkDescriptorSetLayoutBinding samplerBinding =
        _descriptorSetLayout->Make(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                   VK_SHADER_STAGE_FRAGMENT_BIT, 1);

    std::vector<VkDescriptorSetLayoutBinding> bindings = {uboBinding,
                                                          samplerBinding};

    bool ret = _descriptorSetLayout->Init(_context->GetVkDevice(), bindings);
    if (!ret) {
        return false;
    }

    // ---------- 创建 DescriptorPool ----------
    _descriptorPool = new VulkanDescriptorPool();
    ret = _descriptorPool->Init(_context->GetVkDevice(), _framesInFlight);
    if (!ret) {
        return false;
    }

    // ---------- 创建默认 Sampler ----------
    _defaultSampler = new VulkanSampler();
    ret = _defaultSampler->Init(_context->GetVkPhysicalDevice(),
                                _context->GetVkDevice());

    return ret;
}

void VulkanResourceManager::Shutdown()
{
    for (auto vb : _vertexBuffers) {
        SDelete(vb);
    }
    _vertexBuffers.clear();

    for (auto ib : _indexBuffers) {
        SDelete(ib);
    }
    _indexBuffers.clear();

    for (auto ub : _uniformBuffers) {
        SDelete(ub);
    }
    _uniformBuffers.clear();

    for (auto tex : _textures) {
        SDelete(tex);
    }
    _textures.clear();

    SDelete(_descriptorPool);
    SDelete(_descriptorSetLayout);
    SDelete(_defaultSampler);

    _context = nullptr;
    _framesInFlight = 0;
}

// -------- Buffer 创建 --------
VulkanVertexBuffer *VulkanResourceManager::CreateVertexBuffer(const void *data,
                                                              uint32_t size,
                                                              uint32_t stride)
{
    if (nullptr == _context || !data || size == 0) {
        return nullptr;
    }

    VulkanVertexBuffer *vb = new VulkanVertexBuffer(
        _context->GetVkPhysicalDevice(), _context->GetVkDevice(),
        _context->GetVkCommandPool(), _context->GetGraphicsQueue(), data, size,
        stride);

    _vertexBuffers.push_back(vb);
    return vb;
}

VulkanIndexBuffer *
VulkanResourceManager::CreateIndexBuffer(const void *data, uint32_t size,
                                         VkIndexType indexType)
{
    if (nullptr == _context || !data || size == 0) {
        return nullptr;
    }

    VulkanIndexBuffer *ib = new VulkanIndexBuffer(
        _context->GetVkPhysicalDevice(), _context->GetVkDevice(),
        _context->GetVkCommandPool(), _context->GetGraphicsQueue(), data, size,
        indexType);

    _indexBuffers.push_back(ib);
    return ib;
}

VulkanUniformBuffer *
VulkanResourceManager::CreateUniformBuffer(uint32_t size, uint32_t binding)
{
    if (nullptr == _context || size == 0) {
        return nullptr;
    }

    VulkanUniformBuffer *ub = new VulkanUniformBuffer();
    ub->Init(_context->GetVkPhysicalDevice(), _context->GetVkDevice(), size,
             binding);

    // 自动更新 DescriptorSet
    UpdateUniformBufferDescriptor(ub, binding);

    _uniformBuffers.push_back(ub);
    return ub;
}

// -------- Texture --------
VulkanTexture *VulkanResourceManager::CreateTexture(const char *filePath,
                                                    bool generateMipmaps)
{
    if (nullptr == _context || !filePath) {
        return nullptr;
    }

    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

    VulkanTexture *tex =
        new VulkanTexture(_context->GetVkPhysicalDevice(),
                          _context->GetVkDevice(), _context->GetVkCommandPool(),
                          _context->GetGraphicsQueue(), filePath, sampleCount);

    // 自动更新 DescriptorSet
    UpdateTextureDescriptor(tex, 1);

    _textures.push_back(tex);
    return tex;
}

// -------- Swapchain 重建 --------
void VulkanResourceManager::OnSwapchainRecreated(uint32_t framesInFlight)
{
    _framesInFlight = framesInFlight;

    for (auto ub : _uniformBuffers) {
        if (ub) {
            // ub->Recreate(_context->GetVkDevice(), framesInFlight);
            UpdateUniformBufferDescriptor(ub, ub->GetBinding());
        }
    }
}

// -------- 私有接口：更新 DescriptorSet --------
void VulkanResourceManager::UpdateUniformBufferDescriptor(
    VulkanUniformBuffer *ub, uint32_t binding)
{
    // if (!_descriptorPool || !_descriptorSetLayout || !ub) {
    //     return;
    // }

    // for (uint32_t i = 0; i < _framesInFlight; ++i) {
    //     VkDescriptorBufferInfo bufferInfo{ub->GetVkBuffer(i), 0,
    //     ub->GetSize()}; VkWriteDescriptorSet write{}; write.sType =
    //     VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; write.dstSet =
    //     _descriptorPool->GetDescriptorSet(i); write.dstBinding = binding;
    //     write.dstArrayElement = 0;
    //     write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //     write.descriptorCount = 1;
    //     write.pBufferInfo = &bufferInfo;
    //     vkUpdateDescriptorSets(_context->GetVkDevice(), 1, &write, 0,
    //     nullptr);
    // }
}

void VulkanResourceManager::UpdateTextureDescriptor(VulkanTexture *tex,
                                                    uint32_t binding)
{
    // if (!_descriptorPool || !_descriptorSetLayout || !tex ||
    // !_defaultSampler)
    //     return;

    // for (uint32_t i = 0; i < _framesInFlight; ++i) {
    //     VkDescriptorImageInfo imageInfo{
    //         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, tex->GetImageView(),
    //         _defaultSampler->GetVkSampler()};

    //    VkWriteDescriptorSet write{};
    //    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //    write.dstSet = _descriptorPool->GetDescriptorSet(i);
    //    write.dstBinding = binding;
    //    write.dstArrayElement = 0;
    //    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //    write.descriptorCount = 1;
    //    write.pImageInfo = &imageInfo;

    //    vkUpdateDescriptorSets(_context->GetVkDevice(), 1, &write, 0,
    //    nullptr);
    //}
}

} // namespace RHI