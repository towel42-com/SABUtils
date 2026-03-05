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

#ifndef __WINDOWSERROR_H
#define __WINDOWSERROR_H

#include "Towel42UtilsExport.h"
#ifdef TOWEL42_QCORE_SUPPORT
    #include <QString>
#endif
#include <string>

namespace NTowel42Utils
{
#ifdef TOWEL42_QCORE_SUPPORT
    TOWEL42_UTILS_EXPORT QString getWindowsError();   // windows only
    TOWEL42_UTILS_EXPORT QString getWindowsError( int errorCode );   // windows only
#endif
    TOWEL42_UTILS_EXPORT std::wstring getWindowsErrorStd();   // windows only
    TOWEL42_UTILS_EXPORT std::wstring getWindowsErrorStd( int errorCode );   // windows only
}
#endif
