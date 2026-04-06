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
#include "ImageHandler.h"
#include "SVGUtils.h"
#include "SetReadOnly.h"
#include "CantorHash.h"
#include "uiUtils.h"

#include <QByteArray>
#include <QImageReader>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QToolButton>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QIcon>
#include <QRect>
#include <QTimer>

#include <QDialogButtonBox>

namespace NTowel42Utils
{
    static const QSize kImageSize( 100, 100 );

    CImageHandler::CImageHandler( QWidget *parent /*= nullptr*/ ) :
        QWidget( parent )
    {
        connect( this, &CImageHandler::sigReadOnlyChanged, this, &CImageHandler::slotReadOnlyChanged );
        connect( this, &CImageHandler::sigImageTypeChanged, this, &CImageHandler::slotImageTypeChanged );

        setupUi();

        connect( fImageButton, &QToolButton::clicked, this, &CImageHandler::slotSelectImage );
        connect( fClearButton, &QPushButton::clicked, [ & ]() { this->loadDefaultImage(); } );
        setImageType( EImageType::eImage );
    }

    void CImageHandler::setupUi()
    {
        fLayoutDirty = true;
        setObjectName( "NTowel42Utils__CImageHandler" );

        fFrame = new QFrame( this );
        fFrame->setObjectName( "fFrame" );
        fFrame->setFrameShape( QFrame::Shape::Box );
        fFrame->setFrameShadow( QFrame::Shadow::Plain );

        fImageButton = new QToolButton( this );
        fImageButton->setObjectName( "fImageToolButton" );

        fClearButton = new QPushButton( this );
        fClearButton->setObjectName( "fClearButton" );
        QIcon icon( QIcon::fromTheme( QIcon::ThemeIcon::EditClear ) );
        fClearButton->setIcon( icon );

        fDescription = new QLineEdit( this );
        fDescription->setObjectName( "fDescription" );
        fDescription->setPlaceholderText( tr( "Description:" ) );

        setImageType( EImageType::eImage, true );
        slotReadOnlyChanged();
        layout();
    }

    void CImageHandler::setImageType( EImageType type, bool force )
    {
        if ( force || ( fImageType != type ) )
        {
            fImageType = type;
            emit sigImageTypeChanged();

            auto clearText = tr( "Clear %1" ).arg( imageTypeText() );
            fClearButton->setToolTip( clearText );
            fClearButton->setText( clearText );
        }
    }

    void CImageHandler::setReadOnly( bool readOnly )
    {
        if ( fReadOnly == readOnly )
            return;
        fReadOnly = readOnly;
        emit sigReadOnlyChanged();
    }

    void CImageHandler::loadDefaultImage()
    {
        fLayoutDirty = true;
        if ( fImageType == EImageType::eAvatar )
            setImageFile( ":/Towel42UtilsResources/blank-avatar.png" );
        else if ( fImageType == EImageType::eImage )
            setImageFile( ":/Towel42UtilsResources/blank-image.png" );

        fImageData.reset();
        layout();
    }

    CImageHandler::~CImageHandler()
    {
    }

    void CImageHandler::slotSelectImage()
    {
        if ( fReadOnly )
            return;

        QStringList supportedFormats;
        for ( const QByteArray &format : QImageReader::supportedImageFormats() )
        {
            supportedFormats.append( "*." + QString::fromUtf8( format ) );
        }

        QString filter = QString( "All Supported Images (%1)" ).arg( supportedFormats.join( " " ) );

        auto fileName = QFileDialog::getOpenFileName( this, tr( "Select Image File" ), {}, filter );
        if ( fileName.isEmpty() )
            return;

        if ( !checkFileSize( window(), fileName ) )
            return;

        setImageFile( fileName );
    }

    bool CImageHandler::checkFileSize( QWidget *parent, const QString &fileName )
    {
        return checkImageSize( parent, QFileInfo( fileName ).size() );
    }

