#ifndef __T42UTILS_SYSTEMLOGGINGDEFS_H
#define __T42UTILS_SYSTEMLOGGINGDEFS_H
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

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY( t42utils_base )
Q_DECLARE_LOGGING_CATEGORY( t42utils_autoFetch )
Q_DECLARE_LOGGING_CATEGORY( t42utils_autoFetch_isVisible )
Q_DECLARE_LOGGING_CATEGORY( t42utils_autoFetch_findFirstVisible )
Q_DECLARE_LOGGING_CATEGORY( t42utils_autoFetch_expandingIndex )
Q_DECLARE_LOGGING_CATEGORY( t42utils_autoSize )
Q_DECLARE_LOGGING_CATEGORY( t42utils_autoTabStop )
Q_DECLARE_LOGGING_CATEGORY( t42utils_backgroundFileCheck )
Q_DECLARE_LOGGING_CATEGORY( t42utils_buttonEnabler )
Q_DECLARE_LOGGING_CATEGORY( t42utils_dbUtils )
Q_DECLARE_LOGGING_CATEGORY( t42utils_dbUtils_reportError )
Q_DECLARE_LOGGING_CATEGORY( t42utils_dbUtils_validateQuery )
Q_DECLARE_LOGGING_CATEGORY( t42utils_doubleProgressDlg )
Q_DECLARE_LOGGING_CATEGORY( t42utils_downloadFile )
Q_DECLARE_LOGGING_CATEGORY( t42utils_fileUtils )
Q_DECLARE_LOGGING_CATEGORY( t42utils_findAllFiles )
Q_DECLARE_LOGGING_CATEGORY( t42utils_GitHubGetVersions )
Q_DECLARE_LOGGING_CATEGORY( t42utils_MD5 )
Q_DECLARE_LOGGING_CATEGORY( t42utils_getDate )
Q_DECLARE_LOGGING_CATEGORY( t42utils_appendToLog )
Q_DECLARE_LOGGING_CATEGORY( t42utils_toDate )
Q_DECLARE_LOGGING_CATEGORY( t42utils_dumpRow )
Q_DECLARE_LOGGING_CATEGORY( t42utils_regExReplace )
Q_DECLARE_LOGGING_CATEGORY( t42utils_setReadOnly ) 
Q_DECLARE_LOGGING_CATEGORY( t42utils_systemInfo )
Q_DECLARE_LOGGING_CATEGORY( t42utils_widgetChanged )
namespace NTowel42Utils
{
    namespace NSystemLogging
    {
        void initTowel42UtilCategories();
    }
}
#endif