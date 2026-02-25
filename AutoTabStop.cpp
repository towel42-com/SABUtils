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

#include "AutoTabStop.h"
#include "MapTo.h"

#include <QString>
#include <QDebug>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QEvent>
#include <QApplication>

#include <functional>
#include <unordered_set>
#include <set>

namespace NTowel42Utils
{
    CAutoTabStop::CAutoTabStop( QWidget *targetWidget ) :
        QObject( targetWidget ),
        fTargetWidget( targetWidget )
    {
        fShowEventTimer = new QTimer( this );
        fShowEventTimer->setInterval( 50 );
        fShowEventTimer->setSingleShot( true );
        connect( fShowEventTimer, &QTimer::timeout, this, &CAutoTabStop::slotTimerTimeout );
        fTargetWidget->installEventFilter( this );
    }

    void CAutoTabStop::slotTimerTimeout()
    {
        fFirstFocusItem = autoTabStop( fTargetWidget );
        if ( fFirstFocusItem )
            fFirstFocusItem->setFocus( Qt::FocusReason::TabFocusReason );
        fTargetWidgetShown = true;
        fTargetWidget->removeEventFilter( this );
    }

    bool CAutoTabStop::eventFilter( QObject *watched, QEvent *event )
    {
        if ( !fTargetWidgetShown && ( watched == fTargetWidget ) && ( event->type() == QEvent::Show ) )
        {
            fShowEventTimer->stop();
            fShowEventTimer->start();
        }
        return QObject::eventFilter( watched, event );
    }

    void CAutoTabStop::dumpSinglePos( const QWidget *targetWidget, const QWidget *widget, const QString &prefix )
    {
        qDebug().noquote() << prefix << widget << NTowel42Utils::mapTo( nullptr, widget, widget->pos() ) << targetWidget << NTowel42Utils::mapTo( targetWidget, widget, widget->pos() );
    }

    void CAutoTabStop::dumpPos( const QWidget *targetWidget, const QWidget *widget, const std::function< QLabel *( const QWidget * ) > &getLabelForBuddyFunc /*= {}*/ )
    {
        targetWidget = widget;
        while ( !targetWidget->isWindow() )
            targetWidget = targetWidget->parentWidget();

        dumpSinglePos( targetWidget, widget );
        auto focusWidget = CAutoTabStop::determineLastFocusChild( widget );
        if ( focusWidget != widget )
        {
            dumpSinglePos( targetWidget, focusWidget, "    " );
        }
        if ( getLabelForBuddyFunc )
        {
            auto label = getLabelForBuddyFunc( widget );
            if ( label )
            {
                dumpSinglePos( targetWidget, label, "    " );
            }
        }
    }

    bool CAutoTabStop::validTabStop( const QWidget *targetWidget, const QWidget *widget )
    {
        if ( !targetWidget || !widget )
            return false;

        return ( widget->focusPolicy() != Qt::FocusPolicy::NoFocus )   //
               && targetWidget->isAncestorOf( widget )   //
               && widget->isVisible()   //
               && !widget->pos().isNull()   //
               && !NTowel42Utils::mapTo( nullptr, widget, widget->pos() ).isNull()   //
            ;
    }

    std::list< QWidget * > CAutoTabStop::getFocusChain( QWidget *start, const QWidget *parentWidget, bool bForward, bool allWidgets /*= false */ )
    {
        if ( !start )
        {
            start = QApplication::focusWidget();
            if ( !parentWidget->isAncestorOf( start ) )
                start = nullptr;
        }

        if ( !start )
        {
            auto children = parentWidget->findChildren< QWidget * >();
            for ( auto &&child : children )
            {
                if ( child->focusPolicy() != Qt::FocusPolicy::NoFocus )
                {
                    start = child;
                    break;
                }
            }
        }
        if ( !start )
            return {};

        std::list< QWidget * > ret;
        std::unordered_set< QWidget * > beenThere;
        auto currWidget = start;
        // detect infinite loop
        do
        {
            if ( beenThere.find( currWidget ) != beenThere.end() )
                return ret;

            if ( ( allWidgets && parentWidget->isAncestorOf( currWidget ) ) || validTabStop( parentWidget, currWidget ) )
                ret.push_back( currWidget );
            currWidget = bForward ? currWidget->nextInFocusChain() : currWidget->previousInFocusChain();
        }
        while ( currWidget != start );
        return ret;
    }

