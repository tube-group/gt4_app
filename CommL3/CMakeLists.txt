cmake_minimum_required(VERSION 3.10)
project(CommL3)

# 查找源文件
# file(GLOB SOURCES "src/*.cpp")
set(SOURCES
    src/main.cpp
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

# 定义高斯 SDK 的绝对路径
# set(GAUSS_SDK_DIR "${CMAKE_SOURCE_DIR}/third_party/gauss_sdk")

# =================================================================
# 2. 核心：强制高斯头文件优先级高于 vcpkg
# =================================================================
# 使用 BEFORE 关键字，确保 -I${GAUSS_SDK_DIR}/include 排在 vcpkg 的包含路径之前
target_include_directories(${PROJECT_NAME} BEFORE PRIVATE "${GAUSS_SDK_DIR}/include")

# 强行把高斯的库目录放到链接器搜索路径的最前面
# target_link_directories(${PROJECT_NAME} BEFORE PRIVATE "${GAUSS_SDK_DIR}/lib")

# 5. 统一使用 PRIVATE 关键字进行链接，防止统一性冲突
target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${CMAKE_DL_LIBS}
        libpqxx::pqxx            # PostgreSQL C++库
        spdlog::spdlog           # spdlog库
        redis++::redis++  # redis++库
        nlohmann_json::nlohmann_json # nlohmann_json库
        Threads::Threads
        ${HIGPLAT_LIB}           # 自有库 higplat
        # -lpq # 强制链接，由于上面加了 BEFORE，它会抓到高斯的 libpq.so
)

# =================================================================
# 6. 运行期配置：核心 RPATH 动态拼接
# =================================================================
# 动态获取当前构建目录下 vcpkg 编译输出的动态库绝对路径
# CMAKE_CURRENT_BINARY_DIR 通常对应 build/TubeTrack
# 我们向上一级定位到 build/vcpkg_installed/x64-linux-dynamic/lib
# set(VCPKG_INSTALLED_LIB_DIR "/opt/vcpkg/installed/x64-linux-dynamic/debug/lib")

# =================================================================
# 4. 运行期保障：设置 RPATH
# =================================================================
# 确保程序在 Linux 运行时，优先去高斯 SDK 的 lib 目录加载 libpq.so
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     BUILD_WITH_INSTALL_RPATH TRUE
#     # 用分号 `;` 隔开多个路径：
#     # 路径一：高斯 SDK 目录（让程序优先去这里找 libpq.so）
#     # 路径二：vcpkg 本地安装目录（让程序去这里找 libpqxx 以及 spdlog/redis++ 等动态库）
#     INSTALL_RPATH "${GAUSS_SDK_DIR}/lib;${VCPKG_INSTALLED_LIB_DIR}"
# )
