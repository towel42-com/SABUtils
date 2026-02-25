// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __MAPTO_H
#define __MAPTO_H

#include "Towel42UtilsExport.h"

#include <QPoint>
#include <QRect>
class QWidget;

namespace NTowel42Utils
{
    // NOTE if mapToStopWidget is nullptr, the result WILL NOT include dpi issues
    TOWEL42_UTILS_EXPORT QPoint mapTo( const QWidget *mapToStopWidget, const QWidget *curr, const QPoint &pt );   // necessary to fix a "bug" in QWidget::mapTo
    TOWEL42_UTILS_EXPORT QPoint mapToWindow( const QWidget *curr, const QPoint &pt );
    TOWEL42_UTILS_EXPORT QPoint mapToGlobal( const QWidget *curr, const QPoint &pt );
    TOWEL42_UTILS_EXPORT QPoint mapToParent( const QWidget *curr, const QPoint &pt );

    // NOTE if mapToStopWidget is nullptr, the result WILL NOT include dpi issues
    TOWEL42_UTILS_EXPORT QRect mapTo( const QWidget *mapToStopWidget, const QWidget *curr, const QRect &rect );   // necessary to fix a "bug" in QWidget::mapTo
    TOWEL42_UTILS_EXPORT QRect mapToWindow( const QWidget *curr, const QRect &rect );
    TOWEL42_UTILS_EXPORT QRect mapToGlobal( const QWidget *curr, const QRect &rect );
    TOWEL42_UTILS_EXPORT QRect mapToParent( const QWidget *curr, const QRect &rect );
}
#endif
