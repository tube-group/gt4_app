cmake_minimum_required(VERSION 3.10)
project(gt4_app)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 统一使用 find_package 查找所有第三方库
find_package(libpqxx REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(hiredis REQUIRED)
find_package(redis++ REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SOCI REQUIRED COMPONENTS core postgresql)

# 设置公共头文件路径变量
set(COMMON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/common_include")

# 添加所有子项目
add_subdirectory(TubeTrack)
add_subdirectory(FormDemo)


message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build 目录: ${CMAKE_BINARY_DIR}")
message(STATUS "可执行文件输出目录: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

