cmake_minimum_required(VERSION 2.6)
project(Fractals1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -g")

find_package (OpenGL REQUIRED)
find_package (glfw3 REQUIRED)

find_package (GLEW REQUIRED STATIC)

file(GLOB Sources
    "src/*.cpp"
    "imgui/*.cpp"
)

add_executable(main ${Sources} )
target_include_directories(main PUBLIC include imgui)

target_link_libraries(main glew ${OPENGL_LIBRARIES} glfw)