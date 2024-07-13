cmake_minimum_required(VERSION 3.10)
project(vanassistant)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include backend-specific configurations
if(DEFINED BACKEND)
    #include(${CMAKE_SOURCE_DIR}/cmake/backends/${BACKEND}.cmake)
else()
    message(FATAL_ERROR "Please define the BACKEND variable (e.g., -DBACKEND=PGE or -DBACKEND=FB)")
endif()

# Include platform-specific configurations
if(DEFINED PLATFORM)
    #include(${CMAKE_SOURCE_DIR}/cmake/platforms/${PLATFORM}.cmake)
else()
    message(FATAL_ERROR "Please define the PLATFORM variable (e.g., -DPLATFORM=PIOS / -DPLATFORM=WSL / -DPLATFORM=WASM / -DPLATFORM=BUILDROOT)")
endif()

# Include the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

#Source files, depending on the backend
if(${BACKEND} STREQUAL "PGE")
    set(SOURCES ${SOURCES} src/main_pge.cpp)
elseif(${BACKEND} STREQUAL "FB")
    set(SOURCES ${SOURCES} src/main_fb.cpp)
    #Include the FBG"s source directories
    file(GLOB FBG_SOURCES ${CMAKE_SOURCE_DIR}/src/fb/*.c)
    list(APPEND SOURCES ${FBG_SOURCES})
    file(GLOB FBG_SOURCES ${CMAKE_SOURCE_DIR}/src/fb/lodepng/*.c)
    list(APPEND SOURCES ${FBG_SOURCES})
    file(GLOB FBG_SOURCES ${CMAKE_SOURCE_DIR}/src/fb/nanojpeg/*.c)
    list(APPEND SOURCES ${FBG_SOURCES})
else()
    message(FATAL_ERROR "Invalid backend: ${BACKEND}")
endif()

# Include the sam's source directory
file(GLOB SAM_SOURCES ${CMAKE_SOURCE_DIR}/src/sam/*.c)
list(APPEND SOURCES ${SAM_SOURCES})

# Define the executable
add_executable(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${SOURCES})

#Select platform libs
if(${PLATFORM} STREQUAL "X11")
    set(PLATFORM_LIBS X11 GL pthread png stdc++fs dl)
    set(PLATFORM_CXX_FLAGS -O2)
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/local.cmake)
elseif(${PLATFORM} STREQUAL "BUILDROOT")
    set(PLATFORM_LIBS pthread dl atomic)
    set(PLATFORM_CXX_FLAGS -O2)
    #set(CMAKE_TOOLCHAIN_FILE "/home/drzoid/rpi-zero-minimal-buildroot/build_workdir/host/usr/share/buildroot/toolchainfile.cmake")
elseif(${PLATFORM} STREQUAL "PIOS")
    set(PLATFORM_LIBS pthread dl atomic)
    set(PLATFORM_CXX_FLAGS -O2)
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/crosspigcc.cmake)
elseif(${PLATFORM} STREQUAL "WASM")
    set(PLATFORM_LIBS pthread dl embind)
    set(PLATFORM_CXX_FLAGS -O2 -pthread)
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/emscripten.cmake)
else()
    message(FATAL_ERROR "Invalid platform: ${PLATFORM}")
endif()


#Output file
string(TOLOWER ${PLATFORM} OUTPUTFILE)

# Link libraries and set properties
target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${PLATFORM_LIBS})
target_compile_options(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PRIVATE ${PLATFORM_CXX_FLAGS})
set_target_properties(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PROPERTIES OUTPUT_NAME "vanassistant.${OUTPUTFILE}")

if(${PLATFORM} STREQUAL "WASM")
    set_target_properties(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PROPERTIES
        LINK_FLAGS "-s WASM=1 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG -s ASSERTIONS=1 -s -pthread -s PTHREAD_POOL_SIZE=5 --shell-file ../wasm/shell.html"
        OUTPUT_NAME "vanassistant.html"
    )
endif()

#Post build commands
if(${PLATFORM} STREQUAL "PIOS")
    # Add custom command to copy deploy script to build directory
    add_custom_command(
        TARGET ${PROJECT_NAME}_${BACKEND}_${PLATFORM} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/scripts/deploy_pi.sh
            ${CMAKE_BINARY_DIR}/deploy_pi.sh
        COMMENT "Copying deploy_pi.sh to build directory"
    )
endif()

