# The MIT License (MIT)
#
# Copyright (c) 2020-2021 Towel 42 Development, LLC and Scott Aron Bloom
# SPDX-License-Identifier: MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.30)
MACRO(CheckQtComponent whichLibVar components)
    if ( ${${whichLibVar}} )
        foreach( component ${components} )
            set( _QT_LIB_NAME Qt${component}_FOUND )
            if ( NOT "${_QT_LIB_NAME}" )
                #            set( VARNAME TOWEL42_Q${component}_SUPPORT )
                #            string( TOUPPER ${VARNAME} VARNAME )

                MESSAGE( AUTHOR_WARNING "  ${whichLibVar} requires the following Qt library 'Qt${component}'." )
                LIST( APPEND QtLibList ${component} )
            endif()
            unset( _QT_LIB_NAME )
        endforeach()
    endif()
ENDMACRO()

if( "${TOWEL42_UTILS_DLL}" STREQUAL "" )
    SET( TOWEL42_UTILS_DLL OFF )
endif()
SET( TOWEL42_UTILS_DLL ${TOWEL42_UTILS_DLL} CACHE BOOL "Build Towel42 Utils as a Shared Library" )

SET( TOWEL42_VSINSTALLER_SUPPORT OFF CACHE BOOL "Enable VS Installer Support" )

SET( TOWEL42_BIFSUPPORT OFF CACHE BOOL "Support for BIF Utilities" )
SET( TOWEL42_GIFSUPPORT OFF CACHE BOOL "Support for GIF Utilities" )
SET( TOWEL42_DESIGNERPLUGIN_SUPPORT OFF CACHE BOOL "Enable Creation of Designer Plugins Support" )
SET( TOWEL42_ZIP_SUPPORT OFF CACHE BOOL "Enable ZIP Utilities Support" )
SET( TOWEL42_ENABLE_TESTING OFF CACHE BOOL "Enable Testing" )

SET( TOWEL42_QCONCURRENT_SUPPORT OFF CACHE BOOL "Enable QConcurrent Support" )
SET( TOWEL42_QCORE_SUPPORT OFF CACHE BOOL "Enable QCore Support" )
SET( TOWEL42_QNETWORK_SUPPORT OFF CACHE BOOL "Enable QNetwork Support" )
SET( TOWEL42_QSQL_SUPPORT OFF CACHE BOOL "Enable QSql Support" )
SET( TOWEL42_QSVG_SUPPORT OFF CACHE BOOL "Enable QSvg Support" )
SET( TOWEL42_QWIDGETS_SUPPORT OFF CACHE BOOL "Enable QWidgets and QGui Support" )
SET( TOWEL42_QXML_SUPPORT OFF CACHE BOOL "Enable QXml Support" )

SET( QtLibList "" )

CheckQtComponent( TOWEL42_DESIGNERPLUGIN_SUPPORT "Core;Widgets;Gui;UiPlugin")
CheckQtComponent( TOWEL42_BIFSUPPORT Core )
CheckQtComponent( TOWEL42_GIFSUPPORT Core)
CheckQtComponent( TOWEL42_ZIP_SUPPORT Core )

if ( TOWEL42_VSINSTALLER_SUPPORT )
    LIST( APPEND QtLibList AxContainer )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_AXCONTAINER_LIB )
endif()
if ( TOWEL42_QCONCURRENT_SUPPORT )
    LIST( APPEND QtLibList "Concurrent" "Widgets" )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_CONCURRENT_LIB )
endif()
if ( TOWEL42_QCORE_SUPPORT )
    LIST( APPEND QtLibList Core )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_CORE_LIB )
endif()
if ( TOWEL42_QNETWORK_SUPPORT )
    LIST( APPEND QtLibList "Network" "Widgets" )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_NETWORK_LIB )
endif()
if ( TOWEL42_QSQL_SUPPORT )
    LIST( APPEND QtLibList Sql )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_SQL_LIB )
endif()
if ( TOWEL42_QSVG_SUPPORT )
    LIST( APPEND QtLibList Svg )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_SVG_LIB )
endif()
if ( TOWEL42_QWIDGETS_SUPPORT )
    LIST( APPEND QtLibList Widgets Gui )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_WIDGETS_LIB -DQT_GUI_LIB )
