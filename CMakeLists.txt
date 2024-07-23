cmake_minimum_required(VERSION 3.10)
project(vanassistant)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Include platform-specific configurations
if(DEFINED PLATFORM)
    #include(${CMAKE_SOURCE_DIR}/cmake/platforms/${PLATFORM}.cmake)
    if(${PLATFORM} STREQUAL "X11")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "WASM")
        set(BACKEND "PGE")
    elseif(${PLATFORM} STREQUAL "WIN")
        set(BACKEND "PGEW")
    elseif(${PLATFORM} STREQUAL "BUILDROOT")
        set(BACKEND "FB")
    elseif(${PLATFORM} STREQUAL "PIOS")
        set(BACKEND "FB")
    endif()
else()
    message(FATAL_ERROR "Please define the PLATFORM variable (e.g., -DPLATFORM=PIOS / -DPLATFORM=WSL / -DPLATFORM=WASM / -DPLATFORM=BUILDROOT)")
endif()

# Include backend-specific configurations
if(DEFINED BACKEND)
    #include(${CMAKE_SOURCE_DIR}/cmake/backends/${BACKEND}.cmake)
else()
    message(FATAL_ERROR "Please define the BACKEND variable (e.g., -DBACKEND=PGE or -DBACKEND=FB)")
endif()

# Include the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

#Source files, depending on the backend
if(${BACKEND} STREQUAL "PGE")
    #Include the olc::pge entry point
    set(SOURCES ${SOURCES} src/main_pge.cpp)
elseif(${BACKEND} STREQUAL "PGEW")
    #Include the olc::pge entry point
    set(SOURCES ${SOURCES} src/main_pge_win.cpp)
elseif(${BACKEND} STREQUAL "FB")
    #Include the framebuffer's entry point
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
elseif(${PLATFORM} STREQUAL "WIN")

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


else()
    message(FATAL_ERROR "Invalid platform: ${PLATFORM}")
endif()

#Output file
string(TOLOWER ${PLATFORM} OUTPUTFILE)

# Link libraries and set properties
target_link_libraries(${PROJECT_NAME}_${BACKEND}_${PLATFORM} ${PLATFORM_LIBS})
target_compile_options(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PRIVATE ${PLATFORM_CXX_FLAGS})
set_target_properties(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PROPERTIES OUTPUT_NAME "vanassistant.${OUTPUTFILE}")

#Overwrite the output name for the WASM platform and add the necessary flags
if(${PLATFORM} STREQUAL "WASM")
    set_target_properties(${PROJECT_NAME}_${BACKEND}_${PLATFORM} PROPERTIES
        LINK_FLAGS "-s WASM=1 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG -s ASSERTIONS=1 -s -pthread -s PTHREAD_POOL_SIZE=5 --shell-file ../wasm/shell.html"
        OUTPUT_NAME "vanassistant.html"
    )
endif()

#Post build commands
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
            /home/drzoid/rpi-zero-minimal-buildroot/vanassistant_overlay/opt/vanassistant/vanassistant.${OUTPUTFILE}
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
