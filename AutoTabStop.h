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

//     QWidget *determineLastFocusChild( const QWidget *target ) is copyright the qt project as it was an internal function

#ifndef __AUTOTABSTOP_H
#define __AUTOTABSTOP_H

#include "Towel42UtilsExport.h"

class QWidget;
class QTimer;
class QLabel;

#include <QObject>
#include <QWidget>   // required for the firstFocusItem property
#include <list>
#include <functional>

namespace NTowel42Utils
{
    // the easy way to use this,
    // in the constructor of the widget simply
    // new CAutoTabStop( this )
    // the class will take care of the rest
    // you can also use the statics to do it manually

    class TOWEL42_UTILS_EXPORT CAutoTabStop : public QObject
    {
        Q_OBJECT;
        Q_PROPERTY( bool focusToFirstWidgetOnShow MEMBER fSetFocusOnShow );
        Q_PROPERTY( bool targetWidgetShown MEMBER fTargetWidgetShown );
        Q_PROPERTY( QWidget *firstFocusItem MEMBER fFirstFocusItem );

    public:
        CAutoTabStop( QWidget *targetWidget );
        void setFocusToFirstWidgetOnShow( bool setFocusOnShow ) { fSetFocusOnShow = setFocusOnShow; }
        bool focusToFirstWidgetOnShow() const { return fSetFocusOnShow; }

        QWidget *firstFocusItem() const { return fFirstFocusItem; }

        virtual bool eventFilter( QObject *watched, QEvent *event ) override;

        static QWidget *autoTabStop( QWidget *rootWidget );   // returns the firstFocusItem
        static QWidget *getFirstFocusItem( QWidget *rootWidget );
        static bool validTabStop( const QWidget *widget );
        static std::list< QWidget * > getFocusChain( QWidget *start, const QWidget *rootWidget, bool forward = true, bool allWidgets = false );
        static QWidget *determineLastFocusChild( const QWidget *target );   // taken from QWidget.cpp

        static void dumpSinglePos( const QWidget *widget, const QString &prefix = {} );
        static void dumpPos( const QWidget *widget, const std::function< QLabel *( const QWidget * ) > &getLabelForBuddyFunc = {} );
    private Q_SLOTS:
        void slotTimerTimeout();

    private:

        QWidget *fTargetWidget{ nullptr };
        QWidget *fFirstFocusItem{ nullptr };
        QTimer *fShowEventTimer{ nullptr };
        bool fTargetWidgetShown{ false };
        bool fSetFocusOnShow{ true };
    };
}
#endif
