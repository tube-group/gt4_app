cmake_minimum_required(VERSION 3.10)
project(FormDemo)

# 查找源文件
set(SOURCES
    src/subscriber.cpp
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

# 链接 第三方库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        redis++::redis++_static  # redis++库
        Threads::Threads
)
