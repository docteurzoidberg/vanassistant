cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0077 NEW)

project(testproject VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

# Paths to the libraries
set(DRZENGINE_LIB_DIR /home/drzoid/dev/drzengine)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Set the platform to X11 if not specified
if(NOT DEFINED PLATFORM)
    set(PLATFORM "LINUX_X11")
endif()

# Platform-determined backend
if(DEFINED PLATFORM)
    if(${PLATFORM} STREQUAL "LINUX_X11")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "PI_X11")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "WASM")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "WIN")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "BUILDROOT")
        set(BACKEND "PGEFB")
    elseif(${PLATFORM} STREQUAL "LINUX_FB")
        set(BACKEND "PGEFB")
    elseif(${PLATFORM} STREQUAL "PI_FB")
        set(BACKEND "PGEFB")
    endif()
else()
    message(FATAL_ERROR "Please define the PLATFORM option (e.g., -DPLATFORM=PIOS / -DPLATFORM=WSL / -DPLATFORM=WASM / -DPLATFORM=BUILDROOT)")
endif()

# Include the global include directory + the FBGraphics and SAM include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${DRZENGINE_LIB_DIR}/include)

# Others backend specific source files
if(${BACKEND} STREQUAL "PGE")
    message(STATUS "Building for PGE backend")
elseif(${BACKEND} STREQUAL "PGEFB")
    message(STATUS "Building for PGEFB backend")
else()
    if(NOT DEFINED BACKEND)
        message(FATAL_ERROR "BACKEND is undefined, This should be automatically set when the right platform is set, or should be command-line provided (e.g., -DBACKEND=PGE or -DBACKEND=PGEFB)")
    else()
        message(FATAL_ERROR "Invalid backend: ${BACKEND}")
    endif()
endif()

# add src/*.cpp files to SOURCES
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the main.cpp file to the SOURCES list
list(APPEND SOURCES main.cpp)

#add widget.cpp files to SOURCES
#list(APPEND SOURCES ../../src/vanassistant/Widget.cpp)

# Set DRZENGINE library options to not build tests, examples, or documentation
set(BUILD_PGEFBLIB OFF)
set(BUILD_PGELIB OFF)
set(BUILD_FBLIB OFF)
set(BUILD_DOC OFF)
set(BUILD_TEST_APPS OFF)

# Then according to the backend, set which library to build
if(${BACKEND} STREQUAL "PGE")
    set(BUILD_PGELIB ON)
elseif(${BACKEND} STREQUAL "PGEFB")
    set(BUILD_PGEFBLIB ON)
else()
    message(FATAL_ERROR "Invalid backend for the DRZENGINE library: ${BACKEND}")
endif()

# Define the executable source files
add_executable(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${SOURCES})

# Add the DRZENGINE library as external project
add_subdirectory(${DRZENGINE_LIB_DIR} ${CMAKE_BINARY_DIR}/drzengine_build)

# Select platform libraries and flags
if(${PLATFORM} STREQUAL "LINUX_X11")

    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/local.cmake)
    set(PLATFORM_CXX_FLAGS -O2)   

    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} drzenginepge)

    #taken from Moros1138's CMakeLists.txt <3

    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${OpenGL_LIBRARIES} OpenGL::GL)

    # X11
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} X11::X11)

    # Threads
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})

    find_package(PNG REQUIRED)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})

    # stdc++fs
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} stdc++fs)

    # dl, for miniaudio
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} dl)



elseif(${PLATFORM} STREQUAL "WASM")
    #set(PLATFORM_LIBS pthread dl embind)
    set(PLATFORM_CXX_FLAGS -O2 -pthread)
    
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/emscripten.cmake)

    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} pthread)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} dl)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} embind)

