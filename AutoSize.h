// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __AUTOSIZE_H
#define __AUTOSIZE_H

#include "Towel42UtilsExport.h"
#include <QWidget>
#include <optional>
#include <functional>
#include <QString>

class QTableView;
class QTreeView;
class QAbstractItemView;
class QHeaderView;
class QComboBox;
class QWidget;
class QAbstractScrollArea;
class QString;
class QLabel;
class QLineEdit;

namespace NTowel42Utils
{
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QTableView *table, int minWidth = -1 );   // autoSize( table, table->horizontalHeader )
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QTreeView *treeView, int minWidth = -1 );   // autoSize( table, table->header )
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QAbstractItemView *view, int minWidth = -1 );   // autoSize( table, table->horizontalHeader )
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QAbstractItemView *view, QHeaderView *header, int minWidth = -1 );
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QComboBox *comboBox, int minNumChars = -1 );

    TOWEL42_UTILS_EXPORT std::optional< int > resizeWidthToFitWithoutScrollbars( QAbstractScrollArea *scrollArea, bool shrinkIfNecessary, const std::function< std::optional< int >() > &widthHintFunc );
    TOWEL42_UTILS_EXPORT std::optional< int > resizeWidthToFitWithoutScrollbars( QAbstractScrollArea *scrollArea, bool shrinkIfNecessary, const std::optional< int > & hintWidth );

    // sets the minimum size so the text is visible
    TOWEL42_UTILS_EXPORT void setMinimumWidth( QWidget *widget, const QString &textToSizeTo, const std::optional< QString > &extraText );
    TOWEL42_UTILS_EXPORT void setMinimumWidth( QWidget *widget, const std::optional< QString > &extraText = {} );
    TOWEL42_UTILS_EXPORT void setMinimumWidth( QLabel *label, const std::optional< QString > &extraText = {} );
    TOWEL42_UTILS_EXPORT void setMinimumWidth( QLineEdit *lineEdit, const std::optional< QString > &extraText = {} );

    template< typename T >
    void setMinimumWidth( QWidget *widget, const std::optional< QString > &extraText = {} )   // finds all children of type className and sets their minimum size
    {
        if ( !widget )
            return;
        auto children = widget->findChildren< T >();
        for ( auto &&ii : children )
        {
            if ( qobject_cast< QLabel * >( ii ) )
                setMinimumWidth( (QLabel *)ii, extraText );
            else if ( qobject_cast< QLineEdit * >( ii ) )
                setMinimumWidth( (QLineEdit *)ii, extraText );
            else
                setMinimumWidth( ii, extraText );
        }
    }
}

#endif
