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

#ifndef __REGEXUTILS_H
#define __REGEXUTILS_H

#include "Towel42UtilsExport.h"
#include <string>
#ifdef TOWEL42_QCORE_SUPPORT
    #include <QString>
class QRegularExpression;
#endif
#include <list>
#include <optional>

namespace NTowel42Utils
{
    namespace NStringUtils
    {
        TOWEL42_UTILS_EXPORT bool regExEqual( const std::string &lhs, const std::string &rhs );

        TOWEL42_UTILS_EXPORT bool matchRegExpr( const char *s1, const char *s2 );

#ifdef TOWEL42_QCORE_SUPPORT
        TOWEL42_UTILS_EXPORT std::list< std::string > splitStringRegEx( const std::string &string, const std::string &regex, bool nocase = false, bool skipEmpty = false );   // split based on regex
        TOWEL42_UTILS_EXPORT bool isExactMatchRegEx( const std::string &data, const std::string &pattern, bool nocase );
        TOWEL42_UTILS_EXPORT QString encodeRegEx( const char *inString );
        TOWEL42_UTILS_EXPORT QString encodeRegEx( QString inString );
        TOWEL42_UTILS_EXPORT std::string encodeRegEx( const std::string &inString );
#endif

        TOWEL42_UTILS_EXPORT std::string addToRegEx( std::string oldRegEx, const std::string &regEx );

        TOWEL42_UTILS_EXPORT bool isSpecialRegExChar( char ch, bool includeDotSlash = true );
#ifdef TOWEL42_QCORE_SUPPORT
        TOWEL42_UTILS_EXPORT bool isSpecialRegExChar( const QChar &ch, bool includeDotSlash = true );

        // used in the regExReplace and regExReplaceAll
        // $$ - is replaced with the literal $
        // $& - is replaced with with captured text
        // $N - is replaced with the Nth captured text
        // ${name} - is replaced with the named capture
        TOWEL42_UTILS_EXPORT std::optional< QString > replaceMatch( const QString &replacement, QRegularExpressionMatch &match );

        // replaces all occurences of pattern in input with replacement
        TOWEL42_UTILS_EXPORT std::optional< QString > regExReplace( const QString &input, const QString &pattern, const QString &replacement );
        TOWEL42_UTILS_EXPORT std::optional< QString > regExReplace( const QString &input, const QRegularExpression &pattern, const QString &replacement );
#endif
    }
}
#endif
