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

#ifndef __MOVETOTRASH_H
#define __MOVETOTRASH_H

#include "Towel42UtilsExport.h"

#include <string>
#include <filesystem>
#include <memory>
#ifdef QT_CORE_LIB
class QFileInfo;
class QString;
#endif
#include <memory>


namespace NTowel42Utils
{
    namespace NFileUtils
    {
        struct TOWEL42_UTILS_EXPORT SRecycleOptions
        {
            SRecycleOptions() {}

            bool fDeleteOnRecycleFailure{ true };
            bool fForce{ false };
            bool fVerbose{ false };
            bool fInteractive{ false };
        };

#ifdef QT_CORE_LIB
        TOWEL42_UTILS_EXPORT bool moveToTrash( const QFileInfo &info, QString *msg = nullptr, std::shared_ptr< SRecycleOptions > options = {} );
        TOWEL42_UTILS_EXPORT bool moveToTrash( const QString &fileName, QString *msg = nullptr, std::shared_ptr< SRecycleOptions > options = {} );
#endif
        TOWEL42_UTILS_EXPORT bool moveToTrash( const std::wstring &fileName, std::wstring *msg = nullptr, std::shared_ptr< SRecycleOptions > options = {} );
        TOWEL42_UTILS_EXPORT bool moveToTrash( const std::filesystem::path &fileName, std::wstring *msg = nullptr, std::shared_ptr< SRecycleOptions > options = {} );
    }
}
#endif
