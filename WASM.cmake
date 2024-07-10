
# Specify the compiler for Emscripten
set(CMAKE_C_COMPILER emcc)
set(CMAKE_CXX_COMPILER em++)

# Additional compiler flags
set(PLATFORM_CXX_FLAGS "-O2")
set(PLATFORM_LIBS "embind")
