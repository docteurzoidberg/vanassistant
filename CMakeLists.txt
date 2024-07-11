
cmake_minimum_required(VERSION 3.10)
project(vanassistant)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Include the sam's source directory
file(GLOB SAM_SOURCES ${CMAKE_SOURCE_DIR}/src/sam/*.c)
list(APPEND SOURCES ${SAM_SOURCES})

# Include the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Include platform-specific configurations
if(DEFINED PLATFORM)
    include(${CMAKE_SOURCE_DIR}/${PLATFORM}.cmake)
else()
    message(FATAL_ERROR "Please define the PLATFORM variable (e.g., -DPLATFORM=PI)")
endif()

if(PLATFORM STREQUAL "PI")
    set(SOURCES ${SOURCES} src/main_fb.cpp)  

    file(GLOB FBG_SOURCES ${CMAKE_SOURCE_DIR}/src/fb/*.c)
    list(APPEND SOURCES ${FBG_SOURCES})

    file(GLOB FBG_SOURCES ${CMAKE_SOURCE_DIR}/src/fb/lodepng/*.c)
    list(APPEND SOURCES ${FBG_SOURCES})

    file(GLOB FBG_SOURCES ${CMAKE_SOURCE_DIR}/src/fb/nanojpeg/*.c)
    list(APPEND SOURCES ${FBG_SOURCES})
    
else()
    set(SOURCES ${SOURCES} src/main_pge.cpp)
endif()

# Define the executable
add_executable(${PROJECT_NAME}_${PLATFORM} ${SOURCES})

#Output file
string(TOLOWER ${PLATFORM} OUTPUTFILE)

# Link libraries and set properties
target_link_libraries(${PROJECT_NAME}_${PLATFORM} ${PLATFORM_LIBS})
target_compile_options(${PROJECT_NAME}_${PLATFORM} PRIVATE ${PLATFORM_CXX_FLAGS})
set_target_properties(${PROJECT_NAME}_${PLATFORM} PROPERTIES OUTPUT_NAME "vanassistant.${OUTPUTFILE}")

# Platform-specific configurations
if(PLATFORM STREQUAL "PI")
    target_link_libraries(${PROJECT_NAME}_${PLATFORM} pthread dl atomic)
    
    # Add custom command to copy deploy script to build directory
    add_custom_command(
        TARGET ${PROJECT_NAME}_PI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/scripts/deploy_pi.sh
            ${CMAKE_BINARY_DIR}/deploy_pi.sh
        COMMENT "Copying deploy_pi.sh to build directory"
    )

elseif(PLATFORM STREQUAL "X11")
    target_link_libraries(${PROJECT_NAME}_${PLATFORM} X11 GL pthread png stdc++fs /usr/lib/x86_64-linux-gnu dl)
elseif(PLATFORM STREQUAL "WASM")
    target_compile_options(${PROJECT_NAME}_${PLATFORM} PRIVATE -pthread)
    target_link_libraries(${PROJECT_NAME}_${PLATFORM} pthread dl)
    set_target_properties(${PROJECT_NAME}_${PLATFORM} PROPERTIES
        LINK_FLAGS "-s WASM=1 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG -s ASSERTIONS=1 -s -pthread -s PTHREAD_POOL_SIZE=5 --shell-file ../wasm/shell.html"
        OUTPUT_NAME "vanassistant.html"
    )
endif()
