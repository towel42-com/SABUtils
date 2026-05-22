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

#ifndef __FROMSTRING_H
#define __FROMSTRING_H

#include "Towel42UtilsExport.h"

#include <string>
#include <sstream>
#include <cstdint>
#include <type_traits>
#include <iostream>

namespace NTowel42Utils
{
    template< class T >
    bool fromString( T &retVal, const char *arg )
    {
        if ( !arg || !*arg )
            return false;
        std::stringstream ss( arg );
        ss >> retVal;
        char c;
        if ( ss.fail() || ss.get( c ) )
            return false;
        return true;
    }

    template< typename T, typename = std::enable_if_t< !std::is_integral< T >::value > >
    bool fromString( T &retVal, const std::string &arg )
    {
        return fromString( retVal, arg.c_str() );
    }

    TOWEL42_UTILS_EXPORT bool fromChar( int &retVal, char ch, int base );

    template< typename T >
    std::enable_if_t< std::is_same_v< T, std::int64_t > || std::is_same_v< T, std::uint64_t >, bool > fromStringEx( T &retVal, const std::string_view &arg, int base = 10 )
    {
        retVal = 0;
        bool aOK = false;
        for ( auto &&currChar : arg )
        {
            int currVal = 0;
            if ( !fromChar( currVal, currChar, base ) )
            {
                std::cerr << "Invalid character: " << currChar << std::endl;
                return 0;
            }
            retVal = ( retVal * base ) + currVal;
        }
        return retVal;
    }

    template< typename T >
    std::enable_if_t< std::is_same_v< T, std::int64_t >, bool > fromString( T &retVal, const std::string_view &arg, int base = 10 )
    {
        return fromStringEx( retVal, arg, base );
    }

    template< typename T >
    std::enable_if_t< std::is_same_v< T, std::uint64_t >, bool > fromString( T &retVal, const std::string_view &arg, int base = 10 )
    {
        return fromStringEx( retVal, arg, base );
    }

    template< typename T, typename = std::enable_if_t< std::is_integral< T >::value && !std::is_same< T, std::int64_t >::value && !std::is_same< T, std::uint64_t >::value > >
    bool fromString( T &retVal, const std::string_view &arg, int base = 10 )
    {
        if constexpr ( std::is_signed_v< T > )
        {
            int64_t tmpVal = 0;
            if ( !fromString( tmpVal, arg, base ) )
                return false;
            if ( tmpVal <= std::numeric_limits< T >::max() && tmpVal >= std::numeric_limits< T >::min() )
            {
                retVal = static_cast< T >( tmpVal );
                return true;
            }
        }
        else
        {
            uint64_t tmpVal = 0;
            if ( !fromString( tmpVal, arg, base ) )
                return false;
            if ( tmpVal <= std::numeric_limits< T >::max() && tmpVal >= std::numeric_limits< T >::min() )
            {
                retVal = static_cast< T >( tmpVal );
                return true;
            }
        }
        return false;
    }

    TOWEL42_UTILS_EXPORT bool fromString( double &retVal, const std::string &arg );
    TOWEL42_UTILS_EXPORT bool fromString( double &retVal, const char *arg );
    TOWEL42_UTILS_EXPORT bool fromString( float &retVal, const std::string &arg );
    TOWEL42_UTILS_EXPORT bool fromString( float &retVal, const char *arg );
    TOWEL42_UTILS_EXPORT bool fromString( bool &retVal, const char *arg );
    TOWEL42_UTILS_EXPORT bool fromString( bool &retVal, const std::string &arg );
}
#endif
