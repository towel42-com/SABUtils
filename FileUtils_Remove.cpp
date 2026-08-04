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
#include "FileUtils.h"

#ifdef QT_CORE_LIB
    #include <QFileInfo>
    #include <QDir>
#endif

#include <filesystem>
#include <locale>
#include <codecvt>

namespace NTowel42Utils
{
    namespace NFileUtils
    {
        bool removePath( const std::string &path, std::string *msg )
        {
            if ( !std::filesystem::exists( path ) )
                return true;

            bool success = false;
            if ( std::filesystem::is_regular_file( path ) )
            {
                std::error_code ec;
                success = std::filesystem::remove( path, ec );
                if ( !success && msg )
                {
                    *msg += std::string( "\nCould not remove file '" ) + path + "': " + ec.message();
                }
            }
            else if ( std::filesystem::is_directory( path ) )
            {
                std::error_code ec;
                success = std::filesystem::remove_all( path, ec );
                if ( !success && msg )
                {
                    *msg += std::string( "\nCould not remove directory (recursively)'" ) + path + "': " + ec.message();
                }
            }
            return success;
        }

        bool removeInsideOfDir( const std::string &dir, std::string *msg )
        {
            if ( !std::filesystem::exists( dir ) )
                return true;

            if ( !std::filesystem::is_directory( dir ) )
            {
                if ( msg )
                    *msg = std::string( "Expected directory not a regular file'" + dir + "'." );
                return false;
            }

            return removePath( dir, msg );
        }

#ifdef QT_CORE_LIB
        bool removePath( const QString &path, QString *msg )
        {
            std::string localMsg;
            auto retVal = removePath( path.toStdString(), &localMsg );
            if ( msg )
                *msg = QString::fromStdString( localMsg );
            return retVal;
        }

        bool removeInsideOfDir( const QString &dirStr, QString *msg )
        {
            std::string localMsg;
            auto retVal = removeInsideOfDir( dirStr.toStdString(), &localMsg );
            if ( msg )
                *msg = QString::fromStdString( localMsg );
            return retVal;
        }
#endif
    }
}
