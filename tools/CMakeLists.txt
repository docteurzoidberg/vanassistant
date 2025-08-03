cmake_minimum_required(VERSION 3.16)
project(VanAssistantTools)

# Option globale : binaires dans le dossier tools/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Ajouter tous les sous-projets ici
add_subdirectory(object-to-header)
add_subdirectory(serial-gui)
add_subdirectory(png-to-header)
# etc...
