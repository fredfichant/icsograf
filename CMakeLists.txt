cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(icsograf VERSION 1.0.1 LANGUAGES CXX)
set(PROJECT_SLUG "icsograf")
set(PROJECT_DESCRIPTION "Interactive Knotian Editor")
set(PROJECT_MANINTAINER_NAME "frédéric fichant")
set(PROJECT_MANINTAINER_EMAIL "fredfichant@gmail.com")
set(PROJECT_MANINTAINER "${PROJECT_MANINTAINER_NAME} <${PROJECT_MANINTAINER_EMAIL}>")
set(PROJECT_DESCRIPTION_LONG "icsograf is an interactive knotian editor. \
Knots drawn with icsograf can be exported as SVG.")
set(PROJECT_DOMAIN "webknot.web.app")
set(PROJECT_HOMEPAGE_URL "https://webknot.web.app/")
set(REPO "https://github.com/fredfichant/icsograf")
set(REPO_GIT "https://github.com/fredfichant/icsograf.git")
set(BUG_URL "")
set(ICON_RELPATH "img/icon.svg")
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

# Qt
set (QT_SUPPORTED_VERSIONS 5)
# Help CMake find Qt5 from Homebrew on macOS when no prefix is provided.
if(APPLE AND NOT Qt5_DIR)
    if(EXISTS "/usr/local/opt/qt@5/lib/cmake/Qt5")
        list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt@5")
    elseif(EXISTS "/opt/homebrew/opt/qt@5/lib/cmake/Qt5")
        list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")
    endif()
endif()
find_package(Qt5 COMPONENTS Widgets Xml Svg Network PrintSupport Sql REQUIRED)
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

if(APPLE)
    # Use standard macOS bundle layout: resources must go into Contents/Resources
    # rather than Contents/data. The executable is inside Contents/MacOS.
    install(DIRECTORY data/ DESTINATION ${PROJECT_SLUG}.app/Contents/Resources)

    # Note: the application target is configured as a BUNDLE in src/CMakeLists.txt
    # (set_target_properties(... MACOSX_BUNDLE TRUE) and install(TARGETS ... BUNDLE DESTINATION .)).
    # With this layout CPack DragNDrop will include a proper .app bundle.
elseif(WIN32)
    # The executable is in bin and searches for ../data.
    install(DIRECTORY data/ DESTINATION data)
else()
    install(DIRECTORY data/ DESTINATION ${DATA_INSTALL})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/icsograf.desktop.in
                   ${CMAKE_CURRENT_BINARY_DIR}/icsograf.desktop)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/icsograf.desktop DESTINATION "share/applications")
    install(FILES data/img/icon2.png DESTINATION share/icons/hicolor/64x64/apps/
            RENAME ${PROJECT_SLUG}.png)
    install(FILES data/img/icon2.svg DESTINATION share/icons/hicolor/scalable/apps/
            RENAME ${PROJECT_SLUG}.svg)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/icsograf.1.in
               ${CMAKE_CURRENT_BINARY_DIR}/icsograf.1)
find_program(GZIP_EXECUTABLE gzip)
if(GZIP_EXECUTABLE)
    execute_process(
        COMMAND ${GZIP_EXECUTABLE} -n -c ${CMAKE_CURRENT_BINARY_DIR}/icsograf.1
        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/icsograf.1.gz
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/icsograf.1.gz DESTINATION share/man/man1)
else()
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/icsograf.1 DESTINATION share/man/man1)
endif()

# ==================== Configuration CPack ====================
set(CPACK_PACKAGE_NAME "icsograf")
set(CPACK_PACKAGE_VENDOR "icsograf")
set(CPACK_PACKAGE_DESCRIPTION "icsograf est l'éditeur d'états nœudiens")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Configuration globale
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# ========== Configuration spécifique par plateforme ==========


if(APPLE)
    # Installer les ressources dans le dossier Resources du .app (convention macOS)
    install(DIRECTORY data/ DESTINATION ${PROJECT_SLUG}.app/Contents/Resources)

    # Installer le bundle si la cible est construite en tant que MACOSX_BUNDLE.
    # Remarque : la cible exécutable doit être créée en MACOSX_BUNDLE dans src/CMakeLists.txt :
    #   add_executable(icsograf MACOSX_BUNDLE ... )
    # Si la cible s'appelle 'icsograf', cette ligne installera la .app à la racine de l'install tree :
    install(TARGETS icsograf BUNDLE DESTINATION . OPTIONAL)
endif()


# Windows (WIX Installer)
if(WIN32)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "A9BF3F7B-7918-4D84-8E5A-9D80F4E608CE")
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/img/icon.ico")
endif()

# Linux (DEB, RPM, TGZ)
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    
    # Configuration DEB
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "frédéric fichant <fredfichant@gmail.com>")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/fredfichant/icsograf")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5, libqt5svg5, libqt5printsupport5, libqt5sql5-sqlite")
    
    # Configuration RPM
    set(CPACK_RPM_PACKAGE_RELEASE "1")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

if(APPLE OR WIN32)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy_qt.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake @ONLY)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)
endif()

# Configuration commune
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_STRIP_FILES FALSE)

include(CPack)