    bool CImageHandler::checkImageSize( QWidget *parent, int64_t sz )
    {
        if ( sMaxImageSize.has_value() && ( sz > sMaxImageSize.value() ) )
        {
            QLocale locale;
            QMessageBox::warning( parent, tr( "File Too Large" ), tr( "The selected file is larger than the maximum allowed size of %1 bytes." ).arg( locale.toString( sMaxImageSize.value() ) ), QMessageBox::StandardButton::Ok );
            return false;
        }
        return true;
    }

    void CImageHandler::slotReadOnlyChanged()
    {
        fImageButton->setToolButtonStyle( fReadOnly ? Qt::ToolButtonIconOnly : Qt::ToolButtonTextUnderIcon );
        fImageButton->setAutoRaise( !fReadOnly );
        fImageButton->setCursor( fReadOnly ? Qt::ForbiddenCursor : Qt::ArrowCursor );

        NTowel42Utils::setReadOnly( fClearButton, fReadOnly, false );
        NTowel42Utils::setReadOnly( fDescription, fReadOnly, false );

        fLayoutDirty = true;
        layout();
    }

    void CImageHandler::slotImageTypeChanged()
    {
        fLayoutDirty = true;
        auto typeName = ( fImageType == EImageType::eAvatar ) ? tr( "Avatar" ) : tr( "Image" );
        fImageButton->setText( tr( "Click to Select %1 File..." ).arg( typeName ) );

        fClearButton->setToolTip( tr( "Clear %1" ).arg( typeName ) );
        fClearButton->setText( tr( "Clear %1" ).arg( typeName ) );

        setWindowTitle( tr( "Select %1:" ).arg( typeName ) );
        loadDefaultImage();
        layout();
    }

    std::shared_ptr< SImageData > CImageHandler::imageData() const
    {
        auto retVal = std::make_shared< SImageData >();
        if ( fImageData )
        {
            retVal->fData = fImageData->fData;
            retVal->fDescription = fDescription->text();
            retVal->fExtraData = fImageData->fExtraData;
        }
        return retVal;
    }

    void CImageHandler::setImageFile( const QString &fileName )
    {
        auto file = QFile( fileName );
        if ( !file.open( QFile::ReadOnly ) )
            return;
        auto data = file.readAll();
        if ( data.isNull() )
        {
            QMessageBox::critical( this, tr( "Could not read file" ), tr( "Reading file '%1' produced no data" ).arg( fileName ), QMessageBox::StandardButton::Ok );
            return;
        }

        setImageDataInt( data );
    }

    void CImageHandler::setImageData( std::shared_ptr< SImageData > imageData )
    {
        fImageData = imageData;
        setImagePixmap();
    }

    void CImageHandler::setImageDataInt( const QByteArray &imageData )
    {
        if ( !fImageData )
            fImageData = std::make_shared< SImageData >();

        fImageData->fPixmaps.clear();
        fImageData->fData = imageData;
        fImageData->fDescription = fDescription->text();
        setImagePixmap();
    }

    void CImageHandler::setImagePixmap()
    {
        std::optional< QPixmap > pm;
        if ( fImageData )
            pm = fImageData->pixmap( QSize( 100, 100 ) );

        if ( !pm.has_value() )
        {
            fImageData.reset();
            fImageButton->setIcon( QIcon() );
            fDescription->setText( QString() );
            fLayoutDirty = true;
            loadDefaultImage();
            return;
        }

        fImageButton->setIcon( QIcon( pm.value() ) );
        fImageButton->setIconSize( kImageSize );
        fDescription->setText( fImageData->fDescription );
        fLayoutDirty = true;
        layout();
    }

    void CImageHandler::layout()
    {
        if ( !fLayoutDirty )
            return;

        setFixedSize( computeSize() );
        fLayoutDirty = false;
    }

