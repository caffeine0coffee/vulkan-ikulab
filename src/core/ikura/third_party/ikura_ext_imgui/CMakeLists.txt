file(GLOB_RECURSE imgui_sources "*.cpp")

add_library(ikura_ext_imgui STATIC ${imgui_sources})

# print cmake_current_source_dir
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Freetype REQUIRED)

target_link_libraries(ikura_ext_imgui PRIVATE Vulkan::Headers)
target_link_libraries(ikura_ext_imgui PRIVATE Freetype::Freetype)