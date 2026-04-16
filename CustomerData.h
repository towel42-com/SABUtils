// The MIT License( MIT )
//
// Copyright( c ) 2022 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __CUSTOMERDATA_H
#define __CUSTOMERDATA_H

#include "Towel42UtilsExport.h"

#include <optional>
#include <string>
#include <utility>
#include <string_view>
#include <tuple>

#ifdef QT_CORE_LIB
    #include <QString>
    #include <QStringView>
    #include <QStringList>
#endif

namespace NTowel42Utils
{
    TOWEL42_UTILS_EXPORT std::optional< std::string > fixupPhoneNumber( const std::string &phoneNumber );
    TOWEL42_UTILS_EXPORT std::optional< std::pair< std::string, std::string > > splitByCountryCode( std::string_view phoneNumber );
    TOWEL42_UTILS_EXPORT bool isValidEmail( const std::string &email );

#ifdef QT_CORE_LIB
    TOWEL42_UTILS_EXPORT std::optional< QString > fixupPhoneNumber( const QString &phoneNumber );
    TOWEL42_UTILS_EXPORT std::optional< std::pair< QString, QString > > splitByCountryCode( const QString &phoneNumber );
    TOWEL42_UTILS_EXPORT bool isValidEmail( const QString &email );

    // returns pwOK, confirmPW OK
    TOWEL42_UTILS_EXPORT std::pair< bool, bool > confirmPassword( bool userTypeRequiresPassword, bool isNewUser, const QString &password, const QString &confirmPassword );

    #ifdef QT_SQL_LIB
    TOWEL42_UTILS_EXPORT QStringList stateList();
    using TCityStateZip = std::tuple< QString, QString, QString >;
    TOWEL42_UTILS_EXPORT std::optional< TCityStateZip > getCityStateForZip( const QString &zipCode, bool useStateAbbreviation );
    TOWEL42_UTILS_EXPORT std::optional< QString > getStateNameForAbbreviation( const QString &abbr );
    TOWEL42_UTILS_EXPORT bool isValidZipCode( const QString &zipCode );
    #endif
#endif

}

#endif
