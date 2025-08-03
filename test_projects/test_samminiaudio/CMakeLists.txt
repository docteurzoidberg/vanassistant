cmake_minimum_required(VERSION 3.10)
project(SAMMiniaudioPGE)

set(CMAKE_CXX_STANDARD 17)

add_executable(sam_demo
    main.cpp
    sam/sam.c
    sam/reciter.c
    sam/render.c 
    sam/debug.c
)

# Si olcPixelGameEngine.h et miniaudio.h sont dans le dossier du projet :
target_include_directories(sam_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Pour éviter les warnings de MSVC (optionnel si tu es sous GCC/Clang)
target_compile_definitions(sam_demo PRIVATE _CRT_SECURE_NO_WARNINGS)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIRS})
target_link_libraries(sam_demo X11::X11)

# OpenGL
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
target_link_libraries(sam_demo ${OpenGL_LIBRARIES} OpenGL::GL)


# Threads
find_package(Threads REQUIRED)
target_link_libraries(sam_demo Threads::Threads)
include_directories(${Threads_INCLUDE_DIRS})

find_package(PNG REQUIRED)
target_link_libraries(sam_demo PNG::PNG)
include_directories(${PNG_INCLUDE_DIRS})

# stdc++fs
target_link_libraries(sam_demo stdc++fs)

# dl, for miniaudio
target_link_libraries(sam_demo dl)
