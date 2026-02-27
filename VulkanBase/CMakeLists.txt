cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

set(ProName VulkanBase)

project(${ProName})

set(SRC
    ${COMMON}/PrintMsg.h
    ${COMMON}/MacroHead.h
    ${COMMON}/VulkanHead.h

    src/main.cpp

    src/VkBase/VulkanBase.h
    src/VkBase/VulkanBase.cpp
    src/VkBase/VulkanInstance.h
    src/VkBase/VulkanInstance.cpp
    src/VkBase/VulkanPhysicalDevice.h
    src/VkBase/VulkanPhysicalDevice.cpp
    src/VkBase/VulkanDevice.h
    src/VkBase/VulkanDevice.cpp

    src/VkBase/VulkanSurface.h
    src/VkBase/VulkanSurface.cpp
    src/VkBase/VulkanSwapchain.h
    src/VkBase/VulkanSwapchain.cpp

    src/VkBase/VulkanRenderPass.h
    src/VkBase/VulkanRenderPass.cpp
    src/VkBase/VulkanFramebuffer.h
    src/VkBase/VulkanFramebuffer.cpp
    src/VkBase/VulkanPipelineLayout.h
    src/VkBase/VulkanPipelineLayout.cpp
    src/VkBase/VulkanPipeline.h
    src/VkBase/VulkanPipeline.cpp

    src/VkBase/VulkanCommandPool.h
    src/VkBase/VulkanCommandPool.cpp
    src/VkBase/VulkanCommandBuffer.h
    src/VkBase/VulkanCommandBuffer.cpp

    src/VkBase/VulkanSync.h
    src/VkBase/VulkanSync.cpp

    src/VkBase/VulkanUtils.h
    src/VkBase/VulkanUtils.cpp
    src/VkBase/VulkanShaderModule.h
    src/VkBase/VulkanShaderModule.cpp

    src/VkBase/VulkanBuffer.h
    src/VkBase/VulkanBuffer.cpp
    src/VkBase/VulkanVertexBuffer.h
    src/VkBase/VulkanVertexBuffer.cpp
    src/VkBase/VulkanIndexBuffer.h
    src/VkBase/VulkanIndexBuffer.cpp
    src/VkBase/VulkanUniformBuffer.h
    src/VkBase/VulkanUniformBuffer.cpp

    src/VkBase/VulkanDescriptorPool.h
    src/VkBase/VulkanDescriptorPool.cpp
    src/VkBase/VulkanDescriptorSet.h
    src/VkBase/VulkanDescriptorSet.cpp
    src/VkBase/VulkanDescriptorSetLayout.h
    src/VkBase/VulkanDescriptorSetLayout.cpp
)

add_executable(${ProName} ${SRC})

target_include_directories(${ProName} 
    PRIVATE
        ${COMMON}
        ${THIRD_PARTY_DIR}/
        ${THIRD_PARTY_DIR}/glm
        ${THIRD_PARTY_DIR}/gli
        ${THIRD_PARTY_DIR}/stb
        ${THIRD_PARTY_DIR}/imgui
        ${THIRD_PARTY_DIR}/tinyobj
        ${THIRD_PARTY_DIR}/tinygltf
        ${THIRD_PARTY_DIR}/ktx/include
        ${THIRD_PARTY_DIR}/glfw/include
        ${THIRD_PARTY_DIR}/ktx/other_include
)

target_link_libraries(${ProName}
    PRIVATE
        glfw
        ${THIRD_PARTY_DIR}/libs/vulkan/vulkan-1.lib
)

file(GLOB COMSRC ${COMMON}/*.h* ${COMMON}/*.cpp ${COMMON}/*.hpp)

file(GLOB VkCore src/VkBase/VulkanBase.* src/VkBase/VulkanInstance.* src/VkBase/VulkanPhysicalDevice.* src/VkBase/VulkanDevice.*)
file(GLOB VkWindow src/VkBase/VulkanSurface.* src/VkBase/VulkanSwapchain.*)
file(GLOB VkRender src/VkBase/VulkanRenderPass.* src/VkBase/VulkanFramebuffer.* src/VkBase/VulkanPipelineLayout.* src/VkBase/VulkanPipeline.*)
file(GLOB VkCommand src/VkBase/VulkanCommandPool.* src/VkBase/VulkanCommandBuffer.*)
file(GLOB VkSync src/VkBase/VulkanSync.*)
file(GLOB VkUtils src/VkBase/VulkanUtils.*)

source_group(Common FILES ${COMSRC})
source_group(VkCore FILES ${VkCore})
source_group(VkWindow FILES ${VkWindow})
source_group(VkRender FILES ${VkRender})
source_group(VkCommand FILES ${VkCommand})
source_group(VkSync FILES ${VkSync})
source_group(VkUtils FILES ${VkUtils})
