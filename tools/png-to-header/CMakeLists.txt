cmake_minimum_required(VERSION 3.10)
project(png2h)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PNG REQUIRED)

add_executable(png2h main.cpp)

target_include_directories(png2h PRIVATE ${PNG_INCLUDE_DIRS})
target_link_libraries(png2h PRIVATE ${PNG_LIBRARIES})

