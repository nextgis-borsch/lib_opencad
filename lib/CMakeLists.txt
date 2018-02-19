#*******************************************************************************
#  Project: libopencad
#  Purpose: OpenSource CAD formats support library
#  Author: Alexandr Borzykh, mush3d at gmail.com
#  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
#  Language: C++
#*******************************************************************************
#  The MIT License (MIT)
#
#  Copyright (c) 2016 Alexandr Borzykh
#  Copyright (c) 2016-2018 NextGIS, <info@nextgis.com>
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
#*******************************************************************************

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

set(OBJ_LIB)

add_subdirectory(dwg)

set(HHEADERS
    opencad.h
    opencad_api.h
    cadfile.h
    cadfileio.h
    cadheader.h
    cadclasses.h
    cadtables.h
    cadgeometry.h
    cadlayer.h
    cadcolors.h
    caddictionary.h
    cadobjects.h)

set(HHEADER_PRIV
    cadfilestreamio.h
    )

set(CSOURCES
    opencad.cpp
    cadcolors.cpp
    cadfile.cpp
    cadfileio.cpp
    cadfilestreamio.cpp
    cadheader.cpp
    cadclasses.cpp
    cadtables.cpp
    cadgeometry.cpp
    cadobjects.cpp
    cadlayer.cpp
    caddictionary.cpp)

set(LIB_NAME ${PROJECT_NAME})
if(BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${CSOURCES} ${HHEADERS} ${HHEADER_PRIV} ${OBJ_LIB})
    set_target_properties (${LIB_NAME} PROPERTIES
        VERSION ${VERSION}
        SOVERSION ${SOVERSION}
        DEFINE_SYMBOL OCAD_EXPORTS
    )
    if(MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES IMPORT_SUFFIX "_i.lib")
    endif()
elseif(OSX_FRAMEWORK)
    add_library(${LIB_NAME} SHARED ${CSOURCES} ${HHEADERS} ${HHEADER_PRIV} ${OBJ_LIB})
    set_target_properties(${LIB_NAME} PROPERTIES
      FRAMEWORK TRUE
      FRAMEWORK_VERSION ${FRAMEWORK_VERSION}
      MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${VERSION}
      MACOSX_FRAMEWORK_BUNDLE_VERSION ${VERSION}
      MACOSX_FRAMEWORK_IDENTIFIER org.opencad.opencad
      XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
      PUBLIC_HEADER "${HHEADERS}"
      VERSION ${VERSION}
      SOVERSION ${SOVERSION}
      )
else()
    add_library(${LIB_NAME} STATIC ${CSOURCES} ${HHEADERS} ${HHEADER_PRIV} ${OBJ_LIB})
endif()

set(INSTALL_TARGETS ${LIB_NAME})
if(BUILD_SHARED_LIBS AND BUILD_STATIC_LIBS)
    add_library(${LIB_NAME}static STATIC ${CSOURCES} ${HHEADERS} ${HHEADER_PRIV} ${OBJ_LIB})
    set(INSTALL_TARGETS ${INSTALL_TARGETS} ${LIB_NAME}static)
    if(WIN32)
        set_target_properties(${LIB_NAME}static PROPERTIES OUTPUT_NAME ${LIB_NAME}_i)
    else()
        set_target_properties(${LIB_NAME}static PROPERTIES OUTPUT_NAME ${LIB_NAME})
    endif()
    set_target_properties(${LIB_NAME}static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif()

set(TARGET_LINK ${TARGET_LINK} ${LIB_NAME} PARENT_SCOPE)

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS ${INSTALL_TARGETS}
        EXPORT ${PACKAGE_UPPER_NAME}Targets
        RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT libraries
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT libraries
        LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT libraries
        FRAMEWORK DESTINATION ${INSTALL_LIB_DIR}
    )
endif()

if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)
    install(FILES ${HHEADERS} DESTINATION "${INSTALL_INC_DIR}" COMPONENT headers)
endif()

# Add path to includes to build-tree export
target_include_directories(${LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# Add all targets to the build-tree export set
export(TARGETS ${INSTALL_TARGETS}
    FILE ${PROJECT_BINARY_DIR}/${PACKAGE_UPPER_NAME}Targets.cmake)
