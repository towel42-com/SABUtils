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
#include "SetReadOnly.h"
#include "ButtonEnabler.h"
#include "ImageHandler.h"
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QToolButton>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QIcon>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QBuffer>

namespace NTowel42Utils
{
    CImageListWidget::CImageListWidget( QWidget *parent /*= nullptr*/ ) :
        QWidget( parent ),
        fImagesDescription( tr( "Image" ) )

    {
        setupUi();

        connect( fCheckbox, &QCheckBox::toggled, this, &CImageListWidget::slotShowImagesChanged );
        connect( fAddImage, &QToolButton::clicked, this, &CImageListWidget::slotAddImage );
        connect( fDelImage, &QToolButton::clicked, this, &CImageListWidget::slotDelImage );
        connect( fMoveUp, &QToolButton::clicked, this, &CImageListWidget::slotMoveImageUp );
        connect( fMoveDown, &QToolButton::clicked, this, &CImageListWidget::slotMoveImageDown );
        setReadOnly( false, true );
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

        fCheckbox = new QCheckBox( this );
        fCheckbox->setObjectName( "fCheckbox" );
        fCheckbox->setText( tr( "Images:" ) );

        int rowNum = 0;
        gridLayout->addWidget( fCheckbox, rowNum++, 0, 1, 2 );

        fImages = new CImageDropListWidget( this );
        connect( fImages, &CImageDropListWidget::sigImageDropped, this, &CImageListWidget::slotImageDropped );
        connect( fImages, &CImageDropListWidget::sigImageFileDropped, this, &CImageListWidget::slotImageFileDropped );

        fImages->setObjectName( "fImages" );
        gridLayout->addWidget( fImages, rowNum, 0, 5, 1 );

        fAddImage = new QToolButton( this );
        fAddImage->setObjectName( "fAddImage" );
        {
            auto icon = QIcon::fromTheme( QIcon::ThemeIcon::ListAdd );
            fAddImage->setIcon( icon );
            fAddImage->setFixedSize( icon.availableSizes().front() );
        }

        gridLayout->addWidget( fAddImage, rowNum++, 1, 1, 1 );

        fDelImage = new QToolButton( this );
        fDelImage->setObjectName( "fDelImage" );
        {
            QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::EditDelete ) );
            fDelImage->setIcon( icon );
            fDelImage->setFixedSize( icon.availableSizes().front() );
        }

        gridLayout->addWidget( fDelImage, rowNum++, 1, 1, 1 );

        fMoveUp = new QToolButton( this );
        fMoveUp->setObjectName( "fMoveUp" );
        {
            QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::GoUp ) );
            fMoveUp->setIcon( icon );
            fMoveUp->setFixedSize( icon.availableSizes().front() );
        }
        gridLayout->addWidget( fMoveUp, rowNum++, 1, 1, 1 );

        fMoveDown = new QToolButton( this );
        fMoveDown->setObjectName( "moveDown" );
        {
            QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::GoDown ) );
            fMoveDown->setIcon( icon );
            fMoveDown->setFixedSize( icon.availableSizes().front() );
        }

        gridLayout->addWidget( fMoveDown, rowNum++, 1, 1, 1 );

        fSpacerWidget = new QWidget( this );
        fSpacerWidget->setObjectName( "fSpacerWidget" );
        auto vboxLayout = new QVBoxLayout( fSpacerWidget );
        auto spacer = new QSpacerItem( 20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding );
        vboxLayout->addItem( spacer );

        gridLayout->addWidget( fSpacerWidget, rowNum++, 1, 1, 1 );
        setSizePolicy( QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum );
    }

    void CImageListWidget::setReadOnly( bool readOnly )
    {
        setReadOnly( readOnly, false );
    }

    void CImageListWidget::setReadOnly( bool readOnly, bool force )
    {
        if ( !force && ( fReadOnly == readOnly ) )
            return;

        fReadOnly = readOnly;
        NTowel42Utils::setReadOnly( this, readOnly );
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

        if ( fReadOnly )
            disconnect( fImages, &QListWidget::itemDoubleClicked, this, &CImageListWidget::slotEditItem );
        else
            connect( fImages, &QListWidget::itemDoubleClicked, this, &CImageListWidget::slotEditItem );
    }

    void CImageListWidget::setImagesDescription( const QString &caption )
    {
        fImagesDescription = caption;
        if ( fImagesDescription.isEmpty() )
            fImagesDescription = tr( "Image" );

        slotShowImagesChanged();
        fCheckbox->setText( tr( "Has %1?" ).arg( fImagesDescription ) );
    }

    QString CImageListWidget::imagesDescription() const
    {
        return fImagesDescription;
    }

    QString CImageListWidget::selectImageWindowTitle() const
    {
        auto retVal = fImagesDescription;
        if ( retVal.isEmpty() )
            retVal = tr( "Image" );

        return tr( "Select %1:" ).arg( retVal );
    }

    void CImageListWidget::slotShowImagesChanged()
    {
        fAddImage->setVisible( fCheckbox->isChecked() );
        fDelImage->setVisible( fCheckbox->isChecked() );
        fMoveUp->setVisible( fCheckbox->isChecked() );
        fMoveDown->setVisible( fCheckbox->isChecked() );
        fImages->setVisible( fCheckbox->isChecked() );
        fSpacerWidget->setVisible( fCheckbox->isChecked() );
    }

    QSize CImageListWidget::minimumSizeHint() const
    {
        auto height = QWidget::minimumSizeHint().height();
        if ( fCheckbox->isChecked() )
        {
            height = std::max( height, fCheckbox->sizeHint().height() + fImages->sizeHint().height() + 20 );
            height = std::max( height, ( fAddImage->sizeHint().height() * 4 ) + 3 * 20 );
        }
        auto width = QWidget::minimumSizeHint().width();
        return { width, height };
    }

    bool CImageListWidget::hasImages() const
    {
        return fCheckbox->isChecked();
    }

    void CImageListWidget::setHasImages( bool hasImages )
    {
        fCheckbox->setChecked( hasImages );
    }

    void CImageListWidget::slotSelectionChanged( bool enabled )
    {
        auto upEnabled = enabled && ( fImages->currentRow() > 0 );
        fMoveUp->setEnabled( upEnabled );

        auto downEnabled = enabled && ( fImages->currentRow() < ( fImages->count() - 1 ) );
        fMoveDown->setEnabled( downEnabled );
    }

    TImageDataList CImageListWidget::getImages() const
    {
        std::list< QListWidgetItem * > items;
        for ( auto ii = 0; ii < fImages->count(); ++ii )
        {
            auto curr = fImages->item( ii );
            if ( !curr )
                continue;
            items.push_back( curr );
        }
        items.sort(
            [ this ]( QListWidgetItem *lhs, QListWidgetItem *rhs )
            {
                auto lhsIndex = fImages->model()->index( fImages->row( lhs ), 0 );
                auto rhsIndex = fImages->model()->index( fImages->row( rhs ), 0 );

                auto lhsRect = fImages->visualRect( lhsIndex );
                auto rhsRect = fImages->visualRect( rhsIndex );

                if ( lhsRect.y() != rhsRect.y() )
                    return lhsRect.y() < rhsRect.y();

                return lhsRect.x() < rhsRect.x();
            } );

        TImageDataList retVal;
        for ( auto &&ii : items )
        {
            auto currImageData = imageDataForItem( ii );
            if ( !currImageData )
                continue;
            retVal.push_back( currImageData );
        }
        return retVal;
    }

    void CImageListWidget::setAutoNameImages( bool autoName )
    {
        fAutoName = autoName;
    }

    std::shared_ptr< NTowel42Utils::SImageData > CImageListWidget::imageDataForItem( QListWidgetItem *curr ) const
    {
        if ( !curr )
            return {};
        auto data = curr->data( Qt::UserRole + 1 ).toByteArray();
        auto extraData = curr->data( Qt::UserRole + 2 );
        auto desc = curr->text();

        auto imageData = std::make_shared< NTowel42Utils::SImageData >( data, desc );
        imageData->setData( Qt::UserRole + 2, extraData );
        return imageData;
    }

    void CImageListWidget::loadImages( const TImageDataList &images, const std::function< bool( TImageData image ) > &addImage /*= {}*/ )
    {
        bool imageAdded = false;
        for ( auto &&imageData : images )
        {
            if ( addImage && !addImage( imageData ) )
                continue;

            imageAdded = loadImage( imageData ) || imageAdded;
        }
        fCheckbox->setChecked( imageAdded );
    }

    bool CImageListWidget::loadImage( std::shared_ptr< NTowel42Utils::SImageData > imageData, QListWidgetItem *item /*= nullptr*/ )
    {
        auto pm = imageData->pixmap();
        if ( !pm.has_value() )
        {
            QMessageBox::critical( this, tr( "Invalid image file" ), tr( "The image could not be loaded" ), QMessageBox::StandardButton::Ok );
            return false;
        }

        QIcon icon;
        icon.addPixmap( pm.value() );
        if ( icon.isNull() )
        {
            QMessageBox::critical( this, tr( "Invalid image file" ), tr( "The image could not be loaded" ), QMessageBox::StandardButton::Ok );
            return false;
        }
        if ( !item )
            item = new QListWidgetItem( fImages );

        item->setIcon( icon );
        auto description = imageData->fDescription;
        if ( description.isEmpty() )
            description = tr( "Image %1" ).arg( fImages->count() );

        item->setText( description );
        item->setData( Qt::UserRole + 1, imageData->fData );
        item->setData( Qt::UserRole + 2, imageData->data( Qt::UserRole + 2 ) );
        return true;
    }

    void CImageListWidget::slotEditItem()
    {
        if ( fReadOnly )
            return;
        auto item = fImages->currentItem();
        if ( !item )
            return;

        auto imageData = imageDataForItem( item );

        auto dlg = new CImageHandlerDlg( this );
        dlg->setImageData( imageData );
        dlg->setWindowTitle( selectImageWindowTitle() );
        connect(
            dlg, &CImageHandlerDlg::accepted,
            [ this, item, dlg ]()
            {
                auto imageData = dlg->imageData();
                loadImage( imageData, item );
                dlg->deleteLater();
            } );
        connect( dlg, &CImageHandlerDlg::rejected, [ this, dlg ]() { dlg->deleteLater(); } );
        dlg->open();
    }

    void CImageListWidget::slotImageDropped( const QByteArray &data )
    {
        auto imageData = SImageData::fromData( window(), data );
        if ( !imageData )
            return;
        loadImage( imageData );
    }

    void CImageListWidget::slotImageFileDropped( const QString &filePath )
    {
        auto imageData = SImageData::fromFile( window(), filePath );
        if ( !imageData )
            return;
        loadImage( imageData );
    }

    void CImageListWidget::slotAddImage()
    {
        if ( fReadOnly )
            return;

        auto dlg = new CImageHandlerDlg( true, this );
        dlg->setWindowTitle( selectImageWindowTitle() );
        connect(
            dlg, &CImageHandlerDlg::accepted,
            [ this, dlg ]()
            {
                auto imageData = dlg->imageData();
                loadImage( imageData );
                dlg->deleteLater();
            } );
        connect( dlg, &CImageHandlerDlg::rejected, [ this, dlg ]() { dlg->deleteLater(); } );
        dlg->open();
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

    CImageDropListWidget::CImageDropListWidget( QWidget *parent /*= nullptr */ ) :
        QListWidget( parent )
    {
        setIconSize( QSize( 128, 128 ) );
        setViewMode( QListView::ViewMode::IconMode );
        setDragDropOverwriteMode( false );
        setSelectionMode( QAbstractItemView::SingleSelection );
        setEditTriggers( QAbstractItemView::EditTrigger::NoEditTriggers );
    }

    void CImageDropListWidget::setReadOnly( bool readOnly )
    {
        NTowel42Utils::setReadOnly( this, readOnly );

        fReadOnly = readOnly;

        setDragDropMode( fReadOnly ? QAbstractItemView::NoDragDrop : QAbstractItemView::DragDrop );
        setDragEnabled( !fReadOnly );
        setDefaultDropAction( fReadOnly ? Qt::IgnoreAction : Qt::MoveAction );
        setAcceptDrops( !fReadOnly );
        setDropIndicatorShown( !fReadOnly );
        viewport()->setAcceptDrops( !fReadOnly );
    }

    void CImageDropListWidget::dragEnterEvent( QDragEnterEvent *event )
    {
        if ( event->mimeData()->hasUrls() || event->mimeData()->hasImage() )
            event->accept();
        else
            event->ignore();
    }

    void CImageDropListWidget::dropEvent( QDropEvent *event )
    {
        if ( event->mimeData()->hasUrls() )
        {
            auto urls = event->mimeData()->urls();
            for ( auto &&ii : urls )
            {
                auto url = ii.toLocalFile();
                if ( url.isEmpty() )
                    continue;
                emit sigImageFileDropped( url );
            }
        }
        else if ( event->mimeData()->hasImage() )
        {
            auto image = qvariant_cast< QImage >( event->mimeData()->imageData() );
            if ( !image.isNull() )
            {
                QByteArray byteArray;
                QBuffer buffer( &byteArray );
                buffer.open( QIODevice::WriteOnly );
                image.save( &buffer, "PNG" );
                emit sigImageDropped( byteArray );
            }
        }
    }

    QStringList CImageDropListWidget::mimeTypes() const
    {
        auto retVal = QListWidget::mimeTypes();
        retVal << QStringLiteral( "text/uri-list" ) << QStringLiteral( "image/*" );
        return retVal;
    }
}
