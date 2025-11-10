cmake_minimum_required(VERSION 3.10)
project(Tetris3D)

set(CMAKE_CXX_STANDARD 17)

# Chemins GLFW
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")
include_directories(${GLFW_DIR}/include)

# Chemins include
include_directories(${CMAKE_SOURCE_DIR}/include)

# Sources
file(GLOB SOURCES "src/*.cpp" "src/*.c")

add_executable(Tetris3D ${SOURCES})

# Link bibliothèques
target_link_libraries(Tetris3D 
    ${GLFW_DIR}/lib-mingw-w64/libglfw3.a
    opengl32
    gdi32
    user32
    kernel32
)