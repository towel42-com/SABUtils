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
#ifndef IMAGELISTWIDGET_H
#define IMAGELISTWIDGET_H

#include "Towel42UtilsExport.h"
#include "WidgetUtilsFwd.h"
#include <QWidget>
#include <functional>

class QLabel;
class QToolButton;
class QListWidget;
class QListWidgetItem;
namespace NTowel42Utils
{
    struct SImageData;

    class TOWEL42_UTILS_EXPORT CImageListWidget : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
        Q_PROPERTY( QString caption READ caption WRITE setCaption )
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

        void setCaption( const QString &caption );
        QString caption() const;
    Q_SIGNALS:
    public Q_SLOTS:
        void slotAddImage();
        void slotDelImage();
        void slotMoveImageUp();
        void slotMoveImageDown();
        void slotSelectionChanged( bool enabled );
        void slotEditItem();

    private:
        void setReadOnly( bool readOnly, bool force );
        std::shared_ptr< NTowel42Utils::SImageData > imageDataForItem( QListWidgetItem *curr ) const;
        void setupUi();
        void loadImage( std::shared_ptr< NTowel42Utils::SImageData > imageData, QListWidgetItem *item=nullptr );

        bool fReadOnly{ false };

        QLabel *fLabel{ nullptr };
        QToolButton *fAddImage{ nullptr };
        QToolButton *fDelImage{ nullptr };
        QToolButton *fMoveUp{ nullptr };
        QToolButton *fMoveDown{ nullptr };
        QListWidget *fImages{ nullptr };

        QString fCaption;
        bool fAutoName{ false };
    };
}
#endif
