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
#include "ImageListWidget.h"
#include "setReadOnly.h"
#include "ButtonEnabler.h"
#include "ImageHandler.h"
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QToolButton>
#include <QListWidget>
#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include <QIcon>

namespace NTowel42Utils
{
    CImageListWidget::CImageListWidget( QWidget *parent /*= nullptr*/ ) :
        QWidget( parent )
    {
        setupUi();

        connect( fAddImage, &QToolButton::clicked, this, &CImageListWidget::slotAddImage );
        connect( fDelImage, &QToolButton::clicked, this, &CImageListWidget::slotDelImage );
        connect( fMoveUp, &QToolButton::clicked, this, &CImageListWidget::slotMoveImageUp );
        connect( fMoveDown, &QToolButton::clicked, this, &CImageListWidget::slotMoveImageDown );
        setReadOnly( false );
    }

    CImageListWidget::~CImageListWidget()
    {
    }

    void CImageListWidget::setupUi()
    {
        if ( objectName().isEmpty() )
            setObjectName( "NTowel42Utils::CImageListWidget" );

        auto gridLayout = new QGridLayout( this );
        gridLayout->setObjectName( "gridLayout_3" );
        gridLayout->setContentsMargins( 0, 0, 0, 0 );
        fLabel = new QLabel( this );
        fLabel->setObjectName( "fLabel" );
        fLabel->setText( tr( "Images:" ) );

        int rowNum = 0;
        gridLayout->addWidget( fLabel, rowNum++, 0, 1, 2 );

        fImages = new QListWidget( this );
        fImages->setObjectName( "fImages" );
        fImages->setIconSize( QSize( 128, 128 ) );
        fImages->setViewMode( QListView::ViewMode::IconMode );

        gridLayout->addWidget( fImages, rowNum, 0, 5, 1 );

        fAddImage = new QToolButton( this );
        fAddImage->setObjectName( "fAddImage" );
        {
            QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::ListAdd ) );
            fAddImage->setIcon( icon );
        }

        gridLayout->addWidget( fAddImage, rowNum++, 1, 1, 1 );

        fDelImage = new QToolButton( this );
        fDelImage->setObjectName( "fDelImage" );
        {
            QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::EditDelete ) );
            fDelImage->setIcon( icon );
        }

        gridLayout->addWidget( fDelImage, rowNum++, 1, 1, 1 );

        fMoveUp = new QToolButton( this );
        fMoveUp->setObjectName( "fMoveUp" );
        {
            QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::GoUp ) );
            fMoveUp->setIcon( icon );
        }
        gridLayout->addWidget( fMoveUp, rowNum++, 1, 1, 1 );

        fMoveDown = new QToolButton( this );
        fMoveDown->setObjectName( "moveDown" );
        {
            QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::GoDown ) );
            fMoveDown->setIcon( icon );
        }

        gridLayout->addWidget( fMoveDown, rowNum++, 1, 1, 1 );

        auto spacer = new QSpacerItem( 20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding );

        gridLayout->addItem( spacer, rowNum++, 1, 1, 1 );
    }

    void CImageListWidget::setReadOnly( bool readOnly )
    {
        if ( fReadOnly == readOnly )
            return;

        fReadOnly = readOnly;
        if ( !fReadOnly )
        {
            new NTowel42Utils::CButtonEnabler( fImages, fDelImage, this );

            auto enabler = new NTowel42Utils::CButtonEnabler( fImages, fMoveUp, this );
            connect( enabler, &NTowel42Utils::CButtonEnabler::sigSelectionChanged, this, &CImageListWidget::slotSelectionChanged );

            enabler = new NTowel42Utils::CButtonEnabler( fImages, fMoveDown, this );
            connect( enabler, &NTowel42Utils::CButtonEnabler::sigSelectionChanged, this, &CImageListWidget::slotSelectionChanged );
        }
        else
        {
            auto enablers = findChildren< NTowel42Utils::CButtonEnabler * >();
            for ( auto &&enabler : enablers )
            {
                delete enabler;
            }
        }
        fImages->setEditTriggers( fReadOnly ? QAbstractItemView::EditTrigger::NoEditTriggers : ( QAbstractItemView::EditTrigger::DoubleClicked | QAbstractItemView::EditTrigger::SelectedClicked ) );

        NTowel42Utils::setReadOnly( this, readOnly );
    }

    void CImageListWidget::slotSelectionChanged( bool enabled )
    {
        auto upEnabled = enabled && ( fImages->currentRow() > 0 );
        fMoveUp->setEnabled( upEnabled );

        auto downEnabled = enabled && ( fImages->currentRow() < ( fImages->count() - 1 ) );
        fMoveDown->setEnabled( downEnabled );
    }

    std::list< std::shared_ptr< NTowel42Utils::SImageData > > CImageListWidget::getImages() const
    {
        std::list< std::shared_ptr< NTowel42Utils::SImageData > > retVal;
        for ( auto ii = 0; ii < fImages->count(); ++ii )
        {
            auto curr = fImages->item( ii );
            if ( !curr )
                continue;
            auto data = curr->data( Qt::UserRole + 1 ).toByteArray();
            auto extraData = curr->data( Qt::UserRole + 2 );
            auto desc = curr->text();

            auto imageData = std::make_shared< NTowel42Utils::SImageData >( extraData, data, desc );
            retVal.push_back( imageData );
        }
        return retVal;
    }

    void CImageListWidget::loadImages( const std::list< std::shared_ptr< NTowel42Utils::SImageData > > &images )
    {
        for ( auto &&imageData : images )
        {
            loadImage( imageData );
        }
    }

    void CImageListWidget::slotAddImage()
    {
        if ( fReadOnly )
            return;

        auto dlg = new CImageHandlerDlg( this );
        connect( dlg, &CImageHandlerDlg::accepted, [this, dlg]()
                 {
                     auto imageData = dlg->imageData();
                     loadImage( imageData );
                     dlg->deleteLater();
            } );
        connect( dlg, &CImageHandlerDlg::rejected, [ this, dlg ]() { dlg->deleteLater(); } );
        dlg->open();
    }

    void CImageListWidget::loadImage( std::shared_ptr< NTowel42Utils::SImageData > imageData )
    {
        auto pm = imageData->pixmap();
        if ( !pm.has_value() )
        {
            QMessageBox::critical( this, tr( "Invalid image file" ), tr( "The file could not be processed" ), QMessageBox::StandardButton::Ok );
            return;
        }

        QIcon icon;
        icon.addPixmap( pm.value() );
        if ( icon.isNull() )
        {
            QMessageBox::critical( this, tr( "Invalid image file" ), tr( "The file could not be processed" ), QMessageBox::StandardButton::Ok );
            return;
        }
        auto item = new QListWidgetItem( icon, imageData->fDescription, fImages );

        item->setData( Qt::UserRole + 1, imageData->fData );
        item->setData( Qt::UserRole + 2, imageData->fExtraData );
    }

    void CImageListWidget::slotDelImage()
    {
        auto item = fImages->currentItem();
        delete fImages->currentItem();
    }

    void CImageListWidget::slotMoveImageUp()
    {
        if ( fReadOnly )
            return;

        auto currentRow = fImages->currentRow();
        if ( currentRow <= 0 )
            return;

        auto item = fImages->takeItem( currentRow );
        fImages->insertItem( currentRow - 1, item );
    }

    void CImageListWidget::slotMoveImageDown()
    {
        if ( fReadOnly )
            return;

        auto currentRow = fImages->currentRow();
        if ( currentRow < 0 )
            return;
        if ( currentRow == ( fImages->count() - 1 ) )
            return;

        auto item = fImages->takeItem( currentRow );
        fImages->insertItem( currentRow + 1, item );
    }
}
