cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0077 NEW)

project(va_serialtest VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Paths to the libraries
set(DRZENGINE_LIB_DIR /home/drzoid/dev/drzengine)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include the global include directory + the FBGraphics and SAM include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/../../include)

set(SOURCES src/main.cpp)

# Set DRZENGINE library options to not build tests, examples, or documentation
set(BUILD_PGELIB ON)
set(BUILD_FBLIB OFF)
set(BUILD_DOC OFF)
set(BUILD_TEST_APPS OFF)

# Define the executable source files
add_executable(${PROJECT_NAME} ${SOURCES})

# Add the DRZENGINE library as external project
add_subdirectory(${DRZENGINE_LIB_DIR} ${CMAKE_BINARY_DIR}/drzengine_build)


set(PLATFORM_CXX_FLAGS -O2)


# Link the drzenginepge library 
target_link_libraries(${PROJECT_NAME} drzenginepge)

#taken from Moros1138's CMakeLists.txt <3

# OpenGL
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenGL_LIBRARIES} OpenGL::GL)

# X11
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} X11::X11)

# Threads
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
include_directories(${Threads_INCLUDE_DIRS})

find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PNG::PNG)
include_directories(${PNG_INCLUDE_DIRS})

# stdc++fs
target_link_libraries(${PROJECT_NAME} stdc++fs)

# dl, for miniaudio
target_link_libraries(${PROJECT_NAME} dl)

# Set Compiler CXX flags
target_compile_options(${PROJECT_NAME} PRIVATE ${PLATFORM_CXX_FLAGS})

# Set the output name for the other platforms
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME va_serialtest)
