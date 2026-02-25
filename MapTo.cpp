// The MIT License( MIT )
//
// Copyright( c ) 2020-2026 Towel 42 Development, LLC and Scott Aron Bloom
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

#include "MapTo.h"

//#include <QString>
//#include <QDebug>
#include <QWidget>
//#include <QLabel>
//#include <QTimer>
//#include <QEvent>
//#include <QApplication>

//#include <functional>
//#include <unordered_set>
//#include <set>

namespace NTowel42Utils
{
    QPoint mapToWindow( const QWidget *curr, const QPoint & pt )
    {
        if ( !curr )
            return {};
        return mapTo( curr->window(), curr , pt);
    }

    QPoint mapToGlobal( const QWidget *curr, const QPoint &pt )
    {
        return mapTo( nullptr, curr, pt );
    }

    QPoint mapToParent( const QWidget *curr, const QPoint &pt )
    {
        if ( !curr )
            return {};
        return mapTo( curr->parentWidget(), curr, pt );
    }

    QPoint mapTo( const QWidget *mapToStop, const QWidget *curr, const QPoint &pt )
    {
        if ( !curr )
            return {};

        QPoint retVal = pt;
        auto mapToWindow = curr->parentWidget();
        while ( mapToWindow != mapToStop )
        {
            retVal += mapToWindow->pos();
            mapToWindow = mapToWindow->parentWidget();
            if ( !mapToWindow )
                break;
        }
        return retVal;
    }

    QRect mapToWindow( const QWidget *curr, const QRect &rect )
    {
        if ( !curr )
            return {};
        return mapTo( curr->window(), curr, rect );
    }

    QRect mapToGlobal( const QWidget *curr, const QRect &rect )
    {
        return mapTo( nullptr, curr, rect );
    }

    QRect mapToParent( const QWidget *curr, const QRect &rect )
    {
        if ( !curr )
            return {};
        return mapTo( curr->parentWidget(), curr, rect );
    }

    QRect mapTo( const QWidget *mapToStop, const QWidget *curr, const QRect &rect )
    {
        if ( !curr )
            return {};

        QPoint topLeft = rect.topLeft();
        auto mapToWindow = curr->parentWidget();
        while ( mapToWindow != mapToStop )
        {
            topLeft += mapToWindow->pos();
            mapToWindow = mapToWindow->parentWidget();
            if ( !mapToWindow )
                break;
        }
        auto retVal = rect;
        retVal.setTopLeft( topLeft );
        return retVal;
    }
}
