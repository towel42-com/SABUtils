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

#ifndef __UIUTILS_H
#define __UIUTILS_H

#include "Towel42UtilsExport.h"
#include <QModelIndex>
#include <vector>
#include <QPixmap>
#include <QImage>
#include <optional>

class QString;
class QPoint;
class QFont;
class QUrl;
class QLineEdit;
class QComboBox;
class QDateEdit;
class QLabel;
class QDate;
class QButtonGroup;
class QAbstractButton;
class QTabWidget;
class QCheckBox;
class QTextEdit;
class QWidget;
class QSpinBox;
class QAbstractItemView;
class QGroupBox;
#include <optional>
#include <functional>

namespace NTowel42Utils
{
    TOWEL42_UTILS_EXPORT bool launchIfURLClicked( const QString &title, const QPoint &pt, const QFont &font );
    TOWEL42_UTILS_EXPORT std::optional< QString > openUrl( const QUrl &url );

    TOWEL42_UTILS_EXPORT bool isValid( QLineEdit *edit, QLabel *label, std::function< bool( const QString &text ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool isValid( QComboBox *cb, QLabel *label, std::function< bool( const QString &text ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool isValid( QDateEdit *de, QLabel *label, std::function< bool( const QDate &date ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool isValid( const QString &text, QLabel *label, std::function< bool( const QString &text ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool isValid( QButtonGroup *bg, QLabel *label, std::function< bool( const QAbstractButton *btn ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool isValid( QCheckBox *cb, QLineEdit *desc, std::function< bool( bool checked, const QString &text ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool isValid( QTextEdit *te, QLabel *label, std::function< bool( const QString &text ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool isValid( QSpinBox *te, QLabel *label, std::function< bool( int value ) > isValidFunc = {} );
    TOWEL42_UTILS_EXPORT bool setIsOK( bool aOK, QWidget *widget, const QString &widgetName );
    TOWEL42_UTILS_EXPORT bool setIsOK( bool aOK, QLabel *label );
    TOWEL42_UTILS_EXPORT bool setIsOK( bool aOK, QCheckBox *cb );
    TOWEL42_UTILS_EXPORT bool setIsOK( bool aOK, QTabWidget *tw, int index );
    TOWEL42_UTILS_EXPORT bool setIsOK( bool aOK, QGroupBox *gb );

    TOWEL42_UTILS_EXPORT QModelIndex selectFirstVisibleItem( QAbstractItemView *view, bool setFocus );

    TOWEL42_UTILS_EXPORT void selectItemInComboBox( QComboBox *cb, const QVariant &value, bool *other = nullptr );

    TOWEL42_UTILS_EXPORT void loadEnumValues( QComboBox *cb, const std::vector< std::pair< int, QString > > &enumValues, std::optional< int > blankValue = {} );

    TOWEL42_UTILS_EXPORT std::optional< QPixmap > pixmapForImageData( const QByteArray &data, const std::optional< QSize > &sz = {} );
    TOWEL42_UTILS_EXPORT std::optional< QPixmap > pixmapForImageFile( const QString &path, const std::optional< QSize > &sz = {} );
    TOWEL42_UTILS_EXPORT std::optional< QImage > imageForImageData( const QByteArray &data, const std::optional< QSize > &sz = {} );
    TOWEL42_UTILS_EXPORT std::optional< QImage > imageForImageFile( const QString &path, const std::optional< QSize > &sz = {} );

    TOWEL42_UTILS_EXPORT std::optional< QImage > findOptimalQualityForSize( const QImage &image, int64_t sz, QByteArray &data );
}
#endif
