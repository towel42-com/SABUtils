# The MIT License (MIT)
#
# Copyright (c) 2020-2026 Scott Aron Bloom
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

if ( TOWEL42_UTILS_DLL )
    set(FOLDER_NAME SharedLibs)
else()
    set(FOLDER_NAME Libs)
endif()

IF(WIN32)
        set( OS_SRCS 
            ConsoleUtils.cpp 
            WindowsError.cpp 
        )
        set( OS_HEADERS 
            ConsoleUtils.h 
            WindowsError.h 
        )
ELSE()
ENDIF()

set(project_SRCS
    CustomerData.cpp
    FileSIDInfo.cpp
    FromString.cpp
    GPUDetect.cpp
    RegExUtils.cpp
    StringComparisonClasses.cpp
    StringUtils.cpp
    VersionInfoData.cpp
    ${OS_SRCS}
)

set(qtproject_CPPMOC_SRCS
)

set(project_H
    CantorHash.h
    CustomerData.h
    EnumUtils.h
    FileSIDInfo.h
    FromString.h
    GPUDetect.h
    HashUtils.h
    RegExUtils.h
    RevertValue.h
    StringComparisonClasses.h
    StringUtils.h
    Towel42UtilsExport.h
    VersionInfoData.h
    Towel42UtilsFwd.h
    ${OS_HEADERS}
)

set(qtproject_UIS
)

MACRO(CheckForCoreSupport whichLibVAR whichLibName)
    if ( NOT TOWEL42_QCORE_SUPPORT )
        if ( ${whichLibVAR} )
            MESSAGE( WARNING "${whichLibName} Support requires QtCore Support, set TOWEL42_QCORE_SUPPORT to true to prevent this warning" )
            set( TOWEL42_QCORE_SUPPORT ON )
            set( TOWEL42_QCORE_SUPPORT ON PARENT_SCOPE )
        endif()
    endif()
ENDMACRO()

MACRO(CheckForWidgetSupport whichLibVAR whichLibName)
    if ( NOT TOWEL42_QWIDGET_SUPPORT )
        if ( ${whichLibVAR} )
            MESSAGE( WARNING "${whichLibName} Support requires QtWidget Support, set TOWEL42_QWIDGET_SUPPORT to true to prevent this warning" )
            set( TOWEL42_QWIDGET_SUPPORT ON )
            set( TOWEL42_QWIDGET_SUPPORT ON PARENT_SCOPE )
        endif()
    endif()
ENDMACRO()

CheckForCoreSupport( TOWEL42_BIFSUPPORT "BIF" )
CheckForCoreSupport( TOWEL42_GIFSUPPORT "GIF" )
CheckForCoreSupport( TOWEL42_QAXOBJECT_SUPPORT "QAXObject" )
CheckForCoreSupport( TOWEL42_QCONCURRENT_SUPPORT "QConcurrent" )
CheckForCoreSupport( TOWEL42_QNETWORK_SUPPORT "QNetwork" )
CheckForCoreSupport( TOWEL42_QSQL_SUPPORT "QSql" )
CheckForCoreSupport( TOWEL42_QSVG_SUPPORT "QSvg" )
CheckForCoreSupport( TOWEL42_QWIDGET_SUPPORT "QWidget" )
CheckForCoreSupport( TOWEL42_QXMLPATTERNS_SUPPORT "QXmlPatterns" )
CheckForCoreSupport( TOWEL42_QXML_SUPPORT "QXml" )
CheckForCoreSupport( TOWEL42_ZIP_SUPPORT "ZIP" )
CheckForWidgetSupport( TOWEL42_DESIGNERPLUGIN_SUPPORT "Designer Plugins" )

