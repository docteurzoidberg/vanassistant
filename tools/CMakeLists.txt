project(obj2h VERSION 0.1.0 LANGUAGES CXX)

cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

# Paths to the libraries
set(DRZENGINE_LIB_DIR /home/drzoid/dev/drzengine)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include the global include directory + the FBGraphics and SAM include directories
include_directories(${DRZENGINE_LIB_DIR}/include)

# Define the executable source files
add_executable(obj2h obj2h.cpp)

#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/local.cmake)
set(PLATFORM_CXX_FLAGS -O2)   

#move bin to ..
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)