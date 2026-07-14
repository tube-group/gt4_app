cmake_minimum_required(VERSION 3.10)
project(CommL3GaussDB)

# 查找源文件
# file(GLOB SOURCES "src/*.cpp")
set(SOURCES
    src/main.cpp
    src/L2RcvL3.cpp
    src/L2SndL3.cpp
    src/workthread.cpp
    src/handleRequestOrderData.cpp
    src/handleMMWE_TQMTIWEA13.cpp
)

set(GAUSS_SDK_DIR "${CMAKE_SOURCE_DIR}/third_party/gauss_sdk")
execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.so.6
    OUTPUT_VARIABLE LIBSTDCXX_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT EXISTS "${LIBSTDCXX_PATH}")
    message(FATAL_ERROR "无法定位编译器运行时 libstdc++.so.6: ${LIBSTDCXX_PATH}")
endif()

get_filename_component(COMPILER_RUNTIME_DIR "${LIBSTDCXX_PATH}" DIRECTORY)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 编译选项
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# 添加头文件包含路径
target_include_directories(${PROJECT_NAME}
    BEFORE
    PRIVATE
        ${GAUSS_SDK_DIR}/include # GaussDB SDK 头文件路径，优先于系统 libpq 头
        ${CMAKE_CURRENT_SOURCE_DIR}/include # 本地头文件路径
        ${COMMON_INCLUDE_DIR} # 公共头文件路径
)

# 查找自有库 higplat
find_library(HIGPLAT_LIB higplat PATHS /usr/local/lib REQUIRED)
find_library(GAUSS_PQ_LIB pq PATHS ${GAUSS_SDK_DIR}/lib NO_DEFAULT_PATH REQUIRED)

# 5. 统一使用 PRIVATE 关键字进行链接，防止统一性冲突
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_DL_LIBS}
        spdlog::spdlog           # spdlog库
        redis++::redis++  # redis++库
        nlohmann_json::nlohmann_json # nlohmann_json库
        cpr::cpr                 # cpr库
        httplib::httplib         # httplib库
        anl                      # cpp-httplib 开启了异步DNS解析，需要链接anl库
        Threads::Threads
        ${GAUSS_PQ_LIB}          # GaussDB SDK 提供的 libpq
        ${HIGPLAT_LIB}           # 自有库 higplat
)

if(UNIX AND NOT APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "${COMPILER_RUNTIME_DIR};${GAUSS_SDK_DIR}/lib"
        INSTALL_RPATH "${COMPILER_RUNTIME_DIR};${GAUSS_SDK_DIR}/lib"
    )
endif()