    QSize CImageHandler::computeSize() const
    {
        int spacing = 6;

        // three rows
        // first row, the image toolbutton
        // second row, description text // fixed height, variable width
        // third row, clear button  - fixed height and width

        // first row
        auto width = 0;

        auto imageButtonSize = kImageSize;
        imageButtonSize.setHeight( spacing + imageButtonSize.height() );

        if ( !fReadOnly )
        {
            QFontMetrics fm( fImageButton->font() );
            const QSize textSize = fm.size( 0, fImageButton->text() );
            imageButtonSize.setHeight( imageButtonSize.height() + spacing + textSize.height() );
            imageButtonSize.setWidth( 2 * spacing + std::max( textSize.width(), imageButtonSize.width() ) );
        }
        else
            imageButtonSize.setWidth( kImageSize.width() + 2 * spacing );
        width = std::max( width, imageButtonSize.width() );
        imageButtonSize.setHeight( imageButtonSize.height() + spacing );
        auto imagePos = QPoint( spacing, spacing );
        auto height = imagePos.y() + imageButtonSize.height();
        auto y0 = imagePos.y() + imageButtonSize.height();

        // second row - line edit
        auto lineEditSize = QSize( width, fDescription->minimumSizeHint().height() );
        height += lineEditSize.height() + spacing;
        auto lineEditPos = QPoint( spacing, y0 );
        y0 += lineEditSize.height() + spacing;

        // the width of the line edit will expand or shrink as necessary since its in a layout

        // third row
        auto clearButtonSize = fClearButton->minimumSizeHint();
        height += spacing + clearButtonSize.height();
        width = std::max( width, clearButtonSize.width() );
        auto clearButtonPos = QPoint( spacing, y0 );
        y0 += clearButtonSize.height() + spacing;

        QFontMetrics fm( fDescription->font() );
        const QSize textSize = fm.size( 0, tr( "A simple image description" ) );
        auto minWidth = textSize.width();

        // when truncated, align right, otherwise center
        lineEditSize.setWidth( width );
        width = std::min( width, minWidth );
        width += 2 * spacing;

        auto computeX = [ spacing ]( int width, const QSize &sz )
        {
            if ( sz.width() > width )
                return width;
            return ( ( width - sz.width() ) / 2 );
        };

        imagePos.setX( computeX( width, imageButtonSize ) );
        lineEditPos.setX( computeX( width, lineEditSize ) );
        clearButtonPos.setX( computeX( width, clearButtonSize ) );

        fImageButton->setGeometry( QRect( imagePos, imageButtonSize ) );
        fDescription->setGeometry( QRect( lineEditPos, lineEditSize ) );
        fClearButton->setGeometry( QRect( clearButtonPos, clearButtonSize ) );

        fFrame->setGeometry( 0, 0, width, height );
        return { width, height };
    }

    QString CImageHandler::imageTypeText() const
    {
        return ( fImageType == EImageType::eImage ) ? tr( "Image" ) : tr( "Avatar" );
    }

    SImageData::SImageData( const QByteArray &data, const QString &description ) :
        fData( data ),
        fDescription( description )
    {
    }

    std::shared_ptr< NTowel42Utils::SImageData > SImageData::fromFile( const QString &fileName, const QString &description /*= {} */ )
    {
        QFile file( fileName );
        if ( !file.open( QFile::ReadOnly ) )
            return {};

        auto data = file.readAll();
        return fromData( data, description.isEmpty() ? QFileInfo( fileName ).fileName() : description );
    }

    std::shared_ptr< NTowel42Utils::SImageData > SImageData::fromData( const QByteArray &data, const QString &description /*= {} */ )
    {
        if ( !CImageHandler::checkImageSize( nullptr, data.size() ) )
        {
            return {};
        }
        auto retVal = std::make_shared< SImageData >( data, description );
        return retVal;
    }

