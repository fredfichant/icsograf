
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(icsograf VERSION 1.0.0 LANGUAGES CXX)
set(PROJECT_SLUG "icsograf")
set(PROJECT_DESCRIPTION "Interactive Knotian Editor")
set(PROJECT_MANINTAINER_NAME "frédéric fichant")
set(PROJECT_MANINTAINER_EMAIL "fredfichant@gmail.com")
set(PROJECT_MANINTAINER "${PROJECT_MANINTAINER_NAME} <${PROJECT_MANINTAINER_EMAIL}>")
set(PROJECT_DESCRIPTION_LONG "icsograf is an editor for interlace patterns. \
Knots drawn with icsograf can be exported as SVG.")
set(PROJECT_DOMAIN "vertex-924b0.web.app")
set(PROJECT_HOMEPAGE_URL "https://vertex-924b0.web.app/")
set(REPO "https://gitlab.com/mattbas/Knotter")
set(REPO_GIT "git://gitlab.com/mattbas/Knotter.git")
set(BUG_URL "https://gitlab.com/mattbas/Knotter/issues")
set(PLUGIN_REPO "https://plugins.knotter.mattbas.org/")
set(DOC_URL "https://knotter.mattbas.org/manual")
set(ICON_RELPATH "img/icon-big.svg")
set(COPY_YEARS "2026-2036")
set(DATA_INSTALL "share/${PROJECT_SLUG}/${PROJECT_SLUG}")

set(VERSION_NAKED ${PROJECT_VERSION})
set(VERSION_SUFFIX "" CACHE STRING "")
if (NOT VERSION_SUFFIX STREQUAL "")
    set(PROJECT_VERSION "${PROJECT_VERSION}+${VERSION_SUFFIX}")
endif()

# CMake Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")


# Snapcraft stuff here because we don't care if cmake fails on CI
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/package/snapcraft.in.yaml"
    "${CMAKE_CURRENT_SOURCE_DIR}/snapcraft.yaml"
)
option(SNAP_IS_SUCH_A_PAIN "" OFF)
if ( SNAP_IS_SUCH_A_PAIN  )
    return()
endif()

# Qt
set (QT_SUPPORTED_VERSIONS 5)
find_package(Qt5 COMPONENTS Widgets Xml Svg Network UiTools PrintSupport REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Debug flags
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "${CMAKE_BUILD_TYPE} build")

# Targets
add_subdirectory(src bin)
enable_testing()
add_subdirectory(tests)
add_custom_target(copy_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_BINARY_DIR}/data
)

# Data
install(DIRECTORY data/ DESTINATION ${DATA_INSTALL})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/knotter.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/knotter.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/knotter.desktop DESTINATION "share/applications")
install(FILES data/img/${PROJECT_SLUG}.png DESTINATION share/icons/hicolor/64x64/apps/ )
install(FILES data/img/${PROJECT_SLUG}.svg DESTINATION share/icons/hicolor/scalable/apps/ )
