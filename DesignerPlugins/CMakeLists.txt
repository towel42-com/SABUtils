# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
find_package( AddDesignerPlugin REQUIRED )

#find_package( T42Utils REQUIRED )
#PrintAllVariables( PATTERN "(QT.*)|(Qt.*)" )
get_filename_component( pluginDir "${QT6_INSTALL_PREFIX}" DIRECTORY )
SET( pluginDir ${pluginDir}/t42-plugins/${Qt6_VERSION}/designer )
AddDesignerPlugin( BASENAME ImageHandler LINK_LIBS Towel42Utils PLUGIN_DIR "${pluginDir}" )
AddDesignerPlugin( BASENAME ButtonGroupWDescriptiveText LINK_LIBS Towel42Utils PLUGIN_DIR "${pluginDir}") 
AddDesignerPlugin( BASENAME SetReadOnlyComboBox PLUGIN_DIR "${pluginDir}")
AddDesignerPlugin( BASENAME SetReadOnlyGroupBox LINK_LIBS Towel42Utils PLUGIN_DIR "${pluginDir}") 
AddDesignerPlugin( BASENAME SummaryDateEdit PLUGIN_DIR "${pluginDir}") 

AddDesignerPlugin( BASENAME ImageListWidget
    FILES 
    ../ImageListWidget.cpp 
    ../ImageListWidget.h 
    ../ButtonEnabler.cpp
    ../ButtonEnabler.h
    ../ImageHandler.h
    ../ImageHandler.cpp
    ../SystemLoggingDefs.h
    ../SystemLoggingDefs.cpp
    ImageListWidgetDesignerPlugin.cpp 
    ImageListWidgetDesignerPlugin.h
    LINK_LIBS Towel42Utils
    PLUGIN_DIR "${pluginDir}"
)
