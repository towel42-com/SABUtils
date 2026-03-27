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
#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include "Towel42UtilsExport.h"

#include <QWidget>
#include <QByteArray>
#include <QVariant>
#include <memory>
#include <optional>

class QToolButton;
class QLineEdit;
class QPushButton;
class QFrame;
namespace NTowel42Utils
{
    enum EImageType
    {
        eAvatar = 0,
        eImage = 1
    };

    struct SSizeHash
    {
        std::size_t operator()( const QSize &sz ) const;
    };

    struct TOWEL42_UTILS_EXPORT SImageData
    {
        SImageData() = default;
        SImageData( const QVariant & extraData, const QByteArray &data, const QString &description );

        std::optional< QPixmap > pixmap( const std::optional< QSize > &sz = {} ) const;
        void addSize(const QSize & sz);
        QVariant fExtraData;   // often the ID in a database
        QByteArray fData;
        QString fDescription;
        mutable std::unordered_map< QSize, std::optional< QPixmap >, SSizeHash > fPixmaps;
    };

    class TOWEL42_UTILS_EXPORT CImageHandler : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY( bool readOnly MEMBER fReadOnly NOTIFY sigReadOnlyChanged );
        Q_PROPERTY( EImageType imageType MEMBER fImageType NOTIFY sigImageTypeChanged );

    public:
        CImageHandler( QWidget *parent = nullptr );
        ~CImageHandler();

        std::shared_ptr< SImageData > imageData() const;
        void setImageData( std::shared_ptr< SImageData > imageData );

        void setImageType( EImageType type );
        EImageType imageType() const { return fImageType; }
    Q_SIGNALS:
        void sigReadOnlyChanged();
        void sigImageTypeChanged();
    public Q_SLOTS:
    private Q_SLOTS:
        void slotSelectImage();
        void slotReadOnlyChanged();
        void slotImageTypeChanged();

    private:
        void setImageDataInt( const QByteArray &imageData );
        void setupUi();
        void loadDefaultImage();
        QSize computeSize() const;
        void setImageFile( const QString &fileName );
        void setImagePixmap();

    private:
        void layout();

        std::shared_ptr< SImageData > fImageData;
        bool fReadOnly{ false };
        EImageType fImageType{ EImageType::eImage };
        bool fLayoutDirty{ true };

        QFrame *fFrame{ nullptr };
        QToolButton *fImageButton{ nullptr };
        QPushButton *fClearButton{ nullptr };
        QLineEdit *fDescription{ nullptr };
    };
}
#endif
