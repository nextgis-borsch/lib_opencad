################################################################################
#  Project: libopencad
#  Purpose: OpenSource CAD formats support library
#  Author: Alexandr Borzykh, mush3d at gmail.com
#  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
#  Language: C++
################################################################################
#  The MIT License (MIT)
#
#  Copyright (c) 2016 Alexandr Borzykh
#  Copyright (c) 2016-2017 NextGIS, <info@nextgis.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
################################################################################

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(opencad)

# Some init settings
set(CMAKE_COLOR_MAKEFILE ON)
# Set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(OSX_FRAMEWORK AND (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS))
  message(FATAL_ERROR "Only OSX_FRAMEWORK key or any or both BUILD_SHARED_LIBS
                       and BUILD_STATIC_LIBS keys are permitted")
endif()

if(OSX_FRAMEWORK)
  set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE INTERNAL "Installation directory for executables" FORCE)
  set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/Library/Frameworks" CACHE INTERNAL "Installation directory for libraries" FORCE)
  set(SKIP_INSTALL_EXECUTABLES OFF)
  set(SKIP_INSTALL_EXPORT ON)
  set(SKIP_INSTALL_FILES ON)
  set(SKIP_INSTALL_DOCS OFF)
  set(CMAKE_MACOSX_RPATH ON)
else()
    include(GNUInstallDirs)
    set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "Installation directory for executables" FORCE)
    set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "Installation directory for libraries" FORCE)
    set(INSTALL_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE INTERNAL "Installation directory for headers" FORCE)
    set(INSTALL_DOC_DIR ${CMAKE_INSTALL_DOCDIR} CACHE INTERNAL "Installation directory for doc pages" FORCE)
endif()

include(util)

check_version(OCAD_MAJOR_VERSION OCAD_MINOR_VERSION OCAD_REV_VERSION)
set(VERSION ${OCAD_MAJOR_VERSION}.${OCAD_MINOR_VERSION}.${OCAD_REV_VERSION})
set(SOVERSION 1)

if(OSX_FRAMEWORK)
    set(FRAMEWORK_VERSION "${OCAD_MAJOR_VERSION}")
    set(INSTALL_INC_DIR "${INSTALL_LIB_DIR}/${PROJECT_NAME}.framework/Versions/${FRAMEWORK_VERSION}/Headers" CACHE INTERNAL "Installation directory for headers" FORCE)
    set(INSTALL_DOC_DIR "${INSTALL_LIB_DIR}/${PROJECT_NAME}.framework/Versions/${FRAMEWORK_VERSION}/Resources/docs")
endif()

report_version(${PROJECT_NAME} ${VERSION})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-D_DEBUG)
endif()

configure_file(${CMAKE_MODULE_PATH}/uninstall.cmake.in ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)

enable_testing()
include(FindAnyProject)

set(TARGET_LINK)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(lib)
set(EXPORT_NAME ${PROJECT_NAME})
export(TARGETS ${TARGET_LINK} FILE ${EXPORT_NAME}-exports.cmake)

option(BUILD_APPS "Build console applications" ON)
if(BUILD_APPS)
    add_subdirectory(apps)
endif()
add_subdirectory(tests)

set(DOC_FILES LICENSE
    README.md)

install (FILES ${DOC_FILES}
         DESTINATION ${INSTALL_DOC_DIR}
         COMPONENT documents
         OPTIONAL)

if (REGISTER_PACKAGE)
    export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}-exports.cmake)

    set(EXPORT_PACKAGE_NAME OpenCAD)
    string(TOUPPER ${EXPORT_PACKAGE_NAME} EXPORT_PACKAGE_NAME_UPPER)
    set(EXPORT_PACKAGE_LIB_NAME ${PROJECT_NAME})

    export(PACKAGE ${EXPORT_PACKAGE_NAME})

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/PackageConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_PACKAGE_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_PACKAGE_NAME}ConfigVersion.cmake
        VERSION ${VERSION}
        COMPATIBILITY AnyNewerVersion )
else()
    export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}-exports.cmake EXPORT_LINK_INTERFACE_LIBRARIES)
endif()

# uninstall
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
