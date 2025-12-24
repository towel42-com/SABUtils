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

#ifdef Q_OS_WINDOWS
    #include <qt_windows.h>
    #include <shellapi.h>
#endif

namespace NTowel42Utils
{
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

}
