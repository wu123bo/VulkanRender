cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

set(ProName VulkanEngine)

project(${ProName})

# 设置源代码目录
set(VULKAN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/RHI/RHI_Vulkan)

# 自动扫描XXX下的所有 cpp 和 h 文件
file(GLOB_RECURSE VULKANSRC
    "${VULKAN_DIR}/*.h"
    "${VULKAN_DIR}/*.cpp"
)

set(SRC
    ${COMMON}/PrintMsg.h
    ${COMMON}/MacroHead.h
    ${COMMON}/VulkanHeadRHI.h

    src/main.cpp
    src/WindowHelper.h

    src/Renderer.h
    src/Renderer.cpp

    src/RHI/IRHI.h
    src/RHI/SurfaceRHI.h
    src/RHI/FactoryRHI.h
    src/RHI/FactoryRHI.cpp

    src/RHI/VulkanRHI.h
    src/RHI/VulkanRHI.cpp

    src/RHI/OpenGLRHI.h
    src/RHI/OpenGLRHI.cpp
    ${VULKANSRC}
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

source_group(Common FILES ${COMSRC})
source_group(RHI REGULAR_EXPRESSION ".*/RHI/.*")
source_group(Render REGULAR_EXPRESSION ".*/Render/.*")

source_group(RHI_Vulkan FILES ${VULKANSRC})