elseif(${PLATFORM} STREQUAL "WIN")

    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/mingw64.cmake)
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/local.cmake

    #set(PLATFORM_LIBS mingw32 SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer SDL2_net SDL2_gfx pthread)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} drzenginepge)

    #taken from Moros1138's CMakeLists.txt <3
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${OpenGL_LIBRARIES} OpenGL::GL)

    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} -mwindows -municode)

    # GDI+
    set(GDIPLUS_LIBRARY gdiplus)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${GDIPLUS_LIBRARY})
    
    # Shlwapi
    set(SHLWAPI_LIBRARY shlwapi)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${SHLWAPI_LIBRARY})
    
    # Dwmapi
    set(DWMAPI_LIBRARY dwmapi)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${DWMAPI_LIBRARY})

    # stdc++fs
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} stdc++fs)

    #gcc and stdc++ static linking
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} -static -static-libgcc -static-libstdc++)

elseif(${PLATFORM} STREQUAL "BUILDROOT") 
    #use provided CMAKE_TOOLCHAIN_FILE from buildroot's build output
    #cmake -DCMAKE_TOOLCHAIN_FILE=/home/drzoid/rpi-zero-minimal-buildroot/build_workdir/host/usr/share/buildroot/toolchainfile.cmake
    
    add_compile_definitions(USE_FBGPLATFORM)  

    if (DEFINED CMAKE_SYSROOT)
        message(STATUS "Cross-compiling with sysroot = ${CMAKE_SYSROOT}")
        # S’assurer que CMake cherche dans ${CMAKE_SYSROOT}…
        set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
        # …mais seulement pour LIB et INCLUDE (pas pour les binaires hôtes)
        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
        # Pour que find_package voit aussi /usr de votre sysroot
        list(APPEND CMAKE_PREFIX_PATH   ${CMAKE_SYSROOT}/usr)
    endif()

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM    NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY    ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE    ONLY)

    find_package(ZLIB  REQUIRED)
    find_package(PNG   REQUIRED)   # recherche du module FindPNG

    target_include_directories(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PRIVATE
        ${ZLIB_INCLUDE_DIRS}
        ${PNG_INCLUDE_DIRS}
    )

    set(PLATFORM_LIBS pthread dl atomic png)
    set(PLATFORM_CXX_FLAGS -O2)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${PLATFORM_LIBS})


        
elseif(${PLATFORM} STREQUAL "PI_FB") 

    add_compile_definitions(USE_FBGPLATFORM)  
    #for cross compiling use CMAKE_TOOLCHAIN_FILE !
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/crosspigcc.cmake)

    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} drzenginepgefb)

    find_package(PNG REQUIRED)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})

    set(PLATFORM_LIBS atomic)
    set(PLATFORM_CXX_FLAGS -O2)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${PLATFORM_LIBS})

    # Threads
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})

    # dl, for miniaudio
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} dl)
        
elseif(${PLATFORM} STREQUAL "PI_X11")

    # Link ATOMIC
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} atomic)

    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${OpenGL_LIBRARIES} OpenGL::GL)

    # X11
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} X11::X11)

    # Threads
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})
    
    # PNG
    find_package(PNG REQUIRED)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})

    # stdc++fs
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} stdc++fs)

    # dl, for miniaudio
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} dl)

else()
    message(FATAL_ERROR "Invalid platform: ${PLATFORM}")
endif()

# Link DrzEngine library
if(${BACKEND} STREQUAL "PGE")
    # Link the drzenginepge library 
    #target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} drzenginepge)
elseif(${BACKEND} STREQUAL "PGEFB")
    # Link the drzenginepgefb library 
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} drzenginepgefb)
endif()

# Set the output file for the project
string(TOLOWER ${PLATFORM} OUTPUTFILE)

# Set Compiler CXX flags
target_compile_options(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PRIVATE ${PLATFORM_CXX_FLAGS})

# Set Project's target compiler properties
if(${PLATFORM} STREQUAL "WASM")
    # Overwrite the output name for the WASM platform and add the necessary flags
    set_target_properties(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PROPERTIES
        LINK_FLAGS "-s WASM=1 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG -s ASSERTIONS=1 -s -pthread -s PTHREAD_POOL_SIZE=5 --shell-file ../wasm/vanassistant_shell.html"
        OUTPUT_NAME "vanassistant.html"
    )
else()
    # Set the output name for the other platforms
    set_target_properties(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.${OUTPUTFILE}")
endif()