if ( TOWEL42_QCORE_SUPPORT )
    add_definitions( -DTOWEL42_QCORE_SUPPORT=1)
    find_package(Qt6 COMPONENTS Core REQUIRED)
    IF(WIN32)
            set( QT_OS_SRCS 
                ForceUnbufferedProcessModifier.cpp 
                MoveToTrash_win.cpp 
                SystemInfo_win.cpp 
            )
            set( QT_OS_HEADERS 
                ${OS_HEADERS}
                ForceUnbufferedProcessModifier.h 
            )
    ELSE()
            set( QT_OS_SRCS 
                ${OS_SRCS}
                MoveToTrash_linux.cpp
                SystemInfo_linux.cpp 
            )
    ENDIF()

    set(qtproject_SRCS
        ${qtproject_SRCS}
        ${QT_OS_SRCS}
        BackgroundFileCheck.cpp
        BackupFile.cpp
        ExcelFuncs.cpp
        FileCompare.cpp
        FileUtils.cpp
        FileUtils_Remove.cpp
        FindAllFiles.cpp
        JsonUtils.cpp
        MD5.cpp
        MoveToTrash.cpp
        QtUtils.cpp
        StayAwake.cpp
        SystemInfo.cpp
        SystemLoggingDefs.cpp
        SetupSystemLogging.cpp
        Towel42UtilsResources.cpp
        TimeStamp.cpp
        UtilityModels.cpp
        utils.cpp
        WordExp.cpp
    )

    set(qtproject_H
        ${qtproject_H}
        BackgroundFileCheck.h
        MD5.h
        UtilityModels.h
    )
    
    set(project_H
        ${QT_OS_HEADERS}
        ${project_H}
        ExcelFuncs.h
        FileCompare.h
        FileUtils.h
        JsonUtils.h
        MetaUtils.h
        MoveToTrash.h
        QtHashUtils.h
        QtUtils.h
        StayAwake.h   
        SystemInfo.h
        SystemLoggingDefs.h
        SystemLoggingDefs_pri.h
        SetupSystemLogging.h
        TimeStamp.h
        Towel42UtilsResources.h
        utils.h
        WordExp.h
    )

    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Core
    )

    set(qtproject_QRC
        resources/Towel42Utils.qrc
    )
    if ( TOWEL42_QCORE_SUPPORT )
        file(GLOB qtproject_QRC_SOURCES "resources/*")
    endif()
endif()

if ( TOWEL42_QWIDGET_SUPPORT )
    add_definitions( -DTOWEL42_QWIDGET_SUPPORT=1)
    find_package(Qt6 COMPONENTS Widgets Gui UiPlugin REQUIRED)
    set(qtproject_SRCS
        ${qtproject_SRCS}
        About.cpp
        AutoSize.cpp
        AutoFetch.cpp
        AutoTabStop.cpp
        AutoWaitCursor.cpp
        ButtonEnabler.cpp
        ButtonGroupWDescriptiveText.cpp
        CollapsableGroupBox.cpp
        DelayComboBox.cpp
        DelayLineEdit.cpp
        DelaySpinBox.cpp
        DoubleProgressDlg.cpp
        FileBasedCache.cpp
        HyperLinkLineEdit.cpp
        ImageHandler.cpp
        ImageListWidget.cpp
        ImageScrollBar.cpp
        LineEditWithSuffix.cpp
        MenuBarEx.cpp
        ProgressBarProxyStyle.cpp
        QtDumper.cpp
        SetReadOnly.cpp
        SetReadOnlyComboBox.cpp
        SetReadOnlyGroupBox.cpp
        ScrollMessageBox.cpp
        SelectFileUrl.cpp
        SpinBox64.cpp
        SpinBox64U.cpp
        SummaryDateEdit.cpp
        SetupSystemLoggingDlg.cpp
        uiUtils.cpp
        UtilityViews.cpp
        WidgetChanged.cpp
        WidgetEnabler.cpp
    )
    set(qtproject_H
        ${qtproject_H}
        AutoTabStop.h
        AutoWaitCursor.h
        ButtonEnabler.h
        ButtonGroupWDescriptiveText.h
        CollapsableGroupBox.h
        DelayComboBox.h
        DelayLineEdit.h
        DelaySpinBox.h
        DoubleProgressDlg.h
        HyperLinkLineEdit.h
        ImageHandler.h
        ImageListWidget.h
        ImageScrollBar.h
        LineEditWithSuffix.h
        MenuBarEx.h
        SetReadOnlyComboBox.h
        SetReadOnlyGroupBox.h
        SummaryDateEdit.h
        SetupSystemLoggingDlg.h
        ScrollMessageBox.h
        SelectFileUrl.h
        SpinBox64.h
        SpinBox64U.h
        UtilityViews.h
        WidgetEnabler.h
    )
    set(project_H
        ${project_H}
        About.h
        AutoFetch.h
        AutoSize.h
        BackupFile.h
        FileBasedCache.h
        ProgressBarProxyStyle.h
        QtDumper.h
        SetReadOnly.h
        SpinBox64_StepType.h
        uiUtils.h
        WidgetChanged.h
        WidgetUtilsFwd.h
    )
    set(qtproject_UIS
        ${qtproject_UIS}
        DownloadFile.ui
        ScrollMessageBox.ui
        SetupSystemLoggingDlg.ui
    )
    
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Widgets
        Qt6::Gui
        Qt6::UiPlugin
    )
endif()

