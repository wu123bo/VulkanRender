#ifndef VULKANTEXTURE_H_
#define VULKANTEXTURE_H_

#include <string>

#include "VulkanBuffer.h"

namespace VKB
{

/**
 * @brief Vulkan 2D 纹理封装类
 *
 * 职责：
 *  - 从文件加载纹理（CPU）
 *  - staging buffer → device local image
 *  - 正确处理 image layout transition
 *  - 创建 ImageView
 *
 * 不负责：
 *  - DescriptorSet 的创建与绑定
 *  - 多 mipmap / array / cubemap（后续可扩展）
 */
class VulkanTexture
{
public:
    VulkanTexture() = default;

    ~VulkanTexture();

    /**
     * @brief 从图片文件初始化纹理
     *
     * @param physicalDevice  物理设备（用于内存类型查找）
     * @param device          逻辑设备
     * @param commandPool     图形命令池（一次性拷贝）
     * @param graphicsQueue   图形队列
     * @param filename        图片路径（如 png / jpg）
     *
     * @return true  创建成功
     * @return false 创建失败
     */
    bool InitFromFile(VkPhysicalDevice physicalDevice, VkDevice device,
                      VkCommandPool commandPool, VkQueue graphicsQueue,
                      const std::string &filename);

    /**
     * @brief 释放所有 Vulkan 资源
     *
     * 销毁顺序：
     *  Sampler → ImageView → Image → DeviceMemory
     */
    void Destroy();

    // =========================
    // Getter
    // =========================

    /// 获取 VkImageView（用于 Descriptor）
    VkImageView GetImageView() const
    {
        return _imageView;
    }

    /// 获取 VkImage（一般不直接暴露使用）
    VkImage GetImage() const
    {
        return _image;
    }

private:
    // =========================
    // 内部辅助函数
    // =========================

    /**
     * @brief 创建 VkImage + 分配并绑定显存
     */
    bool createImage(VkPhysicalDevice physicalDevice, VkDevice device,
                     uint32_t width, uint32_t height, VkFormat format,
                     VkImageUsageFlags usage, VkImage &image,
                     VkDeviceMemory &memory);

    /**
     * @brief Image layout 转换
     *
     * 常用：
     *  UNDEFINED → TRANSFER_DST
     *  TRANSFER_DST → SHADER_READ
     */
    void transitionLayout(VkCommandPool commandPool, VkQueue queue,
                          VkImageLayout oldLayout, VkImageLayout newLayout);

    /**
     * @brief 从 buffer 拷贝数据到 image
     *
     * 用于 staging buffer → image
     */
    void copyFromBuffer(VkCommandPool commandPool, VkQueue queue,
                        VkBuffer buffer, uint32_t width, uint32_t height);

    /**
     * @brief 创建 ImageView
     *
     * 默认：
     *  - 2D
     *  - 单 mip
     *  - COLOR aspect
     */
    void createImageView(VkDevice device, VkFormat format);

private:
    // =========================
    // Vulkan 对象
    // =========================

    VkDevice _device = VK_NULL_HANDLE;

    VkImage _image = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;

    VkImageView _imageView = VK_NULL_HANDLE;
};

} // namespace VKB

#endif // !VULKANTEXTURE_H_
