cmake_minimum_required(VERSION 3.16)
project(ikulab-motion-viewer VERSION 1.2.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# ------------------------------------------------------------
# build settings for each platform
# ------------------------------------------------------------

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif ()

if (WIN32)
    add_compile_options(
            -D_USE_MATH_DEFINES
            -DIS_WINDOWS
            -std:c++20
    )
elseif (LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    add_compile_options(
            -std=c++20 -fdiagnostics-color=always -g
            -Wno-nullability-completeness
    )
elseif (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    add_compile_options(
            -std=c++20 -fdiagnostics-color=always -g
            -Wno-nullability-completeness
    )
endif ()

# ------------------------------------------------------------
# build ikulab-motion-viewer
# ------------------------------------------------------------

# set IKURA_ENABLE_VALIDATION_LAYER flag
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_options(-DIKURA_ENABLE_VALIDATION_LAYER)
endif ()

# build ikura
add_subdirectory(${CMAKE_SOURCE_DIR}/src/core/ikura)
# build ikulab-motion-viewer
add_subdirectory(${CMAKE_SOURCE_DIR}/src/app)
# link ikura to ikulab-motion-viewer
target_link_libraries(ikulab-motion-viewer PRIVATE ikura)

# find / link third party libraries
# libraries are installed via vcpkg
find_package(tinyfiledialogs CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
target_link_libraries(ikulab-motion-viewer PRIVATE tinyfiledialogs::tinyfiledialogs)
target_link_libraries(ikulab-motion-viewer PRIVATE glm::glm)

if ("${FORCE_USE_CPP_MAIN}" STREQUAL "ON")
    message("Build as console application for Windows")
else ()
    message("Build as GUI application for Windows")
    # Set entry point to WinMain
    set_target_properties(ikulab-motion-viewer PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

configure_file(${CMAKE_SOURCE_DIR}/src/app/buildInfo.h.in ${CMAKE_SOURCE_DIR}/src/app/buildInfo.h)
configure_file(${CMAKE_SOURCE_DIR}/assets/macos/Info.plist.in ${CMAKE_SOURCE_DIR}/assets/macos/Info.plist)

# ------------------------------------------------------------
# config ikulab-motion-viewer 
# ------------------------------------------------------------

target_compile_definitions(ikulab-motion-viewer PRIVATE GLFW_INCLUDE_VULKAN)
target_compile_definitions(ikulab-motion-viewer PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS)

# override application resource director
if (DEFINED RESOURCE_DIR)
    target_compile_definitions(ikulab-motion-viewer PRIVATE RESOURCE_DIR="${RESOURCE_DIR}")
    message(STATUS "Resource directory is set to ${RESOURCE_DIR}")
endif ()

if (DEFINED ENABLE_VALIDATION_LAYER)
    target_compile_definitions(ikulab-motion-viewer PRIVATE IKURA_ENABLE_VALIDATION_LAYER)
    message(STATUS "Validation layer is enabled")
endif ()

# ------------------------------------------------------------
# Config installer
# ------------------------------------------------------------

# prepare MacOS bundle
if (APPLE AND GENERATE_PACKAGE)
    include(CPack)

    # create bundle (and install binary and Info.plist)
    set_target_properties(ikulab-motion-viewer PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/assets/macos/Info.plist
    )
    # set rpath
    set_target_properties(ikulab-motion-viewer PROPERTIES
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    install(TARGETS ikulab-motion-viewer BUNDLE DESTINATION .)

    # install files

    # startup script
    install(FILES ${PROJECT_SOURCE_DIR}/assets/macos/startup.sh
        DESTINATION ikulab-motion-viewer.app/Contents/MacOS
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

    # version checker
    install(FILES ${PROJECT_SOURCE_DIR}/assets/imv_version_checker
        DESTINATION ikulab-motion-viewer.app/Contents/Resources
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

    # install version info directory
    install(DIRECTORY ${PROJECT_BINARY_DIR}/version_info
        DESTINATION ikulab-motion-viewer.app/Contents/Resources)

    # vulkan dynamic libraries
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets/macos/Frameworks
        DESTINATION ikulab-motion-viewer.app/Contents
        FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
    # vulkan library settings
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets/macos/vulkan
        DESTINATION ikulab-motion-viewer.app/Contents/Resources)

    # icon
    install(FILES ${PROJECT_SOURCE_DIR}/assets/macos/icon.icns
        DESTINATION ikulab-motion-viewer.app/Contents/Resources)
    # fonts
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets/fonts
        DESTINATION ikulab-motion-viewer.app/Contents/Resources)

    # CPack config for MacOS
    set(CPACK_BUNDLE_NAME "ikulab-motion-viewer")
    set(CPACK_BUNDLE_PLIST "${PROJECT_SOURCE_DIR}/Info.plist")
    set(CPACK_BUNDLE_ICON "${PROJECT_SOURCE_DIR}/icon.icns")
    set(CPACK_BUNDLE_STARTUP_COMMAND "${PROJECT_SOURCE_DIR}/build/app/startup.sh")
endif ()

if (LINUX AND GENERATE_PACKAGE)
    # CPack config
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_VENDOR "ikulab / CaffeineFree")
    set(CPACK_PACKAGE_NAME "ikulab-motion-viewer")
    set(CPACK_PACKAGE_CONTACT "dev@caffeinezero.info")
    set(CPACK_PACKAGE_DESCRIPTION "The BVH Motion Viewer powered by ikura Graphics Engine.")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
endif ()

# ------------------------------------------------------------
# Custom Target
# ------------------------------------------------------------

add_custom_target(write_version_info
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/version_info
    COMMAND ${CMAKE_COMMAND} -E echo "v${PROJECT_VERSION}" > ${PROJECT_BINARY_DIR}/version_info/current_version.txt
    COMMAND ${CMAKE_COMMAND} -E echo "v${PROJECT_VERSION}" > ${PROJECT_BINARY_DIR}/version_info/skip_version.txt
)

add_custom_target(write_version_info_for_develop
    COMMAND ${CMAKE_COMMAND} -E make_directory ${RESOURCE_DIR}/version_info
    COMMAND ${CMAKE_COMMAND} -E echo "v${PROJECT_VERSION}" > ${RESOURCE_DIR}/version_info/current_version.txt
    COMMAND ${CMAKE_COMMAND} -E echo "v${PROJECT_VERSION}" > ${RESOURCE_DIR}/version_info/skip_version.txt
)