if ( TOWEL42_BIFSUPPORT )
    find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)
    if ( NOT TOWEL42_QWIDGET_SUPPORT  )
        MESSAGE( FATAL_ERROR "BIF Support requires QWidget Support" )
    endif()

    add_definitions( -DTOWEL42_BIFSUPPORT=1)
    set(qtproject_SRCS
        ${qtproject_SRCS}
        BIFFile.cpp
        BIFModel.cpp
        BIFWidget.cpp
    )
    set(qtproject_H
        ${qtproject_H}
        BIFModel.h
        BIFWidget.h
    )
    set(project_H
        ${project_H}
        BIFFile.h
    )
    set(qtproject_UIS
        ${qtproject_UIS}
        BIFWidget.ui
    )

    set(qtproject_QRC
        ${qtproject_QRC}
        BIFPlayerResources/BIFPlayerResources.qrc
    )

    file(GLOB tmp "BIFPlayerResources/*")
    SET( qtproject_QRC_SOURCES
        ${qtproject_QRC_SOURCES}
        ${tmp}
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Widgets
        Qt6::Gui
    )
    
endif()


if ( TOWEL42_GIFSUPPORT )
    find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)
    if ( NOT TOWEL42_QWIDGET_SUPPORT  )
        MESSAGE( FATAL_ERROR "GIF Support requires QWidget Support" )
    endif()
    
    add_definitions( -DTOWEL42_GIFSUPPORT=1)
    set(qtproject_SRCS
        ${qtproject_SRCS}
        GIFWriter.cpp
        GIFWriterDlg.cpp
    )
    set(qtproject_H
        ${qtproject_H}
        GIFWriterDlg.h
    )
    set(project_H
        ${project_H}
        GIFWriter.h
    )
    set(qtproject_UIS
        ${qtproject_UIS}
        GIFWriterDlg.ui
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Widgets
        Qt6::Gui
    )
endif()

if ( TOWEL42_QAXOBJECT_SUPPORT )
    find_package(Qt6 COMPONENTS AxContainer REQUIRED)
    add_definitions( -DTOWEL42_QAXOBJECT_SUPPORT=1)

    set(qtproject_SRCS
        ${qtproject_SRCS}
        VSInstallUtils.cpp
    )
    set(project_H
        ${project_H}
        VSInstallUtils.h
    )
    set(qtproject_H
        ${qtproject_H}
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::AxContainer
    )
endif()

if ( TOWEL42_QNETWORK_SUPPORT )
    find_package(Qt6 COMPONENTS Network REQUIRED)
    add_definitions( -DTOWEL42_QNETWORK_SUPPORT=1)
    set(qtproject_SRCS
        ${qtproject_SRCS}
        DownloadFile.cpp
        GitHubGetVersions.cpp
        ValidateOpenSSL.cpp
    )
    set(project_H
        ${project_H}
        ValidateOpenSSL.h
    )
    set(qtproject_H
        ${qtproject_H}
        DownloadFile.h
        GitHubGetVersions.h
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Network
    )
endif()

if ( TOWEL42_QCONCURRENT_SUPPORT )
    find_package(Qt6 COMPONENTS Concurrent REQUIRED)
    add_definitions( -DTOWEL42_QCONCURRENT_SUPPORT=1)
    set(qtproject_SRCS
        ${qtproject_SRCS}
        ThreadedProgressDialog.cpp
    )
    set(project_H
        ${project_H}
    )
    set(qtproject_H
        ${qtproject_H}
        ThreadedProgressDialog.h
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Concurrent
    )
endif()

IF ( TOWEL42_ZIP_SUPPORT )
    add_definitions( -DTOWEL42_ZIP_SUPPORT=1)
    find_package(Qt6 COMPONENTS Core REQUIRED)
    set(qtproject_H
        ${qtproject_H}
    )

    SET( project_H    
        ${project_H}
        ZIP.h
    )

    SET( qtproject_SRCS
        ${qtproject_SRCS}
        ZIP.cpp
    )
    
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Core
    )
    include_directories(${Qt6CorePrivate_INCLUDE_DIRS})
endif()

if ( TOWEL42_QSQL_SUPPORT )
    add_definitions( -DTOWEL42_QSQL_SUPPORT=1)
    find_package(Qt6 COMPONENTS Sql REQUIRED)
    set(qtproject_H
        ${qtproject_H}
    )

    SET( project_H    
        ${project_H}
        DBUtils.h
        DBUtilsFwd.h
    )

    SET( qtproject_SRCS
        ${qtproject_SRCS}
        DBUtils.cpp
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Sql
    )
endif()

if ( TOWEL42_QSVG_SUPPORT )
    find_package(Qt6 COMPONENTS Svg REQUIRED)
    add_definitions( -DTOWEL42_QSVG_SUPPORT=1)
    set(qtproject_H
        ${qtproject_H}
    )

    SET( project_H    
        ${project_H}
        SVGUtils.h
    )

    SET( qtproject_SRCS
        ${qtproject_SRCS}
        SVGUtils.cpp
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Svg
    )
endif()

