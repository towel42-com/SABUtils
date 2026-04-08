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
#ifndef __IMAGELISTWIDGET_H
#define __IMAGELISTWIDGET_H

#include "Towel42UtilsExport.h"
#include "WidgetUtilsFwd.h"
#include <QWidget>
#include <QListWidget>
#include <functional>

class QCheckBox;
class QSpacerItem;
class QToolButton;
namespace NTowel42Utils
{
    struct SImageData;

    class TOWEL42_UTILS_EXPORT CImageDropListWidget : public QListWidget
    {
        Q_OBJECT;
        Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
    public:
        CImageDropListWidget( QWidget *parent = nullptr );
        virtual void dragEnterEvent( QDragEnterEvent *event ) override;
        virtual void dropEvent( QDropEvent *event ) override;
        virtual QStringList mimeTypes() const override;
        void setReadOnly( bool readOnly );
        bool isReadOnly() const { return fReadOnly; }

    Q_SIGNALS:
        void sigImageFileDropped( const QString &filePath );
        void sigImageDropped( const QByteArray &data );

    private:
        bool fReadOnly{ false };
    };

    class TOWEL42_UTILS_EXPORT CImageListWidget : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
        Q_PROPERTY( bool hasImages READ hasImages WRITE setHasImages )
        Q_PROPERTY( QString imagesDescription READ imagesDescription WRITE setImagesDescription )
        Q_PROPERTY( bool autoNameImages READ autoNameImages WRITE setAutoNameImages )
    public:
        CImageListWidget( QWidget *parent = nullptr );
        ~CImageListWidget();

        void loadImages( const TImageDataList &images, const std::function< bool( TImageData image ) > &addImage = {} );
        TImageDataList getImages() const;

        void setAutoNameImages( bool autoName );
        bool autoNameImages() const { return fAutoName; }

        void setReadOnly( bool readOnly );
        bool isReadOnly() const { return fReadOnly; }

        void setImagesDescription( const QString &imagesDescription );
        QString imagesDescription() const;

        virtual QSize minimumSizeHint() const override;

        bool hasImages() const;
        void setHasImages( bool hasImages );

    Q_SIGNALS:
    public Q_SLOTS:
        void slotShowImagesChanged();
        void slotAddImage();
        void slotDelImage();
        void slotMoveImageUp();
        void slotMoveImageDown();
        void slotSelectionChanged( bool enabled );
        void slotEditItem();
        void slotImageDropped( const QByteArray &data );
        void slotImageFileDropped( const QString &filePath );

    private:
        QString selectImageWindowTitle() const;
        void setReadOnly( bool readOnly, bool force );
        std::shared_ptr< NTowel42Utils::SImageData > imageDataForItem( QListWidgetItem *curr ) const;
        void setupUi();
        bool loadImage( std::shared_ptr< NTowel42Utils::SImageData > imageData, QListWidgetItem *item = nullptr );

        bool fReadOnly{ false };

        QCheckBox *fCheckbox{ nullptr };
        QToolButton *fAddImage{ nullptr };
        QToolButton *fDelImage{ nullptr };
        QToolButton *fMoveUp{ nullptr };
        QToolButton *fMoveDown{ nullptr };
        CImageDropListWidget *fImages{ nullptr };
        QWidget *fSpacerWidget{ nullptr };

        QString fImagesDescription;
        bool fAutoName{ false };
    };
}
#endif
