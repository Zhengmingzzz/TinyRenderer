cmake_minimum_required(VERSION 3.27)  # 设置CMake最低版本要求
project(TinyRenderer)                    # 定义项目名称[1,5](@ref)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 强制要求该标准


add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

#头文件目录
include_directories("third_party")
include_directories("source")
include_directories("example")

find_package(rttr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)


add_compile_definitions("Engine_Root_Path=${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE engine_cpp source/*.cpp source/*.h source/*.hpp)
file(GLOB_RECURSE example_cpp example/*.cpp example/*.h)

add_executable(${PROJECT_NAME} "source/main.cpp" ${engine_cpp} ${example_cpp})

target_link_libraries(${PROJECT_NAME} PRIVATE
        RTTR::Core
        nlohmann_json::nlohmann_json

)

# 按照构建要求定义_DEBUG和_RELEASE宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _RELEASE)
endif()