endif()
if ( TOWEL42_QXML_SUPPORT )
    LIST( APPEND QtLibList Xml )
    SET( TOWEL42_MOC_OPTIONS ${TOWEL42_MOC_OPTIONS} -DQT_XML_LIB )
endif()

message( STATUS "T42-Utils: Building for DLL=${TOWEL42_UTILS_DLL}" )

message( STATUS "T42-Utils: Building TOWEL42_QCONCURRENT_SUPPORT=${TOWEL42_QCONCURRENT_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QCORE_SUPPORT=${TOWEL42_QCORE_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QNETWORK_SUPPORT=${TOWEL42_QNETWORK_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QSQL_SUPPORT=${TOWEL42_QSQL_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QSVG_SUPPORT=${TOWEL42_QSVG_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QWIDGETS_SUPPORT=${TOWEL42_QWIDGETS_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QXML_SUPPORT=${TOWEL42_QXML_SUPPORT}" )

message( STATUS "T42-Utils: Building TOWEL42_VSINSTALLER_SUPPORT=${TOWEL42_VSINSTALLER_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_BIFSUPPORT=${TOWEL42_BIFSUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_GIFSUPPORT=${TOWEL42_GIFSUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_DESIGNERPLUGIN_SUPPORT=${TOWEL42_DESIGNERPLUGIN_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_ZIP_SUPPORT=${TOWEL42_ZIP_SUPPORT}" )

message( STATUS "T42-Utils: Building TOWEL42_ENABLE_TESTING=${TOWEL42_ENABLE_TESTING}" )

if ( NOT TOWEL42_UTILS_DIR )
    SET( TOWEL42_UTILS_DIR ${CMAKE_CURRENT_LIST_DIR} )
    MESSAGE( STATUS "T42-Utils: Using ${TOWEL42_UTILS_DIR} as the T42-Utils directory" )
endif()

if ( NOT TOWEL42_CMAKEUTILS_DIR )
    SET( TOWEL42_CMAKEUTILS_DIR ${CMAKE_SOURCE_DIR}/T42-CMakeUtils )
    MESSAGE( STATUS "T42-Utils: Using ${TOWEL42_CMAKEUTILS_DIR} as the T42-CMakeUtils directory" )
else()
    if ( NOT IS_ABSOLUTE ${TOWEL42_CMAKEUTILS_DIR} )
        MESSAGE( AUTHOR_WARNING " T42-Utils: ${TOWEL42_CMAKEUTILS_DIR} is relative, and will be made absolute to ${CMAKE_CURRENT_LIST_DIR}" )
        file( REAL_PATH "${TOWEL42_CMAKEUTILS_DIR}" TMP BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" )
        file( TO_CMAKE_PATH "${TMP}" TOWEL42_CMAKEUTILS_DIR )
    endif()

    if ( NOT EXISTS ${TOWEL42_CMAKEUTILS_DIR} )
        MESSAGE( FATAL_ERROR " T42-Utils: ${TOWEL42_CMAKEUTILS_DIR} does not exist" )
    endif()
    SET( TMP ${TOWEL42_CMAKEUTILS_DIR} )
    file( TO_CMAKE_PATH ${TMP} TOWEL42_CMAKEUTILS_DIR )
    unset( TMP )
endif()
MESSAGE( STATUS "T42-Utils: Using ${TOWEL42_CMAKEUTILS_DIR} as the T42-CMakeUtils directory" )

SET(CMAKE_MODULE_PATH ${TOWEL42_CMAKEUTILS_DIR}/Modules ${CMAKE_MODULE_PATH})

include( ${TOWEL42_CMAKEUTILS_DIR}/Modules/EnableGoogleTesting.cmake )

project( Towel42Utils LANGUAGES C CXX) 

find_package(Threads REQUIRED)
find_package(IncludeProjectSettings REQUIRED)

if ( TOWEL42_FORCE_QT_FOR_CONFIG_TESTING )
    LIST( APPEND QtLibList Core )
endif()

if ( QtLibList )
    message( STATUS "The following Qt libraries have been set for loading: ${QtLibList}" )
    find_package( Qt6 COMPONENTS ${QtLibList} REQUIRED )
endif()

if ( TOWEL42_DESIGNERPLUGIN_SUPPORT )
    SET_PROPERTY(GLOBAL PROPERTY QT_TARGETS_FOLDER CMakePredefinedTargets/QtInternalTargets)
    qt_standard_project_setup()
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/include.cmake )

