#ifndef __T42UTILS_SETUPSYSTEMLOGGING_H
#define __T42UTILS_SETUPSYSTEMLOGGING_H
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

#include "Towel42UtilsExport.h"

#include <list>
#include <QString>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY( t42utils_base )

namespace NTowel42Utils
{
    namespace NSystemLogging
    {
        TOWEL42_UTILS_EXPORT void setupSystemLogging( const QString &appName, const QStringList &otherPrefixes, bool uiMode );   // must be called before any logging is done, and only once
        TOWEL42_UTILS_EXPORT void setupSystemLogging();   // can be called multiple times, but only after the overload above has been called

        TOWEL42_UTILS_EXPORT void init();

        struct SCategoryInfo
        {
private:
            SCategoryInfo( QLoggingCategory *category, bool owned );
public:
            SCategoryInfo( QLoggingCategory *category );
            SCategoryInfo( const QString &category );

            QString fName;
            bool fDebug{ false };
            bool fInfo{ false };
            bool fWarning{ false };
            bool fCritical{ false };
        };
        TOWEL42_UTILS_EXPORT std::list< SCategoryInfo > loggingCategoryNames( bool includeQt );

    }
}
#endif