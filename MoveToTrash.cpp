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

#include "MoveToTrash.h"

#ifdef QT_CORE_LIB
    #include <QFileInfo>
    #include <QDir>
#endif

#include "FileUtils.h"
#include "StringUtils.h"

#include <filesystem>
#include <iostream>

#include <cstdlib>
#include <fstream>

namespace NTowel42Utils
{
    namespace NFileUtils
    {
        bool moveToTrashImpl( const std::filesystem::path &path, std::wstring *msg, std::shared_ptr< SRecycleOptions > options );

#ifdef QT_CORE_LIB
        bool moveToTrash( const QFileInfo &info, QString *msg, std::shared_ptr< SRecycleOptions > options )
        {
            return moveToTrash( info.absoluteFilePath(), msg, options );
        }
#endif

        bool moveToTrash( const std::wstring &path, std::wstring *msg, std::shared_ptr< SRecycleOptions > options )
        {
            auto fi = std::filesystem::path( path ).is_absolute() ? std::filesystem::path( path ) : std::filesystem::absolute( path );
            return moveToTrash( fi, msg, options );
        }

        bool moveToTrash( const std::filesystem::path &path, std::wstring *msg, std::shared_ptr< SRecycleOptions > options )
        {
            if ( options->fVerbose )
                std::cout << "Starting Recycle of '" << path.string() << "'." << std::endl;

            bool aOK = moveToTrashImpl( path, msg, options );
            if ( !aOK )
            {
                if ( msg )
                    *msg = std::wstring( L"Could not move '" ) + path.wstring() + std::wstring( L"' to the recycle bin." );
                if ( options->fDeleteOnRecycleFailure )
                {
                    std::string lclMsg;
                    aOK = removePath( path.string(), &lclMsg );
                }
            }
            return aOK;
        }

#ifdef QT_CORE_LIB
        bool moveToTrash( const QString &path, QString *msg, std::shared_ptr< SRecycleOptions > options )
        {
            std::wstring lclMsg;
            bool retVal = moveToTrash( path.toStdWString(), &lclMsg, options );
            if ( msg )
                *msg = QString::fromStdWString( lclMsg );
            return retVal;
        }
#endif
        bool moveToTrashImpl_Linux( const std::filesystem::path &path, std::wstring *msg, std::shared_ptr< SRecycleOptions > options )
        {
            if ( !std::filesystem::exists( path ) )
            {
                if ( msg )
                    *msg = L"File or Directory '" + path.wstring() + L"' does not exist.";

                return true;
            }

            auto currentTime = std::chrono::system_clock::now();
            std::time_t currentTime_c = std::chrono::system_clock::to_time_t( currentTime );
            
            const char *home = std::getenv( "HOME" );
            if ( !home )
            {
                if ( msg )
                    *msg = L"Could not determine HOME directory";

                return false;
            }

            auto trashFilePath = std::filesystem::path( home ) / L".local/share/Trash/files/";   // trash file path contain delete files
            auto trashInfoPath = std::filesystem::path( home ) / L".local/share/Trash/info/";   // trash info path contain delete files information

            if ( !NFileUtils::mkdir( trashFilePath, true ) )
            {
                if ( msg )
                    *msg = L"Could not make directory '" + trashFilePath.wstring() + L"'";
                return false;
            }
            if ( !NFileUtils::mkdir( trashInfoPath, true ) )
            {
                if ( msg )
                    *msg = L"Could not make directory '" + trashFilePath.wstring() + L"'";
                return false;
            }

            // create file format for trash info file----- START
            auto infoFilePath = trashInfoPath / std::filesystem::path( path.wstring() + L".trashinfo" );
            auto infoFile = std::wofstream( infoFilePath, std::ios::out | std::ios::trunc );   //filename+extension+.trashinfo //  create file information file in /.local/share/Trash/info/ folder

            if ( !infoFile.is_open() )
            {
                if ( msg )
                    *msg = L"Could not open file '" + infoFilePath.wstring() + L"' for write";
                return false;
            }

            infoFile << "[Trash Info]" << "\n";
            infoFile << L"Path=" <<  NStringUtils::toPercentEncoding( infoFilePath.wstring(), L"~_-./" ) << L"\n";   // convert path string in percentage decoding scheme string
            
            infoFile << L"DeletionDate=" << std::put_time( std::localtime( &currentTime_c ), L"%FT%T" ) << L"\n";   // get date and time format YYYY-MM-DDThh:mm:ss

            infoFile.close();

            // create info file format of trash file----- END

            std::error_code ec;
            auto newPath = trashFilePath / path.filename();
            std::filesystem::rename( path, newPath, ec );   // rename(file old path, file trash path)
            if ( !ec && msg )
                *msg = L"Could not rename file '" + path.wstring() + L"' to '" + newPath.wstring() + L"'.";
            return ec.operator bool();
        }
    }
}
