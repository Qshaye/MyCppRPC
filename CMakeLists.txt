cmake_minimum_required(VERSION 3.0)
project(mprpc)

# 带有调试信息的版本，可以进行gdb调试
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_STANDARD 11) 

# 设置项目可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 设置项目库文件的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 添加头文件搜索路径，内层的.h文件都可以直接包含
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

# 设置项目库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(src)
# 使用示例
add_subdirectory(example)
