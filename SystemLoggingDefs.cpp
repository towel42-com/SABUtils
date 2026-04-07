// The MIT License( MIT )
//
// Copyright( c ) 2020-2026 Towel 42 Development, LLC and Scott Aron Bloom
// SPDX-License-Identifier: MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "SystemLoggingDefs.h"

Q_LOGGING_CATEGORY( t42utils_base, "Towel42Utils", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch, "Towel42Utils.autoFetch", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch_isVisible, "Towel42Utils.autoFetch.isVisible", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch_findFirstVisible, "Towel42Utils.autoFetch.findFirstVisible", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch_expandingIndex, "Towel42Utils.autoFetch.expandingIndex", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoSize, "Towel42Utils.autoSize", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoTabStop, "Towel42Utils.autoTabStop", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_backgroundFileCheck, "Towel42Utils.backgroundFileCheck", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_buttonEnabler, "Towel42Utils.buttonEnabler", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dbUtils, "Towel42Utils.dbUtils", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dbUtils_reportError, "Towel42Utils.dbUtils.reportError", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dbUtils_validateQuery, "Towel42Utils.dbUtils.validateQuery", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_doubleProgressDlg, "Towel42Utils.doubleProgressDlg", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_downloadFile, "Towel42Utils.downloadFile", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_fileUtils, "Towel42Utils.fileUtils", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_findAllFiles, "Towel42Utils.findAllFiles", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_GitHubGetVersions, "Towel42Utils.GitHubGetVersions", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_MD5, "Towel42Utils.MD5", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_getDate, "Towel42Utils.getDate", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_appendToLog, "Towel42Utils.appendToLog", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_toDate, "Towel42Utils.toDate", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dumpRow, "Towel42Utils.dumpRow", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_regExReplace, "Towel42Utils.regExReplace", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_setReadOnly, "Towel42Utils.setReadOnly", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_systemInfo, "Towel42Utils.systemInfo", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_widgetChanged, "Towel42Utils.widgetChanged", QtMsgType::QtDebugMsg );

namespace NTowel42Utils
{
    namespace NSystemLogging
    {
        void initTowel42UtilCategories()
        {
            qCInfo( t42utils_base );
            qCInfo( t42utils_autoFetch );
            qCInfo( t42utils_autoFetch_isVisible );
            qCInfo( t42utils_autoFetch_findFirstVisible );
            qCInfo( t42utils_autoFetch_expandingIndex );
            qCInfo( t42utils_autoSize );
            qCInfo( t42utils_autoTabStop );
            qCInfo( t42utils_backgroundFileCheck );
            qCInfo( t42utils_buttonEnabler );
            qCInfo( t42utils_dbUtils );
            qCInfo( t42utils_dbUtils_reportError );
            qCInfo( t42utils_dbUtils_validateQuery );
            qCInfo( t42utils_doubleProgressDlg );
            qCInfo( t42utils_downloadFile );
            qCInfo( t42utils_fileUtils );
            qCInfo( t42utils_findAllFiles );
            qCInfo( t42utils_GitHubGetVersions );
            qCInfo( t42utils_MD5 );
            qCInfo( t42utils_getDate );
            qCInfo( t42utils_appendToLog );
            qCInfo( t42utils_toDate );
            qCInfo( t42utils_dumpRow );
            qCInfo( t42utils_regExReplace );
            qCInfo( t42utils_setReadOnly );
            qCInfo( t42utils_systemInfo );
            qCInfo( t42utils_widgetChanged );
        }
    }
}