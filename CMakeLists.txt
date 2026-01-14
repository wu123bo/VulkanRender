cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

set(ProName VulkanRender)

project(${ProName})

# ---------------------------
# C++ 标准设置
# ---------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------
# 输出路径统一
# ---------------------------
# 可执行文件输出到 Bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# 动态库输出到 Bin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# 静态库输出到 lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------------------------
# 项目选项
# ---------------------------
option(USE_OPENMP "启用 OpenMP" ON)
option(PROJECT_VULKANGUI "启用 VulkanGui" ON)

# ---------------------------
# 平台检测 
# ---------------------------
if (WIN32)
    message("当前平台：Windows")
    add_compile_definitions(_OS_WIN_)
elseif(UNIX)
    message("当前平台：Linux/Unix")
    add_compile_definitions(_OS_LINUX_)
else()
    message(FATAL_ERROR "不支持的操作系统平台！")
endif()

# ---------------------------
# OpenMP 检测
# ---------------------------
if (USE_OPENMP)
    find_package(OpenMP)
    if (OpenMP_CXX_FOUND)
        add_compile_definitions(USE_OPENMP)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        message(STATUS "OpenMP enabled")
    else()
        message(WARNING "OpenMP not found")
    endif()
endif()

# ---------------------------
# 第三方公共头文件库路径 
# ---------------------------
set(COMMON ${CMAKE_SOURCE_DIR}/Common)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/3rd)


# ---------------------------
# 子模块
# ---------------------------
if (PROJECT_VULKANGUI)
    add_subdirectory(3rd/glfw)
    add_subdirectory(VulkanGui)
endif()