cmake_minimum_required(VERSION 3.22.1)

# Setup project and set name
project(OpenGLTemplate)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set custom module directory
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Cmake will generate compile_commands.json file used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(GLFW_BUILD_DOCS OFF)

# These require findPackageName.cmake file to be available
# as an alternative we could use find_library() instead
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
# TODO: could be optional
find_package(assimp REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

if(assimp_FOUND)
    message(STATUS "libAssimp found")
else()
    message(STATUS "libAssimp not found")
endif()

file(GLOB sources src/*.cpp)
add_executable(${PROJECT_NAME} WIN32 ${sources})

add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb_image)

# Set 3rd party include locations
target_include_directories(${PROJECT_NAME} PUBLIC vendor)

message(STATUS "GLEW LIBRARY:${GLEW_LIBRARIES}")
message(STATUS "OpenGL LIBRARY:${OpenGL_LIBRARIES}")

# Link libraries
target_link_libraries(${PROJECT_NAME} ${assimp_LIBRARIES} ${OpenGL_LIBRARIES} ${GLEW_LIBRARIES} ${CMAKE_DL_LIBS} glfw glm imgui stb_image)
