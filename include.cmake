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

set(_PROJECT_NAME Towel42Utils)
    
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
    FileSIDInfo.cpp
    GPUDetect.cpp
    RegExUtils.cpp
    StringComparisonClasses.cpp
    utils.cpp
    StringUtils.cpp
    FromString.cpp
    ${OS_SRCS}
)

set(qtproject_CPPMOC_SRCS
)

set(project_H
    CantorHash.h
    EnumUtils.h
    FileSIDInfo.h
    FromString.h
    HashUtils.h
    GPUDetect.h
    RevertValue.h
    RegExUtils.h
    Towel42UtilsExport.h
    StringComparisonClasses.h
    StringUtils.h
    ${OS_HEADERS}
)

set(qtproject_UIS
)


if ( NOT TOWEL42_QCORE_SUPPORT )
    if ( TOWEL42_QWIDGET_SUPPORT )
        MESSAGE( WARNING "QWidget Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()
    
    if ( TOWEL42_BIFSUPPORT )
        MESSAGE( WARNING "BIF Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()

    if ( TOWEL42_QNETWORK_SUPPORT )
        MESSAGE( WARNING "QNetwork Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()

    if ( TOWEL42_MKVUTILS )
        MESSAGE( WARNING "MKVUtils Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()
    
    if ( TOWEL42_GIFSUPPORT )
        MESSAGE( WARNING "GIF Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()

    if ( TOWEL42_QAXOBJECT_SUPPORT )
        MESSAGE( WARNING "QAXObject Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()
    
        if ( TOWEL42_ZIP_SUPPORT )
        MESSAGE( WARNING "ZIP Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()

    if ( TOWEL42_QCONCURRENT_SUPPORT )
        MESSAGE( WARNING "QConcurrent Support requires QtCore Support" )
        set( TOWEL42_QCORE_SUPPORT true )
    endif()
endif()

if ( TOWEL42_QCORE_SUPPORT )
    IF(WIN32)
            set( QT_OS_SRCS 
                MoveToTrash_win.cpp 
                SystemInfo_win.cpp 
                ForceUnbufferedProcessModifier.cpp 
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
        BackupFile.cpp
        BackgroundFileCheck.cpp
        FFMpegFormats.cpp
        MD5.cpp
        SystemInfo.cpp
        UtilityModels.cpp
        MoveToTrash.cpp
        FileUtils.cpp
        FileUtils_Remove.cpp
        FindAllFiles.cpp
        QtUtils.cpp
        JsonUtils.cpp
        FileCompare.cpp
        StayAwake.cpp
        Towel42UtilsResources.cpp
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
        FFMpegFormats.h
        FileUtils.h
        FileCompare.h
        QtHashUtils.h
        QtUtils.h
        utils.h
        SystemInfo.h
        MoveToTrash.h
        JsonUtils.h
        MetaUtils.h
        Towel42UtilsResources.h
        WordExp.h
        StayAwake.h   
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
    set(qtproject_SRCS
        ${qtproject_SRCS}
        AutoFetch.cpp
        AutoWaitCursor.cpp
        ButtonEnabler.cpp
        CollapsableGroupBox.cpp
        DelayLineEdit.cpp
        DelayComboBox.cpp
        DelaySpinBox.cpp
        DoubleProgressDlg.cpp
        HyperLinkLineEdit.cpp
        FileBasedCache.cpp
        ImageScrollBar.cpp
        LineEditWithSuffix.cpp
        MenuBarEx.cpp
        ProgressBarProxyStyle.cpp
        QtDumper.cpp
        ScrollMessageBox.cpp
        SelectFileUrl.cpp
        SpinBox64.cpp
        SpinBox64U.cpp
        UtilityViews.cpp
        uiUtils.cpp
        WidgetEnabler.cpp
        WidgetChanged.cpp
    )
    set(qtproject_H
        ${qtproject_H}
        AutoWaitCursor.h
        ButtonEnabler.h
        CollapsableGroupBox.h
        DelayLineEdit.h
        DelayComboBox.h
        DelaySpinBox.h
        DoubleProgressDlg.h
        ImageScrollBar.h
        HyperLinkLineEdit.h
        LineEditWithSuffix.h
        MenuBarEx.h
        SelectFileUrl.h
        ScrollMessageBox.h
        SpinBox64.h
        SpinBox64U.h
        UtilityViews.h
        WidgetEnabler.h
    )
    set(project_H
        ${project_H}
        FileBasedCache.h
        AutoFetch.h
        BackupFile.h
        ProgressBarProxyStyle.h
        QtDumper.h
        SpinBox64_StepType.h
        uiUtils.h
        WidgetChanged.h
    )
    set(qtproject_UIS
        ${qtproject_UIS}
        ScrollMessageBox.ui
        DownloadFile.ui
    )
    
    SET( project_pub_DEPS
        ${project_pub_DEPS}
        Qt6::Widgets
    )
endif()

if ( TOWEL42_BIFSUPPORT )
    if ( NOT TOWEL42_QWIDGET_SUPPORT  )
        MESSAGE( FATAL_ERROR "BIF Support requires QWidget Support" )
    endif()

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
endif()


if ( TOWEL42_GIFSUPPORT )
    if ( NOT TOWEL42_QWIDGET_SUPPORT  )
        MESSAGE( FATAL_ERROR "GIF Support requires QWidget Support" )
    endif()
    
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
endif()

if ( TOWEL42_MKVUTILS )
    set(qtproject_SRCS
        ${qtproject_SRCS}
        MKVUtils.cpp
        MediaInfo.cpp
        SetMKVTags.cpp
    )
    set(qtproject_H
        ${qtproject_H}
        SetMKVTags.h
        MediaInfo.h
    )
    set(project_H
        ${project_H}
        MKVUtils.h
    )
    set(qtproject_UIS
        ${qtproject_UIS}
        SetMKVTags.ui
    )

    SET( project_pub_DEPS
        ${project_pub_DEPS}
        MKVReader
        mediainfo
        Qt6::Multimedia
    )
endif()

if ( TOWEL42_QAXOBJECT_SUPPORT )
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
    
    include_directories(${Qt6CorePrivate_INCLUDE_DIRS})
endif()

