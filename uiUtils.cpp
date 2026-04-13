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

#include "uiUtils.h"
#include "utils.h"
#include "SVGUtils.h"

#include <QFontMetrics>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QBuffer>
#include <QTransform>
#include <functional>
#include <unordered_set>
#include <set>

#ifdef Q_OS_WINDOWS
    #include <qt_windows.h>
    #include <shellapi.h>
#endif

namespace NTowel42Utils
{
    QWidget *determineLastFocusChild( const QWidget *target );

    bool launchIfURLClicked( const QString &title, const QPoint &pt, const QFont &font )
    {
        int urlStart;
        int urlLength;
        auto hasUrl = NTowel42Utils::isValidURL( title, &urlStart, &urlLength );
        if ( hasUrl )
        {
            auto xLoc = pt.x();
            if ( xLoc >= 30 )
            {
                xLoc -= 30;
                QFontMetrics fm( font );

                auto preURL = title.left( urlStart );
                auto url = title.mid( urlStart, urlLength );

                auto preRect = fm.boundingRect( preURL );
                if ( xLoc >= preRect.width() )
                {
                    xLoc -= preRect.width();
                    auto urlRect = fm.boundingRect( url );
                    if ( xLoc <= urlRect.width() )
                    {
                        return QDesktopServices::openUrl( url );
                    }
                }
            }
        }
        return false;
    }

    std::optional< QString > openUrl( const QUrl &url )
    {
        QCoreApplication *application = QCoreApplication::instance();
        if ( application && qobject_cast< QGuiApplication * >( application ) )
        {
            if ( QDesktopServices::openUrl( url ) )
                return {};
        }

#ifdef Q_OS_WIN
        auto urlPath = url.toString( QUrl::FullyEncoded );
        auto urlPathUtf16 = reinterpret_cast< const wchar_t * >( urlPath.utf16() );

        auto result = reinterpret_cast< uint64_t >( ShellExecute( nullptr, nullptr, urlPathUtf16, nullptr, nullptr, SW_SHOWNORMAL ) );
        if ( result > 32 )
            return {};
        return QObject::tr( "openUrl '%1' failed (error %2)." ).arg( urlPath ).arg( result );
#endif

        return QObject::tr( "Could not open url.  Run with QGuiApplication" );
    }

    bool setIsOK( bool aOK, QWidget *widget, const QString &widgetName )
    {
        if ( !aOK )
            widget->setStyleSheet( QStringLiteral( "%1 { color : red; }" ).arg( widgetName ) );
        else
            widget->setStyleSheet( QString() );
        return aOK;
    }

    bool setIsOK( bool aOK, QLabel *label )
    {
        return setIsOK( aOK, label, "QLabel" );
    }

    bool setIsOK( bool aOK, QCheckBox *cb )
    {
        return setIsOK( aOK, cb, "QCheckBox" );
    }

    bool setIsOK( bool aOK, QGroupBox *gb )
    {
        return setIsOK( aOK, gb, "QGroupBox" );
    }

    bool setIsOK( bool aOK, QTabWidget *tw, int index )
    {
        if ( !tw )
            return false;
        auto tabCount = tw->count();
        if ( ( index < 0 ) || ( index >= tabCount ) )
            return false;

        auto tmp = tw->tabBar()->tabButton( index, QTabBar::ButtonPosition::LeftSide );
        if ( !aOK )
            tw->tabBar()->setTabTextColor( index, QColor( "red" ) );
        else
            tw->tabBar()->setTabTextColor( index, QColor( "black" ) );
        return aOK;
    }

    QModelIndex selectFirstVisibleItem( QAbstractItemView *view, bool setFocus )
    {
        auto isVisibleIndex = [ view ]( const QModelIndex &idx )
        {
            return idx.isValid() && view->visualRect( idx ).isValid() && view->visualRect( idx ).intersects( view->viewport()->rect() );
        };

        if ( !view->model() )
            return {};

        auto rowCount = view->model()->rowCount();
        auto colCount = view->model()->columnCount();
        int row = 0;
        QModelIndex idx;
        bool foundVisible = false;
        do
        {
            int col = 0;
            do
            {
                idx = view->model()->index( row, col );
                foundVisible = isVisibleIndex( idx );
                col++;
            }
            while ( idx.isValid() && ( col < colCount ) && !foundVisible );
            row++;
        }
        while ( ( row < rowCount ) && !foundVisible );

        if ( idx.isValid() )
        {
            view->setCurrentIndex( idx );
        }
        if ( setFocus )
            view->setFocus();
        return idx;
    }

    void selectItemInComboBox( QComboBox *cb, const QVariant &value, bool *other )
    {
        if ( other )
            *other = false;
        if ( !cb )
            return;
        if ( value.isNull() )
            return;

        int pos = -1;
        if ( value.canConvert< QString >() )
        {
            pos = cb->findText( value.toString() );
        }
        if ( pos == -1 )
        {
            pos = cb->findData( value );
        }
        if ( pos == -1 )
        {
            if ( cb->itemText( 0 ).isEmpty() )
                pos = 0;
            else
            {
                pos = cb->findText( QObject::tr( "Other" ) );
                if ( other )
                    *other = pos != -1;
            }
        }
        cb->setCurrentIndex( pos );
    }

