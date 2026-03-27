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
#include "AvatarHandler.h"

#include <QByteArray>
#include <QImageReader>
#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>
#include <QFile>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include "T42-Utils/SVGUtils.h"

namespace NTowel42Utils
{
    static const QSize kImageSize( 100, 100 );

    CAvatarHandler::CAvatarHandler( QWidget *parent /*= nullptr*/ ) :
        QToolButton( parent )
    {
        setupUi();

        connect( this, &CAvatarHandler::sigReadOnlyChanged, this, &CAvatarHandler::slotReadOnlyChanged );
        connect( this, &CAvatarHandler::clicked, this, &CAvatarHandler::slotSelectAvatar );
    }

    void CAvatarHandler::setupUi()
    {
        setObjectName( "NTowel42Utils__CAvatarHandler" );
        setText( tr( "Click to Select Image File..." ) );
        loadDefaultAvatar();

        slotReadOnlyChanged();
    }

    void CAvatarHandler::loadDefaultAvatar()
    {
        setAvatarFile( ":/Towel42UtilsResources/avatar.png" );
        fAvatarData.reset();
    }

    CAvatarHandler::~CAvatarHandler()
    {
    }

    void CAvatarHandler::slotSelectAvatar()
    {
        if ( fReadOnly )
            return;

        QStringList supportedFormats;
        for ( const QByteArray &format : QImageReader::supportedImageFormats() )
        {
            supportedFormats.append( "*." + QString::fromUtf8( format ) );
        }

        QString filter = QString( "All Supported Images (%1)" ).arg( supportedFormats.join( " " ) );

        auto fileName = QFileDialog::getOpenFileName( this, tr( "Select Avatar File" ), {}, filter );
        if ( fileName.isEmpty() )
            return;

        setAvatarFile( fileName );
    }

    void CAvatarHandler::slotReadOnlyChanged()
    {
        setToolButtonStyle( fReadOnly ? Qt::ToolButtonIconOnly : Qt::ToolButtonTextUnderIcon );
        setAutoRaise( !fReadOnly );
        setCursor( fReadOnly ? Qt::ForbiddenCursor : Qt::ArrowCursor );
        resetMinimumSize();
    }

    std::pair< QByteArray, QVariant > CAvatarHandler::avatarData() const
    {
        if ( !fAvatarData.has_value() )
            return {};
        return fAvatarData.value();
    }

    void CAvatarHandler::setAvatarFile( const QString &fileName )
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

        setAvatarData( data, {} );
    }

    void CAvatarHandler::setAvatarData( const QByteArray &avatarData, const QVariant & extraData )
    {
        fAvatarData.reset();
        QPixmap pixmap;
        auto isSVG = NTowel42Utils::isSVG( avatarData );
        if ( isSVG )
        {
            auto pm = NTowel42Utils::getSVG( avatarData, QSize( 100, 100 ) );
            if ( pm.has_value() )
                pixmap = pm.value();
        }
        else
            pixmap.loadFromData( avatarData );

        if ( pixmap.isNull() )
            fAvatarData.reset();
        else
            fAvatarData = { avatarData, extraData };
        setAvatarPixmap( pixmap );
    }

    QSize CAvatarHandler::computeMinimumSize() const
    {
        int spacing = 6;
        auto height = 2 * spacing + kImageSize.height() + spacing;
        auto width = spacing + kImageSize.width() + spacing;

        if ( !fReadOnly )
        {
            QFontMetrics fm( font() );
            const QSize textSize = fm.size( 0, text() );
            height += textSize.height() + spacing;
            width = std::max( width, textSize.width() + 2 * spacing );
        }
        return { width, height };
    }

    void CAvatarHandler::setAvatarPixmap( const QPixmap &pixmap )
    {
        if ( pixmap.isNull() )
        {
            fAvatarData.reset();
            setIcon( QIcon() );
            loadDefaultAvatar();
            return;
        }

        auto pm = pixmap;
        if ( pm.size() != kImageSize )
            pm = pm.scaled( kImageSize, Qt::KeepAspectRatio );

        setIcon( QIcon( pm ) );
        setIconSize( kImageSize );
        resetMinimumSize();
    }

    void CAvatarHandler::resetMinimumSize()
    {
        setMinimumSize( computeMinimumSize() );
    }

}