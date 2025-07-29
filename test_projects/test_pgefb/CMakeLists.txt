cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0077 NEW)

project(testpgefb VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -O2)

set(CMAKE_C_STANDARD 99)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include the global include directory + the FBGraphics and SAM include directories
include_directories(${CMAKE_SOURCE_DIR}/../../include)
include_directories(${CMAKE_SOURCE_DIR}/../../../drzengine/lib/fb/include)

#set fbgraphics sources from lib/fb/src
set(FBGRAPHICS_LIB_DIR ${CMAKE_SOURCE_DIR}/../../../drzengine/lib/fb)

#Include the additional FBG's source directories
file(GLOB FBG_SOURCES ${FBGRAPHICS_LIB_DIR}/src/*.c)
file(GLOB FBG_SOURCES_LODEPNG ${FBGRAPHICS_LIB_DIR}/src/lodepng/*.c)
file(GLOB FBG_SOURCES_NANOJPEG ${FBGRAPHICS_LIB_DIR}/src/nanojpeg/*.c)

set(SOURCES main.cpp)

list(APPEND SOURCES ${FBG_SOURCES})
list(APPEND SOURCES ${FBG_SOURCES_LODEPNG})
list(APPEND SOURCES ${FBG_SOURCES_NANOJPEG})

# Define the executable source files
add_executable(${PROJECT_NAME} ${SOURCES})

# Threads
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
include_directories(${Threads_INCLUDE_DIRS})

find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PNG::PNG)
include_directories(${PNG_INCLUDE_DIRS})

# stdc++fs
target_link_libraries(${PROJECT_NAME} stdc++fs)