    // from QWidget.cpp
    QWidget *CAutoTabStop::determineLastFocusChild( const QWidget *target )
    {
        // Since we need to repeat the same logic for both 'first' and 'second', we add a function that
        // determines the last focus child for a widget, taking proxies and compound widgets into account.
        // If the target is not a compound widget (it doesn't have a focus proxy that points to a child),
        // 'lastFocusChild' will be set to the target itself.
        QWidget *lastFocusChild = const_cast< QWidget * >( target );

        QWidget *focusProxy = target->focusProxy();
        if ( !focusProxy )
        {
            // QTBUG-81097: Another case is possible here. We can have a child
            // widget, that sets its focusProxy() to the parent (target).
            // An example of such widget is a QLineEdit, nested into
            // a QAbstractSpinBox. In this case such widget should be considered
            // the last focus child.
            for ( auto *object : target->children() )
            {
                QWidget *w = qobject_cast< QWidget * >( object );
                if ( w && w->focusProxy() == target )
                {
                    lastFocusChild = w;
                    break;
                }
            }
        }
        else if ( target->isAncestorOf( focusProxy ) )
        {
            lastFocusChild = focusProxy;
            for ( QWidget *focusNext = lastFocusChild->nextInFocusChain(); focusNext != focusProxy && target->isAncestorOf( focusNext ) && focusNext->window() == focusProxy->window(); focusNext = focusNext->nextInFocusChain() )
            {
                if ( focusNext->focusPolicy() != Qt::NoFocus )
                    lastFocusChild = focusNext;
            }
        }
        return lastFocusChild;
    }

    struct SWidgetLocationCompare
    {
        bool operator()( QWidget *lhs, QWidget *rhs ) const
        {
            Q_ASSERT( lhs && rhs );

            auto lhsFocusChild = CAutoTabStop::determineLastFocusChild( lhs );
            auto rhsFocusChild = CAutoTabStop::determineLastFocusChild( rhs );

            auto lhsFocusChildPos = NTowel42Utils::mapTo( nullptr, lhsFocusChild, lhsFocusChild->pos() );
            auto rhsFocusChildPos = NTowel42Utils::mapTo( nullptr, rhsFocusChild, rhsFocusChild->pos() );

            auto lhsYPos = lhsFocusChildPos.y();
            auto rhsYPos = rhsFocusChildPos.y();

            auto yDiff = std::abs( lhsYPos - rhsYPos );
            if ( yDiff <= 2 )
                yDiff = 0;

            if ( yDiff != 0 )
                return lhsYPos < rhsYPos;

            auto lhsXPos = lhsFocusChildPos.x();
            auto rhsXPos = rhsFocusChildPos.x();
            return lhsXPos < rhsXPos;
        }
    };

    using TWidgetLocationMap = std::set< QWidget *, SWidgetLocationCompare >;
    TWidgetLocationMap determineWidgetLocationMap( QWidget *targetWidget )
    {
        if ( !targetWidget )
            return {};

        auto children = targetWidget->findChildren< QWidget * >();
        std::unordered_set< QWidget * > beenThere;
        for ( auto ii = children.begin(); ii != children.end(); )
        {
            auto beenHandled = beenThere.find( *ii ) != beenThere.end();
            auto realFocusWidget = CAutoTabStop::determineLastFocusChild( *ii );
            beenHandled = beenHandled || ( beenThere.find( realFocusWidget ) != beenThere.end() );
            if ( beenHandled )
            {
                ii = children.erase( ii );
                continue;
            }

            if ( CAutoTabStop::validTabStop( targetWidget, realFocusWidget ) )
            {
                beenThere.insert( *ii );
                beenThere.insert( realFocusWidget );
                ii++;
            }
            else
                ii = children.erase( ii );
        }

        //qDebug() << "================================";
        //for ( auto &&ii : children )
        //{
        //    CAutoTabStop::dumpPos( targetWidget, ii );
        //}

        TWidgetLocationMap widgetMap;
        for ( auto &&child : children )
        {
            auto aOK = widgetMap.insert( child ).second;
            Q_ASSERT( aOK );
        }
        //qDebug() << "================================";
        //for ( auto &&ii : widgetMap )
        //{
        //    CAutoTabStop::dumpPos( targetWidget, ii );
        //}

        //qDebug() << "================================";
        return widgetMap;
    }

    QWidget *CAutoTabStop::autoTabStop( QWidget *parentWidget )
    {
        auto widgetMap = determineWidgetLocationMap( parentWidget );
        QWidget *prev = nullptr;
        for ( const auto &widget : widgetMap )
        {
            if ( !prev )
            {
                prev = widget;
            }
            else
            {
                QWidget::setTabOrder( prev, widget );
                prev = widget;
            }
        }

        if ( widgetMap.empty() )
            return nullptr;
        return *widgetMap.begin();
    }

    QWidget *CAutoTabStop::getFirstFocusItem( QWidget *parentWidget )
    {
        auto widgetMap = determineWidgetLocationMap( parentWidget );
        if ( widgetMap.empty() )
            return nullptr;
        return *widgetMap.begin();
    };
}
