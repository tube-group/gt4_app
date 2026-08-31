cmake_minimum_required(VERSION 3.10)
project(AlarmPublisher)

set(SOURCES
    src/AlarmPublisher.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${COMMON_INCLUDE_DIR}
)

get_target_property(SPDLOG_INCLUDE_DIRS spdlog::spdlog INTERFACE_INCLUDE_DIRECTORIES)
if(SPDLOG_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SPDLOG_INCLUDE_DIRS})
endif()

get_target_property(REDIS_INCLUDE_DIRS redis++::redis++ INTERFACE_INCLUDE_DIRECTORIES)
if(REDIS_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${REDIS_INCLUDE_DIRS})
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        libpqxx::pqxx
        redis++::redis++
        spdlog::spdlog
)