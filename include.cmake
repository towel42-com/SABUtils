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
        MoveToTrash_win.cpp 
        WindowsError.cpp 
    )
    set( OS_HEADERS
        WindowsError.h 
    )
    
else()
    set( QCORE_SUPPORT_OS_SRCS 
        MoveToTrash_linux.cpp
    )
    set( OS_HEADERS
    )
endif()

set(project_SRCS 
    ${OS_SRCS}
#    BackupFile.cpp
    FileUtils.cpp
    FileUtils_Remove.cpp
    FromString.cpp
    MoveToTrash.cpp
    StringComparisonClasses.cpp
    StringUtils.cpp
)

set(project_H
    ${OS_HEADERS}
    FileUtils.h
    FromString.h
    MoveToTrash.h
    StringUtils.h
    StringComparisonClasses.h
)
unset( OS_HEADERS )
unset( OS_SRCS )
if ( Qt6Core_FOUND )
    set(qtproject_SRCS 
        BackupFile.cpp
        FindAllFiles.cpp
        MoveToTrash.cpp
        RegExUtils.cpp
        SystemLoggingDefs.cpp
    )

    set(project_H
        BackupFile.h
        MoveToTrash.h
        RegExUtils.h
        SystemLoggingDefs.h
        SystemLoggingDefs_pri.h
    )
endif()


if ( TOWEL42_QCORE_SUPPORT )
    IF(WIN32)
            set( QCORE_SUPPORT_OS_SRCS 
                ForceUnbufferedProcessModifier.cpp 
                SystemInfo_win.cpp 
                ConsoleUtils.cpp 
            )
            set( QCORE_SUPPORT_OS_HEADERS 
                ForceUnbufferedProcessModifier.h 
                ConsoleUtils.h 
            )
    ELSE()
            set( QCORE_SUPPORT_OS_SRCS 
                SystemInfo_linux.cpp 
            )
    ENDIF()

    set(qtproject_SRCS
        ${qtproject_SRCS}
        ${QCORE_SUPPORT_OS_SRCS}
        BackgroundFileCheck.cpp
        CustomerData.cpp
        ExcelFuncs.cpp
        FileCompare.cpp
        FileSIDInfo.cpp
        GPUDetect.cpp
        JsonUtils.cpp
        MD5.cpp
        QtUtils.cpp
        SetupSystemLogging.cpp
        StayAwake.cpp
        SystemInfo.cpp
        TimeStamp.cpp
        Towel42UtilsResources.cpp
        UtilityModels.cpp
        VersionInfoData.cpp
        WordExp.cpp
        utils.cpp
    )

    set(qtproject_H
        ${qtproject_H}
        BackgroundFileCheck.h
        MD5.h
        UtilityModels.h
    )
    
    set(project_H
        ${CMAKE_BINARY_DIR}/Towel42UtilsExport.h
        ${QCORE_SUPPORT_OS_HEADERS}
        ${project_H}
        ExcelFuncs.h
        FileCompare.h
        JsonUtils.h
        MetaUtils.h
        QtHashUtils.h
        QtUtils.h
        SetupSystemLogging.h
        StayAwake.h   
        SystemInfo.h
        TimeStamp.h
        Towel42UtilsResources.h
        VersionInfoData.h
        WordExp.h
        utils.h
        CantorHash.h
        CustomerData.h
        EnumUtils.h
        FileSIDInfo.h
        GPUDetect.h
        HashUtils.h
        RevertValue.h
        Towel42UtilsFwd.h
    )

    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Core
    )

    set(qtproject_QRC
        resources/Towel42Utils.qrc
    )
    file(GLOB qtproject_QRC_SOURCES "resources/*")
endif()

if ( TOWEL42_QWIDGETS_SUPPORT )
    find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)
    set(qtproject_SRCS
        ${qtproject_SRCS}
        About.cpp
        AutoSize.cpp
        AutoFetch.cpp
        AutoTabStop.cpp
        AutoWaitCursor.cpp
        BackgroundFileCheck.cpp
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
        QtUtils.cpp
        SetReadOnly.cpp
        SetReadOnlyComboBox.cpp
        SetReadOnlyGroupBox.cpp
        ScrollMessageBox.cpp
        SelectFileUrl.cpp
        SpinBox64.cpp
        SpinBox64U.cpp
        SummaryDateEdit.cpp
        SetupSystemLogging.cpp
        SetupSystemLoggingDlg.cpp
        SystemLoggingDefs.cpp
        TimeStamp.cpp
        uiUtils.cpp
        utils.cpp
        UtilityViews.cpp
        VersionInfoData.cpp
        WidgetChanged.cpp
        WidgetEnabler.cpp
    )
    set(qtproject_H
        ${qtproject_H}
        AutoTabStop.h
        AutoWaitCursor.h
        BackgroundFileCheck.h
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
        FileBasedCache.h
        ProgressBarProxyStyle.h
        QtDumper.h
        QtUtils.h
        SetReadOnly.h
        SpinBox64_StepType.h
        uiUtils.h
        utils.h
        SystemLoggingDefs.h
        SystemLoggingDefs_pri.h
        SetupSystemLogging.h
        TimeStamp.h
        VersionInfoData.h
        WidgetChanged.h
        WidgetUtilsFwd.h
    )

    set(qtproject_UIS
        ${qtproject_UIS}
        ScrollMessageBox.ui
        SetupSystemLoggingDlg.ui
    )
    
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Widgets
        Qt6::Gui
    )
