cmake_minimum_required(VERSION 3.10)
project(Tetris3D)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(external/glm)

find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw)

add_library(glad src/glad.c)

file(GLOB SRC_FILES src/*.cpp)
add_executable(Tetris3D ${SRC_FILES})

target_link_libraries(Tetris3D
    glfw
    glad
    ${OPENGL_LIBRARIES}
)