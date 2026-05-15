cmake_minimum_required(VERSION 3.22.1)

# Setup project and set name
project(Vigilant2D)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
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
include_directories(${GLEW_INCLUDE_DIRS})

find_package(Freetype REQUIRED)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp)
add_executable(${PROJECT_NAME} WIN32 ${sources})

# Disable tinyxml2 xml test target before building tinyxm2
set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/tinyxml2)
add_subdirectory(vendor/box2d)
add_subdirectory(vendor/lua)
add_subdirectory(vendor/openal)
add_subdirectory(vendor/libsndfile)

# Set 3rd party include locations
target_include_directories(${PROJECT_NAME} PUBLIC vendor ${FREETYPE_INCLUDE_DIRS})

message(STATUS "GLEW LIBRARY:${GLEW_LIBRARIES}")
message(STATUS "OpenGL LIBRARY:${OpenGL_LIBRARIES}")
message(STATUS "Freetype LIBRARY:${FREETYPE_LIBRARIES}")

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OpenGL_LIBRARIES} ${GLEW_LIBRARIES} ${CMAKE_DL_LIBS}
    glfw glm imgui stb_image tinyxml2 box2d lua::lib Freetype::Freetype openal libsndfile)