IncludeProjectSettings()

add_compile_definitions( 
    $<$<BOOL:TOWEL42_DEBUG_TRACE>:TOWEL42_DEBUG_TRACE>
)

IF( Qt6Core_FOUND )
    find_package(Qt6SrcMoc REQUIRED)
    set(TOWEL42_MOC_OPTIONS "-DNON_VIS_BUILD=1")
endif()    

if( WIN32 AND TOWEL42_VSINSTALLER_SUPPORT )
    include( "${CMAKE_CURRENT_LIST_DIR}/CMakeNuGetTools/cmake/NuGetTools.cmake" )
    UNSET( NUGET_COMMAND CACHE )
    file( REAL_PATH ~/bin HOME_BIN_DIR EXPAND_TILDE)
    find_program( 
        NUGET_COMMAND 
        nuget.exe 
        PATHS ${HOME_BIN_DIR}
        REQUIRED )

    nuget_initialize()

    SET( packageID Microsoft.VisualStudio.Setup.Configuration.Native )
    nuget_add_dependencies(
        PACKAGE ${packageID} VERSION 3.4.2244 CMAKE_PREFIX_PATHS .
    )
    include_directories( ${NUGET_LAST_DEPENDENCY_CMAKE_PREFIX_PATHS_${packageID}}/lib/native/include )
    set( project_pub_DEPS
        ${project_pub_DEPS}
        ${NUGET_LAST_DEPENDENCY_CMAKE_PREFIX_PATHS_${packageID}}/lib/native/v141/x64/Microsoft.VisualStudio.Setup.Configuration.Native.lib
    )
endif()

if ( TOWEL42_UTILS_DLL )
    SET( LIBTYPE SHARED )
    add_compile_definitions( 
        Towel42Utils_EXPORTS
        $<$<BOOL:TOWEL42_UTILS_DLL>:TOWEL42_UTILS_DLL>
    )
else()
    SET( LIBTYPE STATIC )
    
    add_compile_definitions( 
        TOWEL42_UTILS_STATIC_DEFINE
    )
endif()

add_library(${PROJECT_NAME} ${LIBTYPE}
    ${_PROJECT_DEPENDENCIES}
)

include(GenerateExportHeader)
generate_export_header( Towel42Utils
    EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/Towel42UtilsExport.h
    BASE_NAME TOWEL42_UTILS
    )


target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        ${project_pub_DEPS}
    PRIVATE 
        ${project_pri_DEPS}
)

add_definitions( -DNON_VIS_BUILD=1 )
if ( TOWEL42_BIFSUPPORT )
    add_definitions( -DBIF_SCROLLBAR_SUPPORT=1 )
endif()

if ( TOWEL42_UTILS_DLL )
    set_target_properties( ${PROJECT_NAME} PROPERTIES 
                                FOLDER ${FOLDER_NAME} 
                                DEBUG_POSTFIX d
   ) 
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)  
    if ( WIN32 )
        INSTALL( FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION . CONFIGURATIONS Debug RelWithDebInfo )
    endif()
else()
    set_target_properties( ${PROJECT_NAME} PROPERTIES FOLDER ${FOLDER_NAME} )
endif()

if( TOWEL42_ENABLE_TESTING )
    add_subdirectory(UnitTests)
endif()

IF(TOWEL42_DESIGNERPLUGIN_SUPPORT)
    add_subdirectory(DesignerPlugins)
endif()    

if ( NOT TOWEL42_NO_DEPLOY_SUBMODULES )
    find_package(Deploy REQUIRED)
    if ( TOWEL42_UTILS_DLL )
        IF( Qt6Core_FOUND )
            DeployQt( ${PROJECT_NAME} . )
        ENDIF()

        DeploySystem( ${PROJECT_NAME} . )
    endif()
endif()
