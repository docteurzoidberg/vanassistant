cmake_minimum_required(VERSION 3.16)
project(font2h VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trouver FreeType
find_package(Freetype REQUIRED)

# Bibliothèque réutilisable
add_library(fontconvert_lib
  fontconvert.cpp
)

# Liaison moderne si disponible
if (TARGET Freetype::Freetype)
  target_link_libraries(fontconvert_lib PUBLIC Freetype::Freetype)
else()
  target_link_libraries(fontconvert_lib PUBLIC ${FREETYPE_LIBRARIES})
endif()

# Exécutable CLI
add_executable(font2h main.cpp)
target_link_libraries(font2h PRIVATE fontconvert_lib)
