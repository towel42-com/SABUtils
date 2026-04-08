# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
find_package( AddDesignerPlugin REQUIRED )

AddDesignerPlugin( BASENAME ImageHandler LINK_LIBS Towel42Utils )
AddDesignerPlugin( BASENAME ButtonGroupWDescriptiveText LINK_LIBS Towel42Utils ) 
AddDesignerPlugin( BASENAME SetReadOnlyComboBox )
AddDesignerPlugin( BASENAME SetReadOnlyGroupBox LINK_LIBS Towel42Utils ) 
AddDesignerPlugin( BASENAME SummaryDateEdit ) 

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
)
