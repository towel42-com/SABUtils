// The MIT License( MIT )
//
// Copyright( c ) 2020-2025 Scott Aron Bloom
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
            widget->setStyleSheet( QString( "%1 { color : red; }" ).arg( widgetName ) );
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

    void dumpSinglePos( const QWidget *parentWidget, const QWidget *widget, const QString &prefix = {} )
    {
        qDebug().noquote() << prefix << widget << widget->mapToGlobal( widget->pos() ) << parentWidget << widget->mapTo( parentWidget, widget->pos() );
    }

    void dumpPos( const QWidget *parentWidget, const QWidget *widget, const std::function< QLabel *( const QWidget * ) > &getLabelForBuddyFunc = {} )
    {
        parentWidget = widget;
        while ( !parentWidget->isWindow() )
            parentWidget = parentWidget->parentWidget();

        dumpSinglePos( parentWidget, widget );
        auto focusWidget = determineLastFocusChild( widget );
        if ( focusWidget != widget )
        {
            dumpSinglePos( parentWidget, focusWidget, "    " );
        }
        if ( getLabelForBuddyFunc )
        {
            auto label = getLabelForBuddyFunc( widget );
            if ( label )
            {
                dumpSinglePos( parentWidget, label, "    " );
            }
        }
    }
    bool validTabStop( const QWidget *parentWidget, const QWidget *widget )
    {
        if ( !parentWidget || !widget )
            return false;

        return ( widget->focusPolicy() != Qt::FocusPolicy::NoFocus )   //
               && parentWidget->isAncestorOf( widget )   //
               //&& widget->isVisible()   //
               && !widget->pos().isNull()   //
               && !widget->mapToGlobal( widget->pos() ).isNull()   //
            ;
        //&& !widget->objectName().startsWith( "qt_" );
    }

    std::list< QWidget * > getFocusChain( QWidget *start, const QWidget *parentWidget, bool bForward, bool allWidgets /*= false */ )
    {
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

    QWidget *determineLastFocusChild( const QWidget *target )
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

    using TBuddyMap = std::unordered_map< const QWidget *, QLabel * >;

    struct SWidgetLocationCompare
    {
        TBuddyMap fBuddyMap;

        SWidgetLocationCompare( const TBuddyMap &buddyMap = {} ) :
            fBuddyMap( buddyMap )
        {
        }

        QLabel *labelForBuddy( const QWidget *buddy ) const
        {
            auto pos = fBuddyMap.find( buddy );
            if ( pos == fBuddyMap.end() )
                return nullptr;
            return ( *pos ).second;
        };

        bool operator()( QWidget *lhs, QWidget *rhs ) const
        {
            Q_ASSERT( lhs && rhs );

            auto lhsFocusChild = determineLastFocusChild( lhs );
            auto rhsFocusChild = determineLastFocusChild( rhs );

            auto lhsLabel = labelForBuddy( lhs );
            if ( !lhsLabel && ( lhs != lhsFocusChild ) )
                lhsLabel = labelForBuddy( lhsFocusChild );

            auto rhsLabel = labelForBuddy( rhs );
            if ( !rhsLabel && ( lhs != rhsFocusChild ) )
                rhsLabel = labelForBuddy( rhsFocusChild );

            int lhsYPos = 0;
            if ( lhsLabel )
                lhsYPos = lhsLabel->mapToGlobal( lhsLabel->pos() ).y();
            else
                lhsYPos = lhsFocusChild->mapToGlobal( lhsFocusChild->pos() ).y();

            int rhsYPos = 0;
            if ( rhsLabel )
                rhsYPos = rhsLabel->mapToGlobal( rhsLabel->pos() ).y();
            else
                rhsYPos = rhsFocusChild->mapToGlobal( rhsFocusChild->pos() ).y();

            auto yDiff = std::abs( lhsYPos - rhsYPos );
            if ( yDiff <= 2 )
                yDiff = 0;

            if ( yDiff != 0 )
                return lhsYPos < rhsYPos;

            auto lhsPos = lhsFocusChild->mapToGlobal( lhsFocusChild->pos() );
            auto rhsPos = rhsFocusChild->mapToGlobal( rhsFocusChild->pos() );
            return lhsPos.x() < rhsPos.x();
        }
    };

    using TWidgetLocationMap = std::set< QWidget *, SWidgetLocationCompare >;
    TWidgetLocationMap determineWidgetLocationMap( QWidget *parentWidget )
    {
        if ( !parentWidget )
            return {};

        std::unordered_map< const QWidget *, QLabel * > buddyMap;

        auto children = parentWidget->findChildren< QWidget * >();
        for ( auto &&ii : children )
        {
            auto label = dynamic_cast< QLabel * >( ii );
            if ( !label )
                continue;

            auto buddy = label->buddy();
            if ( !buddy )
                continue;

            buddyMap[ buddy ] = label;
            auto buddyChildren = buddy->findChildren< QWidget * >();
            for ( auto &&ii : buddyChildren )
                buddyMap[ ii ] = label;
        }

        std::unordered_set< QWidget * > beenThere;
        for ( auto ii = children.begin(); ii != children.end(); )
        {
            auto beenHandled = beenThere.find( *ii ) != beenThere.end();
            auto realFocusWidget = determineLastFocusChild( *ii );
            beenHandled = beenHandled || ( beenThere.find( realFocusWidget ) != beenThere.end() );
            if ( beenHandled )
            {
                ii = children.erase( ii );
                continue;
            }

            if ( validTabStop( parentWidget, realFocusWidget ) )
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
        //    dumpPos( parentWidget, ii, labelForBuddy );
        //}

        TWidgetLocationMap widgetMap( buddyMap );
        for ( auto &&child : children )
        {
            widgetMap.insert( child );
        }
        //qDebug() << "================================";
        //for ( auto &&ii : widgetMap )
        //{
        //    dumpPos( parentWidget, ii, labelForBuddy );
        //}

        //qDebug() << "================================";
        return widgetMap;
    }

    QWidget *autoTabStop( QWidget *parentWidget )
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

    QWidget *getFirstFocusItem( QWidget *parentWidget )
    {
        auto widgetMap = determineWidgetLocationMap( parentWidget );
        if ( widgetMap.empty() )
            return nullptr;
        return *widgetMap.begin();
    };

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

}
