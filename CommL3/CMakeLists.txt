cmake_minimum_required(VERSION 3.10)
project(CommL3)

# 查找源文件
# file(GLOB SOURCES "src/*.cpp")
set(SOURCES
    src/main.cpp
    src/L2RcvL3.cpp
    src/L2SndL3.cpp
    src/workthread.cpp
)

# 创建可执行文件 
add_executable(${PROJECT_NAME} ${SOURCES})

# 编译选项
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# 添加头文件包含路径
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include # 本地头文件路径
        ${COMMON_INCLUDE_DIR} # 公共头文件路径
)

# 查找自有库 higplat
find_library(HIGPLAT_LIB higplat PATHS /usr/local/lib REQUIRED)                                                          

# 5. 统一使用 PRIVATE 关键字进行链接，防止统一性冲突
target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${CMAKE_DL_LIBS}
        spdlog::spdlog           # spdlog库
        redis++::redis++  # redis++库
        nlohmann_json::nlohmann_json # nlohmann_json库
        cpr::cpr                 # cpr库
        httplib::httplib         # httplib库
        Threads::Threads
        ${HIGPLAT_LIB}           # 自有库 higplat
)
