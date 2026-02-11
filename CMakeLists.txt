# The MIT License (MIT)
#
# Copyright (c) 2020-2021 Scott Aron Bloom
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

if( TOWEL42_UTILS_DLL )
    SET( TOWEL42_UTILS_DLL ON CACHE BOOL "Build Towel42 Utils as a Shared Library" )
else()
    SET( TOWEL42_UTILS_DLL OFF CACHE BOOL "Build Towel42 Utils as a Shared Library" )
endif()

SET( TOWEL42_BIFSUPPORT OFF CACHE BOOL "Support for BIF Utilities" )
SET( TOWEL42_GIFSUPPORT OFF CACHE BOOL "Support for GIF Utilities" )
SET( TOWEL42_MKVUTILS OFF CACHE BOOL "Support for MKV Utilities" )
SET( TOWEL42_NUGET_SUPPORT OFF CACHE BOOL "Enable NuGet Package Support" )
SET( TOWEL42_QAXOBJECT_SUPPORT OFF CACHE BOOL "Enable QAxObject Utilities Support" )
SET( TOWEL42_QCONCURRENT_SUPPORT OFF CACHE BOOL "Enable QConcurrent Utilities Support" )
SET( TOWEL42_QCORE_SUPPORT OFF CACHE BOOL "Enable QCore Utilities Support" )
SET( TOWEL42_QNETWORK_SUPPORT OFF CACHE BOOL "Enable QNetwork Utilities Support" )
SET( TOWEL42_QSQL_SUPPORT OFF CACHE BOOL "Enable QSql Utilities Support" )
SET( TOWEL42_QWIDGET_SUPPORT OFF CACHE BOOL "Enable QWidget Utilities Support" )
SET( TOWEL42_QXMLPATTERNS_SUPPORT OFF CACHE BOOL "Enable QXmlPatterns Utilities Support" )
SET( TOWEL42_QXML_SUPPORT OFF CACHE BOOL "Enable QXml Utilities Support" )
SET( TOWEL42_ZIP_SUPPORT OFF CACHE BOOL "Enable ZIP Utilities Support" )
SET( TOWEL42_ENABLE_TESTING OFF CACHE BOOL "Enable Testing" )
   
message( STATUS "T42-Utils: Building for DLL=${TOWEL42_UTILS_DLL}" )

message( STATUS "T42-Utils: Building TOWEL42_BIFSUPPORT=${TOWEL42_BIFSUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_GIFSUPPORT=${TOWEL42_GIFSUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_MKVUTILS=${TOWEL42_MKVUTILS}" )
message( STATUS "T42-Utils: Building TOWEL42_NUGET_SUPPORT=${TOWEL42_NUGET_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QAXOBJECT_SUPPORT=${TOWEL42_QAXOBJECT_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QCONCURRENT_SUPPORT=${TOWEL42_QCONCURRENT_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QCORE_SUPPORT=${TOWEL42_QCORE_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QNETWORK_SUPPORT=${TOWEL42_QNETWORK_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QSQL_SUPPORT=${TOWEL42_QSQL_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QWIDGET_SUPPORT=${TOWEL42_QWIDGET_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QXMLPATTERNS_SUPPORT=${TOWEL42_QXMLPATTERNS_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_QXML_SUPPORT=${TOWEL42_QXML_SUPPORT}" )
message( STATUS "T42-Utils: Building TOWEL42_ZIP_SUPPORT=${TOWEL42_ZIP_SUPPORT}" )

message( STATUS "T42-Utils: Building TOWEL42_ENABLE_TESTING=${TOWEL42_ENABLE_TESTING}" )

if ( NOT TOWEL42_UTILS_DIR )
    SET( TOWEL42_UTILS_DIR ${CMAKE_CURRENT_LIST_DIR} )
endif()

add_definitions( -DTOWEL42UTILS_INTERNAL )


include( ${CMAKE_SOURCE_DIR}/T42-CMakeUtils/Modules/EnableGoogleTesting.cmake )

include( ${CMAKE_CURRENT_LIST_DIR}/include.cmake )

if ( TOWEL42_MKVUTILS )
    SET(NO_USE_MEDIAINFO_INSTALL ON)
    set(BUILD_SHARED_LIBS ON)
	option(LARGE_FILES "Enable large files support" OFF)
    add_subdirectory( MediaInfo/ProjectFiles/MediaInfoLib )
    add_subdirectory( MKVReader )
endif()

OPTION(TOWEL42_DEBUG_TRACE "Enable Debug Tracing to std::cout" OFF)

project( ${_PROJECT_NAME} ) 
find_package(IncludeProjectSettings REQUIRED)
IncludeProjectSettings()

find_package(Deploy REQUIRED)
find_package(AddUnitTest REQUIRED)

IF(TOWEL42_QCORE_SUPPORT)
    find_package(Qt6SrcMoc REQUIRED)
    set(TOWEL42_MOC_OPTIONS "-DNON_VIS_BUILD=1")
endif()    

if ( TOWEL42_MKVUTILS )
    include_directories(MediaInfo/MediaInfoLib/Source)
    include_directories(MediaInfo/ZenLib/Source)
endif()

if( WIN32 AND TOWEL42_NUGET_SUPPORT )
    include( "${CMAKE_CURRENT_LIST_DIR}/CMakeNuGetTools/cmake/NuGetTools.cmake" )
    UNSET( NUGET_COMMAND CACHE )
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
else()
    SET( LIBTYPE STATIC )
endif()

add_library(${PROJECT_NAME} ${LIBTYPE}
    ${_PROJECT_DEPENDENCIES}
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
    if ( WIN32 )
        INSTALL( FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.dll DESTINATION . )
        INSTALL( FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.pdb DESTINATION . CONFIGURATIONS Debug RelWithDebInfo )
    else()
        INSTALL( FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.so DESTINATION . )
    endif()
else()
    set_target_properties( ${PROJECT_NAME} PROPERTIES FOLDER ${FOLDER_NAME} )
endif()

if( TOWEL42_ENABLE_TESTING )
    add_subdirectory(UnitTests)
endif()

if ( NOT TOWEL42_NO_DEPLOY_SUBMODULES )
    if ( TOWEL42_UTILS_DLL )
        IF(TOWEL42_QCORE_SUPPORT)
            DeployQt( ${PROJECT_NAME} . )
        ENDIF()

        DeploySystem( ${PROJECT_NAME} . )
    endif()
endif()