    std::optional< QPixmap > SImageData::pixmap( const std::optional< QSize > &sz ) const
    {
        auto sizeKey = sz.has_value() ? sz.value() : QSize( -1, -1 );
        auto pos = fPixmaps.find( sizeKey );
        if ( pos != fPixmaps.end() )
            return ( *pos ).second;

        auto currPM = NTowel42Utils::pixmapForImageData( fData, sz );
        fPixmaps[ sizeKey ] = currPM;
        return currPM;
    }

    void SImageData::setData( int role, const QVariant &value )
    {
        fExtraData[ QString::number( role ) ] = value;
    }

    QVariant SImageData::data( int role, const QVariant &defaultValue /*= {} */ )
    {
        auto pos = fExtraData.find( QString::number( role ) );

        if ( pos == fExtraData.end() )
            return defaultValue;

        return ( *pos );
    }

    void SImageData::addSize( const QSize &sz )
    {
        auto pos = fPixmaps.find( sz );
        if ( pos == fPixmaps.end() )
        {
            fPixmaps.erase( pos );
        }

        fPixmaps[ sz ] = NTowel42Utils::pixmapForImageData( fData, sz );
    }

    std::size_t SSizeHash::operator()( const QSize &sz ) const
    {
        return NTowel42Utils::cantorHash( sz.width(), sz.height() );
    }

    CImageHandlerDlg::CImageHandlerDlg( QWidget *parent /*= nullptr*/, Qt::WindowFlags flags /*= Qt::WindowFlags() */ ) :
        QDialog( parent, flags )
    {
        setupUi();
    }

    CImageHandlerDlg::CImageHandlerDlg( bool selectFileOnOpen, QWidget *parent /*= nullptr*/, Qt::WindowFlags flags /*= Qt::WindowFlags() */ ) :
        CImageHandlerDlg( parent, flags )
    {
        if ( selectFileOnOpen )
            QTimer::singleShot( 0, fImageHandler, &CImageHandler::slotSelectImage );
    }

    void CImageHandlerDlg::setupUi()
    {
        if ( objectName().isEmpty() )
            setObjectName( "NTowel42Utils__CImageHandlerDlg" );
        auto layout = new QVBoxLayout( this );
        layout->setObjectName( "fLayout" );

        fImageHandler = new CImageHandler( this );
        fImageHandler->setObjectName( "fImageHandler" );
        layout->addWidget( fImageHandler );

        auto buttonBox = new QDialogButtonBox( this );
        buttonBox->setObjectName( "fButtonBox" );
        buttonBox->setOrientation( Qt::Orientation::Horizontal );
        buttonBox->setStandardButtons( QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Ok );
        QObject::connect( buttonBox, &QDialogButtonBox::accepted, this, qOverload<>( &QDialog::accept ) );
        QObject::connect( buttonBox, &QDialogButtonBox::rejected, this, qOverload<>( &QDialog::reject ) );

        layout->addWidget( buttonBox );

        connect( fImageHandler, &CImageHandler::sigImageTypeChanged, [ & ]() { setWindowTitle( tr( "Select %1:" ).arg( fImageHandler->imageTypeText() ) ); } );

        setFixedSize( minimumSizeHint() );
    }

    CImageHandlerDlg::~CImageHandlerDlg()
    {
    }

    std::shared_ptr< NTowel42Utils::SImageData > CImageHandlerDlg::imageData() const
    {
        return fImageHandler->imageData();
    }

    void CImageHandlerDlg::setImageData( std::shared_ptr< SImageData > imageData )
    {
        fImageHandler->setImageData( imageData );
    }

    void CImageHandlerDlg::setImageType( EImageType type )
    {
        fImageHandler->setImageType( type );
    }

    NTowel42Utils::EImageType CImageHandlerDlg::imageType() const
    {
        return fImageHandler->imageType();
    }

    void CImageHandlerDlg::setReadOnly( bool readOnly )
    {
        fImageHandler->setReadOnly( readOnly );
    }

    bool CImageHandlerDlg::readOnly() const
    {
        return fImageHandler->readOnly();
    }

}
