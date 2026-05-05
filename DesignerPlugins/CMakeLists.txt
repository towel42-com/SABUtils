# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
find_package( AddDesignerPlugin REQUIRED )

#find_package( T42Utils REQUIRED )
#PrintAllVariables( PATTERN "(QT.*)|(Qt.*)" )
get_filename_component( pluginDir "${QT6_INSTALL_PREFIX}" DIRECTORY )
SET( pluginDir ${pluginDir}/t42-plugins/${Qt6_VERSION}/designer )

AddDesignerPlugin( BASENAME ButtonGroupWDT
    FILES 
        ButtonGroupWDescriptiveTextDesignerPlugin.cpp
        ButtonGroupWDescriptiveTextDesignerPlugin.h
        ../ButtonGroupWDescriptiveText.h
        ../ButtonGroupWDescriptiveText.cpp
        ../FromString.cpp
        ../FromString.h
        ../RegExUtils.cpp
        ../RegExUtils.h
        ../SystemLoggingDefs.h
        ../SystemLoggingDefs.cpp
        ../StringUtils.cpp
        ../StringUtils.h
        ../uiUtils.cpp
        ../uiUtils.h
        ../utils.cpp
        ../utils.h
    LINK_LIBS 
    PLUGIN_DIR "${pluginDir}"
)

AddDesignerPlugin( BASENAME ImageHandler
    FILES 
        ImageHandlerDesignerPlugin.cpp
        ImageHandlerDesignerPlugin.h
        ../ImageHandler.cpp
        ../ImageHandler.h
        ../ButtonGroupWDescriptiveText.h
        ../ButtonGroupWDescriptiveText.cpp
        ../FromString.cpp
        ../FromString.h
        ../RegExUtils.cpp
        ../RegExUtils.h
        ../SetReadOnly.cpp
        ../SetReadOnly.h
        ../StringUtils.cpp
        ../StringUtils.h
        ../SystemLoggingDefs.h
        ../SystemLoggingDefs.cpp
        ../uiUtils.cpp
        ../uiUtils.h
        ../utils.cpp
        ../utils.h
    LINK_LIBS  
    PLUGIN_DIR "${pluginDir}" 
)

AddDesignerPlugin( BASENAME ImageListWidget
    FILES 
        ImageListWidgetDesignerPlugin.cpp 
        ImageListWidgetDesignerPlugin.h
        ../ImageListWidget.cpp 
        ../ImageListWidget.h 
        ../ButtonGroupWDescriptiveText.h
        ../ButtonGroupWDescriptiveText.cpp
        ../ButtonEnabler.cpp
        ../ButtonEnabler.h
        ../FromString.cpp
        ../FromString.h
        ../ImageHandler.h
        ../ImageHandler.cpp
        ../RegExUtils.cpp
        ../RegExUtils.h
        ../SetReadOnly.cpp
        ../SetReadOnly.h
        ../StringUtils.cpp
        ../StringUtils.h
        ../SystemLoggingDefs.h
        ../SystemLoggingDefs.cpp
        ../uiUtils.cpp
        ../uiUtils.h
        ../utils.cpp
        ../utils.h
    LINK_LIBS
    PLUGIN_DIR "${pluginDir}"
)

AddDesignerPlugin( BASENAME SetReadOnlyComboBox 
    FILES
        SetReadOnlyComboBoxDesignerPlugin.cpp
        SetReadOnlyComboBoxDesignerPlugin.h
        ../SetReadOnlyComboBox.h
        ../SetReadOnlyComboBox.cpp
    LINK_LIBS 
    PLUGIN_DIR "${pluginDir}"
)

AddDesignerPlugin( BASENAME SetReadOnlyGroupBox 
    FILES
        SetReadOnlyGroupBoxDesignerPlugin.cpp
        SetReadOnlyGroupBoxDesignerPlugin.h
        ../ButtonGroupWDescriptiveText.h
        ../ButtonGroupWDescriptiveText.cpp
        ../FromString.cpp
        ../FromString.h
        ../RegExUtils.cpp
        ../RegExUtils.h
        ../SetReadOnlyGroupBox.h
        ../SetReadOnlyGroupBox.cpp
        ../SetReadOnly.cpp
        ../SetReadOnly.h
        ../StringUtils.cpp
        ../StringUtils.h
        ../SystemLoggingDefs.h
        ../SystemLoggingDefs.cpp
        ../uiUtils.cpp
        ../uiUtils.h
        ../utils.cpp
        ../utils.h

    LINK_LIBS 
    PLUGIN_DIR "${pluginDir}"
) 

AddDesignerPlugin( BASENAME SummaryDateEdit 
    FILES
        SummaryDateEditDesignerPlugin.cpp
        SummaryDateEditDesignerPlugin.h
        ../SummaryDateEdit.h
        ../SummaryDateEdit.cpp
    PLUGIN_DIR "${pluginDir}"
) 
