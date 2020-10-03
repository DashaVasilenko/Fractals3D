if (WIN32)  
   cmake_minimum_required(VERSION 3.5)
endif()

if (APPLE)
   cmake_minimum_required(VERSION 2.6)
endif()

project(Fractals1)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)


if (WIN32)
   find_package(glfw3 3.3 REQUIRED)
   find_package(nlohmann_json CONFIG REQUIRED)
endif()

if (APPLE) 
   find_package (glfw3 REQUIRED)
   set(nlohmann_json_DIR /Users/dashka/Documents/vcpkg/vcpkg/packages/nlohmann-json_x64-osx/share/nlohmann_json)
   find_package(nlohmann_json CONFIG REQUIRED)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (WIN32)
   set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
#[[else()
   message(FATAL_ERROR "This platform is not supported.")]]
endif()

file(GLOB Sources
    "src/*.cpp"
    "imgui/*.cpp"
)

if (WIN32)
   add_executable(main WIN32 ${Sources} )
else()
   add_executable(main ${Sources} )
endif()

target_include_directories(main PUBLIC include imgui)

target_link_libraries(main PRIVATE GLEW::GLEW ${OPENGL_LIBRARIES} glfw nlohmann_json nlohmann_json::nlohmann_json)

if (WIN32)
   add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:main>)
endif()