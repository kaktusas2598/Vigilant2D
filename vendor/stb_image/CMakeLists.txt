file(GLOB STB_SOURCES *.cpp)
add_library(stb_image STATIC ${STB_SOURCES})

include_directories(
    ${CMAKE_SOURCE_DIR})

target_link_libraries(imgui)
