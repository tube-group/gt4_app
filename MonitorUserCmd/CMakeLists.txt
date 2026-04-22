cmake_minimum_required(VERSION 3.10)
project(MonitorUserCmd)

set(SOURCES
    src/main.cpp
    src/Monitor.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}
)

# 查找自有库 higplat
find_library(HIGPLAT_LIB higplat PATHS /usr/local/lib REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        redis++::redis++_static
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        Threads::Threads
        ${HIGPLAT_LIB}           # 自有库 higplat
)
