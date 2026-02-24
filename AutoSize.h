// The MIT License( MIT )
//
// Copyright( c ) 2026 Scott Aron Bloom
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

#include <optional>

class QTableView;
class QTreeView;
class QAbstractItemView;
class QHeaderView;
class QComboBox;
class QWidget;
class QAbstractScrollArea;

namespace NTowel42Utils
{
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QTableView *table, int minWidth = -1 );   // autoSize( table, table->horizontalHeader )
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QTreeView *treeView, int minWidth = -1 );   // autoSize( table, table->header )
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QAbstractItemView *view, int minWidth = -1 );   // autoSize( table, table->horizontalHeader )
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QAbstractItemView *view, QHeaderView *header, int minWidth = -1 );
    TOWEL42_UTILS_EXPORT std::optional< int > autoSize( QComboBox *comboBox, int minNumChars = -1 );

    TOWEL42_UTILS_EXPORT std::optional< int > resizeWidthToFitWithoutScrollbars( QAbstractScrollArea *scrollArea, std::optional< int > hintWidth );
}

#endif
