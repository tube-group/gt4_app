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
    src/LengthPosition.cpp
    src/CirclePosition.cpp
    src/PositionBase.cpp
    src/ProductionPlan.cpp
    src/Scrapt.cpp
    src/ScraptRoller.cpp
    src/SprayPosition.cpp
    src/WeightPosition.cpp
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

# 链接 第三方库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        libpqxx::pqxx            # PostgreSQL C++库
        spdlog::spdlog           # spdlog库
        redis++::redis++_static  # redis++库
        nlohmann_json::nlohmann_json # nlohmann_json库
        SOCI::soci_postgresql_static
        SOCI::soci_core_static
        Threads::Threads
        ${HIGPLAT_LIB}           # 自有库 higplat
)
