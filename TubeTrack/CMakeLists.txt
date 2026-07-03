cmake_minimum_required(VERSION 3.10)
project(TubeTrack)

# 查找源文件
# file(GLOB SOURCES "src/*.cpp")
set(SOURCES
    src/main.cpp
    src/Tube.cpp
    src/Basket.cpp
    src/BackBuffer.cpp
    src/CarvePosition.cpp
    src/AlignPosition.cpp
    src/CirclePosition.cpp
    src/PositionBase.cpp
    src/ProductionPlan.cpp
    src/Scrapt.cpp
    src/ScraptRoller.cpp
    src/SprayPosition.cpp
    src/WeightPosition.cpp
    src/WalkingBeam.cpp
    src/workthread.cpp
    src/statisticsthread.cpp
    src/DoStatistics.cpp
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

# # 强行把高斯官方的 libpq 路径加入到链接和运行期搜索路径 (RPATH) 的最前面
# set(GAUSS_LIB_DIR "${CMAKE_SOURCE_DIR}/GaussDB_lib")

# # 2. 强行把高斯官方的 libpq 路径加入到链接目录的最前面
# target_link_directories(${PROJECT_NAME} BEFORE PRIVATE ${GAUSS_LIB_DIR})

# 链接 第三方库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        AlarmPublisher
        # 核心改动：去掉 -lpq 和 target_link_directories
        # 直接把高斯 libpq.so 的绝对路径喂给链接器，不给它任何猜的机会
        # ${GAUSS_LIB_DIR}/libpq.so
        libpqxx::pqxx            # PostgreSQL C++库
        spdlog::spdlog           # spdlog库
        redis++::redis++  # redis++库
        nlohmann_json::nlohmann_json # nlohmann_json库
        Threads::Threads
        ${HIGPLAT_LIB}           # 自有库 higplat
)

# # 3. 设置 RPATH，确保程序运行时优先去 Gauss 目录找 libpq.so，而不是去 vcpkg 目录
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     BUILD_WITH_INSTALL_RPATH TRUE
#     INSTALL_RPATH "${GAUSS_LIB_DIR}"
# )
