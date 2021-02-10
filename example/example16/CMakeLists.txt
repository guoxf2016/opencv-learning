cmake_minimum_required(VERSION 3.10)
project(example5)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})

# 将源代码添加到此项目的可执行文件。
add_executable (example12 "example1.cpp" "example1.h")
target_link_libraries(example12 PRIVATE ${OpenCV_LIBS})