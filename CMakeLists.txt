cmake_minimum_required(VERSION 3.27)  # 设置CMake最低版本要求[1,2](@ref)
project(TinyRenderer)                    # 定义项目名称[1,5](@ref)


set(CMAKE_CXX_STANDARD 20)  # 设置C++17标准（默认启用）
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 强制要求该标准

#头文件目录
include_directories("third_party")
include_directories("source")
include_directories("example")

add_compile_definitions("TinyRenderer_Root_Path=${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE engine_cpp source/*.cpp source/*.h source/*.hpp)
file(GLOB_RECURSE example_cpp example/*.cpp example/*.h)


add_executable(${PROJECT_NAME} "source/main.cpp" ${engine_cpp} ${example_cpp})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _RELEASE)
endif()