endif()

if ( TOWEL42_BIFSUPPORT )
    find_package(Qt6 COMPONENTS Widgets Gui REQUIRED)
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

    if ( TOWEL42_GIFSUPPORT )
        set(qtproject_SRCS
            ${qtproject_SRCS}
            BIFToGIFWriterDlg.cpp
        )
        set(qtproject_H
            ${qtproject_H}
            BIFToGIFWriterDlg.h
        )
        set(qtproject_UIS
            ${qtproject_UIS}
            BIFToGIFWriterDlg.ui
        )
    endif()
    
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
    if ( NOT Qt6Widgets_FOUND  )
        MESSAGE( FATAL_ERROR "GIF Support requires QWidget Support" )
    endif()
    
    add_definitions( -DTOWEL42_GIFSUPPORT=1)
    set(qtproject_SRCS
        ${qtproject_SRCS}
        QtUtils.cpp
        GIFWriter.cpp
    )
    set(qtproject_H
        ${qtproject_H}
    )
    set(project_H
        ${project_H}
        QtUtils.h
        GIFWriter.h
    )
    set(qtproject_UIS
        ${qtproject_UIS}
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Widgets
        Qt6::Gui
    )
    
    if ( TOWEL42_BIFSUPPORT  )
        set(qtproject_SRCS
            ${qtproject_SRCS}
            BIFToGIFWriterDlg.cpp
        )
        set(qtproject_H
            ${qtproject_H}
            BIFToGIFWriterDlg.h
        )
        set(qtproject_UIS
            ${qtproject_UIS}
            BIFToGIFWriterDlg.ui
        )
    endif()

endif()

if ( TOWEL42_VSINSTALLER_SUPPORT )

    set(qtproject_SRCS
        ${qtproject_SRCS}
        VSInstallUtils.cpp
        AutoWaitCursor.cpp
    )
    set(project_H
        ${project_H}
        VSInstallUtils.h
    )
    set(qtproject_H
        ${qtproject_H}
        AutoWaitCursor.h
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::AxContainer
    )
endif()

if ( TOWEL42_QNETWORK_SUPPORT )
    set(qtproject_SRCS
        ${qtproject_SRCS}
        DownloadFile.cpp
        GitHubGetVersions.cpp
        ValidateOpenSSL.cpp
        VersionInfoData.cpp
    )
    set(project_H
        ${project_H}
        ValidateOpenSSL.h
        VersionInfoData.h
    )
    set(qtproject_H
        ${qtproject_H}
        DownloadFile.h
        GitHubGetVersions.h
    )

    set(qtproject_UIS
        ${qtproject_UIS}
        DownloadFile.ui
    )

    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Network
        Qt6::Widgets
    )
endif()

if ( TOWEL42_QCONCURRENT_SUPPORT )
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
        Qt6::Widgets
    )
endif()

IF ( TOWEL42_ZIP_SUPPORT )
    add_definitions( -DTOWEL42_ZIP_SUPPORT=1)
    find_package(Qt6 COMPONENTS Core REQUIRED)
    
    find_package(ZLIB REQUIRED PATHS C:/Users/scott.TOWEL42/Dropbox/home/bin/zlib)

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
        ZLIB::ZLIB
    )
    include_directories(${Qt6CorePrivate_INCLUDE_DIRS})
endif()

if ( TOWEL42_QSQL_SUPPORT )
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

if ( TOWEL42_DESIGNERPLUGIN_SUPPORT )
    set(qtproject_H
        ${qtproject_H}
    )

    SET( project_H    
        ${project_H}
    )

    SET( qtproject_SRCS
        ${qtproject_SRCS}
    )
    SET( project_pub_DEPS
        ${project_pub_DEPS}
    )
endif()