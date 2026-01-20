cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

set(ProName VulkanGui)

project(${ProName})

set(SRC
    ${COMMON}/PrintMsg.h
    ${COMMON}/MacroHead.h

    src/main.cpp

    src/Shader.h
    src/Shader.cpp
    src/HelloTrangle.h
    src/HelloTrangle.cpp

    #src/VkBase/VulkanBase.h
    #src/VkBase/VulkanBase.cpp
    #src/VkBase/VulkanDevice.h
    #src/VkBase/VulkanDevice.cpp
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