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

#include "AutoSize.h"
#include "QtUtils.h"

#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QTreeView>
#include <QScrollBar>
#include <QApplication>

#include <optional>

namespace NTowel42Utils
{
    std::optional< int > autoSize( QAbstractItemView *view, QHeaderView *header, int minWidth /*=-1*/ )
    {
        if ( !view || !view->model() || !header )
            return -1;

        auto model = view->model();
        fetchMore( model, 3 );

        auto numCols = model->columnCount();
        bool stretchLastColumn = header->stretchLastSection();
        header->setStretchLastSection( false );

        header->resizeSections( QHeaderView::ResizeToContents );
        int numVisibleColumns = 0;
        for ( int ii = 0; ii < numCols; ++ii )
        {
            if ( header->isSectionHidden( ii ) )
                continue;
            numVisibleColumns++;
            if ( numVisibleColumns > 1 )
                break;
        }
        bool dontResize = ( numVisibleColumns <= 1 ) && stretchLastColumn;

        int totalWidth = 0;
        for ( int ii = 0; ii < numCols; ++ii )
        {
            if ( header->isSectionHidden( ii ) )
                continue;

            int contentSz = view->sizeHintForColumn( ii );
            int headerSz = header->sectionSizeHint( ii );

            auto newWidth = std::max( { minWidth, contentSz, headerSz } );
            totalWidth += newWidth;

            if ( !dontResize )
                header->resizeSection( ii, newWidth );
        }
        if ( stretchLastColumn )
            header->setStretchLastSection( true );
        return totalWidth;
    }

    std::optional< int > autoSize( QTableView *table, int minWidth /*= -1*/ )
    {
        return autoSize( table, table->horizontalHeader(), minWidth );
    }

    std::optional< int > autoSize( QTreeView *tree, int minWidth /*= -1*/ )
    {
        return autoSize( tree, tree->header(), minWidth );
    }

    std::optional< int > autoSize( QAbstractItemView *view, int minWidth /*= -1*/ )
    {
        auto treeView = dynamic_cast< QTreeView * >( view );
        if ( treeView )
            return autoSize( treeView, minWidth );

        auto tableView = dynamic_cast< QTableView * >( view );
        if ( tableView )
            return autoSize( tableView, minWidth );
        return -1;
    }

    std::optional< int > autoSize( QComboBox *comboBox, int minNumChars /*= -1*/ )
    {
        if ( !comboBox )
            return {};

        comboBox->view()->setTextElideMode( Qt::ElideNone );
        auto prevPolicy = comboBox->sizeAdjustPolicy();
        if ( prevPolicy == QComboBox::AdjustToContents )
            comboBox->setSizeAdjustPolicy( QComboBox::AdjustToContentsOnFirstShow );
        comboBox->setSizeAdjustPolicy( QComboBox::AdjustToContents );
        qApp->processEvents( QEventLoop::ExcludeUserInputEvents );

        if ( ( minNumChars == -1 ) && !comboBox->placeholderText().isEmpty() && ( comboBox->count() == 0 ) )
        {
            bool hasIcon = comboBox->sizeAdjustPolicy() == QComboBox::AdjustToMinimumContentsLengthWithIcon;
            const QFontMetrics &fm = comboBox->fontMetrics();
            minNumChars = comboBox->placeholderText().length();
        }

        if ( minNumChars != -1 )
            comboBox->setMinimumContentsLength( minNumChars );

        return comboBox->width();
    }

    std::optional< int > resizeWidthToFitWithoutScrollbars( QAbstractScrollArea *scrollArea, std::optional< int > widthHint )
    {
        if ( !scrollArea )
            return {};

        if ( !scrollArea->horizontalScrollBar() )
            return {};

        QWidget *resizeWidget = scrollArea;
        while ( resizeWidget && !resizeWidget->isWindow() && resizeWidget->parentWidget() )
        {
            resizeWidget = resizeWidget->parentWidget();
        }

        if ( !resizeWidget )
            return {};

        auto currPolicy = scrollArea->horizontalScrollBarPolicy();
        scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAsNeeded );
        QSize lhsSize;
        QSize rhsSize;

        if ( widthHint.has_value() && ( resizeWidget->width() != widthHint.value() ) )
        {
            auto sz = resizeWidget->size();
            sz.setWidth( widthHint.value() );
            resizeWidget->resize( sz );
            qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
        }

        // grow/shrink it by 5% to find the initial boundaries
        auto percent = .05;
        int resizeCount = 0;
        if ( scrollArea->horizontalScrollBar()->isVisible() )
        {
            lhsSize = resizeWidget->size();
            bool first = true;
            while ( scrollArea->horizontalScrollBar()->isVisible() )
            {
                lhsSize = resizeWidget->size();
                auto newSize = lhsSize;
                auto delta = first ? 1 : newSize.width() * percent;
                newSize.setWidth( newSize.width() + delta );
                first = false;
                resizeWidget->resize( newSize );
                resizeCount++;
                qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
            }
            rhsSize = resizeWidget->size();
        }
        else
        {
            rhsSize = resizeWidget->size();
            bool first = true;
            while ( !scrollArea->horizontalScrollBar()->isVisible() )
            {
                rhsSize = resizeWidget->size();
                auto newSize = rhsSize;
                auto delta = first ? 1 : newSize.width() * percent;
                newSize.setWidth( newSize.width() - delta );
                first = false;
                resizeWidget->resize( newSize );
                resizeCount++;
                qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
            }
            lhsSize = resizeWidget->size();
        }

        qDebug() << "It took " << resizeCount << " to find starting points";
        if ( resizeWidget->size() != rhsSize )
            resizeWidget->resize( rhsSize );

        while ( lhsSize.width() < rhsSize.width() && ( ( rhsSize.width() - lhsSize.width() ) > 1 ) )
        {
            auto mid = ( rhsSize.width() - lhsSize.width() ) / 2;
            if ( mid == 0 )
                break;
            auto newSize = lhsSize;
            newSize.setWidth( newSize.width() + mid );
            resizeWidget->resize( newSize );
            resizeCount++;
            qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
            if ( scrollArea->horizontalScrollBar()->isVisible() )
                lhsSize = newSize;
            else
                rhsSize = newSize;
        }

        auto isVisible = scrollArea->horizontalScrollBar()->isVisible();
        auto delta = isVisible ? 1 : -1;
        while ( isVisible ? scrollArea->horizontalScrollBar()->isVisible() : !scrollArea->horizontalScrollBar()->isVisible() )
        {
            auto newSize = resizeWidget->size();
            newSize.setWidth( newSize.width() + delta );
            resizeWidget->resize( newSize );
            resizeCount++;
            qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
        }
        scrollArea->setHorizontalScrollBarPolicy( currPolicy );
        qDebug() << "It took " << resizeCount << " total";
        qDebug() << "Final Width: " << resizeWidget->size().width();
        return resizeWidget->size().width();
    }
}
