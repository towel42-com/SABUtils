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
#ifndef __IMAGEHANDLER_H
#define __IMAGEHANDLER_H

#include "Towel42UtilsExport.h"

#include <QWidget>
#include <QDialog>
#include <QByteArray>
#include <QVariant>
#include <QVariantMap>
#include <memory>
#include <optional>
#include <cstdint>

class QToolButton;
class QLineEdit;
class QPushButton;
class QFrame;
namespace NTowel42Utils
{
    struct SSizeHash
    {
        std::size_t operator()( const QSize &sz ) const;
    };

    struct TOWEL42_UTILS_EXPORT SImageData
    {
        SImageData() = default;
        SImageData( const QByteArray &data, const QString &description );

        static std::shared_ptr< NTowel42Utils::SImageData > fromFile( QWidget *parent, const QString &fileName, const QString &description = {} );
        static std::shared_ptr< NTowel42Utils::SImageData > fromData( QWidget *parent, const QByteArray &data, const QString &description = {} );

        std::optional< QPixmap > pixmap( const std::optional< QSize > &sz = {} ) const;

        bool findLargestImageThatFits( int64_t sz );

        void setData( int role, const QVariant &value );
        QVariant data( int role, const QVariant &defaultValue = {} );
        void addSize( const QSize &sz );
        QByteArray fData;
        QString fDescription;
        mutable std::unordered_map< QSize, std::optional< QPixmap >, SSizeHash > fPixmaps;

        QVariantMap fExtraData;
    };
    using TImageData = std::shared_ptr< NTowel42Utils::SImageData >;
    using TImageDataList = std::list< TImageData >;

    class TOWEL42_UTILS_EXPORT CImageHandler : public QWidget
    {
        friend class CImageHandlerDlg;
        friend struct SImageData;

        Q_OBJECT
        Q_PROPERTY( bool readOnly MEMBER fReadOnly NOTIFY sigReadOnlyChanged );
        Q_PROPERTY( EImageType imageType MEMBER fImageType NOTIFY sigImageTypeChanged );

    public:
        enum EImageType
        {
            eAvatar = 0,
            eImage = 1
        };
        Q_ENUM( EImageType );

    public:
        CImageHandler( QWidget *parent = nullptr );
        ~CImageHandler();

        QString imageTypeText() const;

        std::shared_ptr< SImageData > imageData() const;
        void setImageData( std::shared_ptr< SImageData > imageData );

        void setImageType( CImageHandler::EImageType type, bool force = false );
        CImageHandler::EImageType imageType() const { return fImageType; }

        bool readOnly() const { return fReadOnly; }
        void setReadOnly( bool readOnly );

        static void setMaxImageSize( int64_t sz ) { sMaxImageSize = sz; }

        static bool checkFileSize( QWidget *parent, const QString &fileName );
        static bool checkImageSize( const QByteArray &imageData );
    Q_SIGNALS:
        void sigReadOnlyChanged();
        void sigImageTypeChanged();
    public Q_SLOTS:
    private Q_SLOTS:
        void slotSelectImage();

    private:
        void slotReadOnlyChanged();
        void slotImageTypeChanged();

    private:
        void setupUi();
        void loadDefaultImage();
        QSize computeSize() const;
        void setImageFile( const QString &fileName );
        void setImagePixmap();

    private:
        void layout();

        std::shared_ptr< SImageData > fImageData;
        bool fReadOnly{ false };
        CImageHandler::EImageType fImageType{ CImageHandler::EImageType::eImage };
        bool fLayoutDirty{ true };

        QFrame *fFrame{ nullptr };
        QToolButton *fImageButton{ nullptr };
        QPushButton *fClearButton{ nullptr };
        QLineEdit *fDescription{ nullptr };

        inline static std::optional< int64_t > sMaxImageSize;
    };

    class TOWEL42_UTILS_EXPORT CImageHandlerDlg : public QDialog
    {
        Q_OBJECT
        Q_PROPERTY( bool readOnly READ readOnly WRITE setReadOnly );
        Q_PROPERTY( CImageHandler::EImageType imageType READ imageType WRITE setImageType );

    public:
        CImageHandlerDlg( QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );
        CImageHandlerDlg( bool selectFileOnOpen, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags() );

    public:
        void setupUi();

        ~CImageHandlerDlg();

        std::shared_ptr< SImageData > imageData() const;
        void setImageData( std::shared_ptr< SImageData > imageData );

        void setImageType( CImageHandler::EImageType type );
        CImageHandler::EImageType imageType() const;

        void setReadOnly( bool readOnly );
        bool readOnly() const;

    private:
        CImageHandler *fImageHandler{ nullptr };
    };
}
#endif
