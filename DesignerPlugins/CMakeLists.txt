# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(Towel42UtilsDesignerPlugins LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui UiPlugin Widgets)

qt_add_plugin(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ../ImageHandler.cpp ../ImageHandler.h
    ImageHandlerDesignerPlugin.cpp ImageHandlerDesignerPlugin.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::UiPlugin
    Qt::Widgets
    Towel42Utils
)

set(INSTALL_PLUGINS_DIR "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/designer")

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "${INSTALL_PLUGINS_DIR}"
    BUNDLE DESTINATION "${INSTALL_PLUGINS_DIR}"
    LIBRARY DESTINATION "${INSTALL_PLUGINS_DIR}"
)

set_target_properties( ${PROJECT_NAME} PROPERTIES FOLDER DesignerPlugins )
#set_target_properties( ${PROJECT_NAME}_autogen PROPERTIES FOLDER DesignerPlugins )
#set_target_properties( ${PROJECT_NAME}_automoc_json_extraction PROPERTIES FOLDER DesignerPlugins )
