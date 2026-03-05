// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
// SPDX-License-Identifier : MIT License
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
//
#ifndef ZIP_H
#define ZIP_H

// this comes from the private API in Qt

#include "Towel42UtilsExport.h"

#include <QDateTime>
#include <QString>
#include <QFile>

class QFileInfo;
namespace NTowel42Utils
{
    namespace NFileUtils
    {
        enum class EStatus
        {
            eNoError,
            eFileReadError,
            eFileWriteError,
            eFileOpenError,
            eFilePermissionsError,
            eFileError
        };

        QString toText( EStatus status );
        enum class ECompressionPolicy
        {
            eAlwaysCompress,
            eNeverCompress,
            eAutoCompress
        };

        struct SFileInfo
        {
            SFileInfo() noexcept;

            bool isValid() const noexcept { return fIsDir || fIsFile || fIsSymLink; }

            QString fFilePath;
            uint fIsDir    :1;
            uint fIsFile   :1;
            uint fIsSymLink:1;
            QFile::Permissions fPermissions{ 0 };
            uint fCRC{ 0 };
            qint64 fSize{ 0 };
            QDateTime fLastModified;
        };

        class CZipWriterPrivate;

        class TOWEL42_UTILS_EXPORT CZipWriter
        {
        public:
            CZipWriter() = delete;
            CZipWriter( const CZipWriter &rhs ) = delete;
            CZipWriter( const CZipWriter &&rhs ) = delete;
            CZipWriter operator=( const CZipWriter &rhs ) = delete;
            CZipWriter operator=( const CZipWriter &&rhs ) = delete;

            explicit CZipWriter( const QFileInfo &fileName, QIODevice::OpenMode mode = ( QIODevice::WriteOnly | QIODevice::Truncate ) );
            explicit CZipWriter( const QString &fileName, QIODevice::OpenMode mode = ( QIODevice::WriteOnly | QIODevice::Truncate ) );
            explicit CZipWriter( QIODevice *device );
            ~CZipWriter();

            QIODevice *device() const;

            bool isWritable() const;
            bool exists() const;

            EStatus status() const;
            QString statusText() const;

            void setCompressionPolicy( ECompressionPolicy policy );
            ECompressionPolicy compressionPolicy() const;

            QFile::Permissions setCreationPermissions( QFile::Permissions permissions ); // returns current permissions
            QFile::Permissions creationPermissions() const;

            void addItem( const SFileInfo &fileInfo, const std::optional< QByteArray > &data );
            void addFile( const QString &fileName, const QByteArray &data );
            void addFile( const QString &fileName, QIODevice *device );
            void addDirectory( const QString &dirName );
            void addSymLink( const QString &fileName, const QString &destination );

            void close();

        private:
            std::unique_ptr< CZipWriterPrivate > fImpl;
        };

        class CZipReaderPrivate;
        class TOWEL42_UTILS_EXPORT CZipReader
        {
        public:
            CZipReader() = delete;
            CZipReader( const CZipReader &rhs ) = delete;
            CZipReader( const CZipReader &&rhs ) = delete;
            CZipReader operator=( const CZipReader &rhs ) = delete;
            CZipReader operator=( const CZipReader &&rhs ) = delete;

            explicit CZipReader( const QFileInfo &fileName, QIODevice::OpenMode mode = QIODevice::ReadOnly );
            explicit CZipReader( const QString &fileName, QIODevice::OpenMode mode = QIODevice::ReadOnly );
            explicit CZipReader( QIODevice *device );
            ~CZipReader();

            QIODevice *device() const;

            bool isReadable() const;
            bool exists() const;

            QList< SFileInfo > fileInfoList() const;
            int count() const;

            std::optional< SFileInfo > entryInfoAt( int index ) const;
            std::optional< QByteArray > fileData( const QString &fileName ) const;

            bool extractAll( const QString &destinationDir ) const;

            EStatus status() const;
            QString statusText() const;

            void close();

        private:
            std::unique_ptr< CZipReaderPrivate > fImpl;
        };
    }
}

Q_DECLARE_TYPEINFO( NTowel42Utils::NFileUtils::SFileInfo, Q_MOVABLE_TYPE );
Q_DECLARE_TYPEINFO( NTowel42Utils::NFileUtils::EStatus, Q_PRIMITIVE_TYPE );
Q_DECLARE_TYPEINFO( NTowel42Utils::NFileUtils::ECompressionPolicy, Q_PRIMITIVE_TYPE );

#endif
