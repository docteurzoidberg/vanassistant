cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0077 NEW)

project(vanassistant VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Paths to the libraries
set(DRZENGINE_LIB_DIR /home/drzoid/dev/drzengine)
set(FBGRAPHICS_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/fb)
set(SAM_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/sam)

# Some random paths:
set(BUILDROOT_COPY_DEST_DIR /home/drzoid/rpi-zero-minimal-buildroot/vanassistant_overlay/opt/vanassistant/)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Set the platform to X11 if not specified
if(NOT DEFINED PLATFORM)
    set(PLATFORM "X11")
endif()

# Platform-determined backend
if(DEFINED PLATFORM)
    if(${PLATFORM} STREQUAL "X11")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "WASM")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "WIN")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "BUILDROOT")
        set(BACKEND "FB")
    elseif(${PLATFORM} STREQUAL "PIOS")
        #set(BACKEND "FB")
    endif()
else()
    message(FATAL_ERROR "Please define the PLATFORM option (e.g., -DPLATFORM=PIOS / -DPLATFORM=WSL / -DPLATFORM=WASM / -DPLATFORM=BUILDROOT)")
endif()

# Include the global include directory + the FBGraphics and SAM include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${FBGRAPHICS_LIB_DIR}/include ${SAM_LIB_DIR}/include)

# Others backend specific source files
if(${BACKEND} STREQUAL "PGE")
    #Include the olc::pge multiplatform entry point
    set(SOURCES src/main_pge.cpp)
elseif(${BACKEND} STREQUAL "FB")
    #Include the framebuffer's linux only entry point
    set(SOURCES src/main_fb.cpp)

    #Include the additional FBG's source directories
    file(GLOB FBG_SOURCES ${FBGRAPHICS_LIB_DIR}/src/*.c)
    file(GLOB FBG_SOURCES_LODEPNG ${FBGRAPHICS_LIB_DIR}/src/lodepng/*.c)
    file(GLOB FBG_SOURCES_NANOJPEG ${FBGRAPHICS_LIB_DIR}/src/nanojpeg/*.c)

    list(APPEND SOURCES ${FBG_SOURCES})
    list(APPEND SOURCES ${FBG_SOURCES_LODEPNG})
    list(APPEND SOURCES ${FBG_SOURCES_NANOJPEG})
else()
    if(NOT DEFINED BACKEND)
        message(FATAL_ERROR "BACKEND is undefined, This should be automatically set when the right platform is set, or should be command-line provided (e.g., -DBACKEND=PGE or -DBACKEND=FB)")
    else()
        message(FATAL_ERROR "Invalid backend: ${BACKEND}")
    endif()
endif()

# List vanassistant's cpp files
file(GLOB VANSSISTANT_SOURCES src/vanassistant/*.cpp)
list(APPEND SOURCES ${VANSSISTANT_SOURCES})

# List Sam's (c) source files
#file(GLOB SAM_SOURCES ${SAM_LIB_DIR}/src/*.c)
#list(APPEND SOURCES ${SAM_SOURCES})

# Set DRZENGINE library options to not build tests, examples, or documentation
set(BUILD_PGELIB OFF)
set(BUILD_FBLIB OFF)
set(BUILD_DOC OFF)
set(BUILD_TEST_APPS OFF)

# Then according to the backend, set which library to build
if(${BACKEND} STREQUAL "PGE")
    set(BUILD_PGELIB ON)
elseif(${BACKEND} STREQUAL "FB")
    set(BUILD_FBLIB ON)
else()
    message(FATAL_ERROR "Invalid backend for the DRZENGINE library: ${BACKEND}")
endif()

# Define the executable source files
add_executable(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${SOURCES})

# Add the DRZENGINE library as external project
add_subdirectory(${DRZENGINE_LIB_DIR} ${CMAKE_BINARY_DIR}/drzengine_build)

# Select platform libraries and flags
if(${PLATFORM} STREQUAL "X11")

    #set(PLATFORM_LIBS X11 GL pthread png stdc++fs dl)
    set(PLATFORM_CXX_FLAGS -O2)
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/local.cmake)

    # Link the drzenginepge library 
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
    # Link the drzenginepge library 
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} drzenginepge)
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/emscripten.cmake)

    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} pthread)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} dl)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} embind)

elseif(${PLATFORM} STREQUAL "WIN")

    # Link the drzenginepge library
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} drzenginepge)

    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/mingw64.cmake)
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/local.cmake

    #set(PLATFORM_LIBS mingw32 SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer SDL2_net SDL2_gfx pthread)

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
    #set(CMAKE_TOOLCHAIN_FILE "/home/drzoid/rpi-zero-minimal-buildroot/build_workdir/host/usr/share/buildroot/toolchainfile.cmake")
    set(PLATFORM_LIBS pthread dl atomic drzenginefb)
    set(PLATFORM_CXX_FLAGS -O2)
    target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${PLATFORM_LIBS})
elseif(${PLATFORM} STREQUAL "PIOS") 
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchains/crosspigcc.cmake)

    if(${BACKEND} STREQUAL "FB")

        set(PLATFORM_LIBS pthread dl atomic drzenginefb)
        set(PLATFORM_CXX_FLAGS -O2)
        target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${PLATFORM_LIBS})
        
    elseif(${BACKEND} STREQUAL "PGE")

        # Link the drzenginepge library 
        target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} atomic drzenginepge)

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

    
    endif()

else()
    message(FATAL_ERROR "Invalid platform: ${PLATFORM}")
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
    set_target_properties(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PROPERTIES OUTPUT_NAME "vanassistant.${OUTPUTFILE}")
endif()

# Post build commands
if(${PLATFORM} STREQUAL "PIOS")
    #copy ssh deploy script to build directory
    add_custom_command(
        TARGET ${PROJECT_NAME}_${BACKEND}_${PLATFORM} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/scripts/deploy_pi.sh
            ${CMAKE_BINARY_DIR}/deploy_pi.sh
        COMMENT "Copying deploy_pi.sh to build directory"
    )
elseif(${PLATFORM} STREQUAL "BUILDROOT")
    #copy binary to buildroot's vanassistant_overlay directory
    add_custom_command(
        TARGET ${PROJECT_NAME}_${BACKEND}_${PLATFORM} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/vanassistant.${OUTPUTFILE}
            ${BUILDROOT_COPY_DEST_DIR}/vanassistant.${OUTPUTFILE}
        COMMENT "Copying vanassistant.${OUTPUTFILE} to build directory"
    )
    #copy make_img script to build directory
    add_custom_command(
        TARGET ${PROJECT_NAME}_${BACKEND}_${PLATFORM} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/scripts/make_img.sh
            ${CMAKE_BINARY_DIR}/make_img.sh
        COMMENT "Copying make_img.sh to build directory"
    )
endif()