    void loadEnumValues( QComboBox *cb, const std::vector< std::pair< int, QString > > &enumValues, std::optional< int > blankValue )
    {
        if ( blankValue.has_value() )
        {
            cb->addItem( QString(), blankValue.value() );
        }
        for ( auto &&ii : enumValues )
        {
            cb->addItem( ii.second, ii.first );
        }
    }

    bool isValid( QLineEdit *edit, QLabel *label, std::function< bool( const QString &text ) > isValidFunc /*= {} */ )
    {
        if ( !edit || !label )
            return false;

        return isValid( edit->text(), label, isValidFunc );
    }

    bool isValid( QComboBox *cb, QLabel *label, std::function< bool( const QString &text ) > isValidFunc /*= {} */ )
    {
        if ( !cb || !label )
            return false;

        return isValid( cb->currentText(), label, isValidFunc );
    }

    bool isValid( const QString &text, QLabel *label, std::function< bool( const QString &text ) > isValidFunc /*= {} */ )
    {
        bool aOK = false;
        if ( isValidFunc )
            aOK = isValidFunc( text );
        else
            aOK = !text.isEmpty();

        return setIsOK( aOK, label );
    }

    bool isValid( QDateEdit *de, QLabel *label, std::function< bool( const QDate &date ) > isValidFunc /*= {} */ )
    {
        if ( !de || !label )
            return false;

        bool aOK = false;
        if ( isValidFunc )
            aOK = isValidFunc( de->date() );
        else
            aOK = de->date().isValid();

        return setIsOK( aOK, label );
    }

    bool isValid( QButtonGroup *bg, QLabel *label, std::function< bool( const QAbstractButton *btn ) > isValidFunc /*= {} */ )
    {
        if ( !bg || !label )
            return false;
        auto buttons = bg->buttons();
        QAbstractButton *checkedButton = nullptr;
        for ( auto &&ii : buttons )
        {
            if ( ii->isChecked() )
                checkedButton = ii;
        }

        bool aOK = false;
        if ( isValidFunc )
            aOK = isValidFunc( checkedButton );
        else
            aOK = checkedButton != nullptr;
        return setIsOK( aOK, label );
    }

    bool isValid( QCheckBox *cb, QLineEdit *desc, std::function< bool( bool checked, const QString &text ) > isValidFunc /*= {} */ )
    {
        if ( !cb || !desc )
            return false;

        bool aOK = false;
        if ( isValidFunc )
            aOK = isValidFunc( cb->isChecked(), desc->text() );
        else
        {
            aOK = !cb->isChecked() || !desc->text().isEmpty();
        }
        return setIsOK( aOK, cb );
    }

    bool isValid( QTextEdit *te, QLabel *label, std::function< bool( const QString &text ) > isValidFunc /*= {} */ )
    {
        if ( !te || !label )
            return false;

        return isValid( te->toPlainText(), label, isValidFunc );
    }

    bool isValid( QSpinBox *sb, QLabel *label, std::function< bool( int value ) > isValidFunc /*= {} */ )
    {
        if ( !sb || !label )
            return false;

        bool aOK = false;
        if ( isValidFunc )
            aOK = isValidFunc( sb->value() );
        else
            aOK = sb->value() != sb->minimum();
        return setIsOK( aOK, label );
    }

    std::optional< QPixmap > pixmapForImageData( const QByteArray &data, const std::optional< QSize > &sz )
    {
        if ( data.isNull() || data.isEmpty() )
            return {};

        std::optional< QPixmap > pixmap;
#ifdef TOWEL42_QSVG_SUPPORT
        auto isSVG = NTowel42Utils::isSVG( data );
        if ( isSVG )
        {
            pixmap = NTowel42Utils::getSVG( data, sz );
        }
        else
#endif
        {
            QBuffer buf( &const_cast< QByteArray & >( data ) );
            auto imageReader = QImageReader();
            imageReader.setAutoTransform( true );
            imageReader.setDevice( &buf );

            auto image = imageReader.read();
            if ( !image.isNull() )
            {
                //auto transforms = imageReader.transformation();
                //int rotationAngle = 0;
                //if ( ( transforms & QImageIOHandler::TransformationRotate90 ) != 0 )
                //    rotationAngle = 90;
                //else if ( ( transforms & QImageIOHandler::TransformationRotate180 ) != 0 )
                //    rotationAngle = 180;
                //else if ( ( transforms & QImageIOHandler::TransformationRotate270 ) != 0 )
                //    rotationAngle = 270;

                //if ( rotationAngle != 0 )
                //{
                //    QTransform transform;
                //    transform.rotate( rotationAngle );
                //    image = image.transformed( transform );
                //}

                auto pm = QPixmap::fromImage( image );
                if ( sz.has_value() )
                    pm = pm.scaled( sz.value(), Qt::KeepAspectRatio );
                pixmap = pm;
            }
        }
        return pixmap;
    }

    std::optional< QPixmap > pixmapForImageFile( const QString &path, const std::optional< QSize > &sz )
    {
        auto file = QFile( path );
        if ( !file.open( QFile::ReadOnly ) )
            return {};
        auto data = file.readAll();
        if ( data.isNull() )
            return {};

        return pixmapForImageData( data, sz );
    }
}
