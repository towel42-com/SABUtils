// The MIT License( MIT )
//
// Copyright( c ) 2020-2025 Scott Aron Bloom
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

#include "WindowsError.h"
#include "StringUtils.h"

#ifdef TOWEL42_QCORE_SUPPORT
    #ifdef WIN32
        #include <qt_windows.h>
    #endif
#else
    #ifdef WIN32
        #include <windows.h>
    #endif
#endif
#ifdef min
    #undef min
#endif


namespace NTowel42Utils
{
#ifdef WIN32
    std::wstring getWindowsErrorStd()
    {
        auto errorID = ::GetLastError();
        return getWindowsErrorStd( errorID );
    }

    std::wstring getWindowsErrorStd( int errorID )
    {
        std::wstring ret;
        wchar_t *string = 0;
        FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPWSTR)&string, 0, NULL );
        if ( string )
            ret = string;
        LocalFree( (HLOCAL)string );

        if ( ret.empty() && errorID == ERROR_MOD_NOT_FOUND )
            ret = L"The specified module could not be found.";
        if ( ( ret.length() >= 2 ) && ( *std::rbegin( ret ) == L'\n' ) && ( *std::next( std::rbegin( ret ) ) == L'\n' ) )
        {
            ret.erase( std::rbegin( ret ).base() );
            ret.erase( std::rbegin( ret ).base() );
        }
        if ( ret.empty() )
        {
            ret = L"Unknown error 0x" + NStringUtils::toHex( errorID );
        }
        return ret;
    }
#else
    std::wstring getWindowsErrorStd()
    {
        return {};
    }
    std::wstring getWindowsErrorStd( int errorID )
    {
        (void)errorID;
        return {};
    }
#endif()

#ifdef TOWEL42_QCORE_SUPPORT
    QString getWindowsError()
    {
        return QString::fromStdWString( getWindowsErrorStd() );
    }

    QString getWindowsError( int errorID )
    {
        return QString::fromStdWString( getWindowsErrorStd( errorID ) );
    }
#endif
}

