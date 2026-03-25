// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
// SPDX-License-Identifier : MIT License
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
//

#include "VersionInfoData.h"

#include <QString>
#include <QDateTime>
#include <cstdint>
#include <QCoreApplication>
#include <QTimeZone>

namespace NTowel42Utils
{

    QDateTime CVersionInfoData::buildDateTime( bool localTime ) const
    {
        static QDateTime sBuildDateTime;
        if ( !sBuildDateTime.isValid() )
        {
            sBuildDateTime = QDateTime::fromString( buildDateUTC() + QStringLiteral( " " ) + buildTimeUTC(), QStringLiteral( "MM/dd/yyyyZ HH:mm:ssUTCZ" ) );
            Q_ASSERT( sBuildDateTime.isValid() );
            sBuildDateTime.setTimeZone( QTimeZone::UTC );
        }

        auto retVal = sBuildDateTime;
        if ( localTime )
            retVal = retVal.toLocalTime();
        return retVal;
    }

    QString CVersionInfoData::getBuildDateText( bool localTime, bool includeTZ, bool sortableDate ) const
    {
        QString retVal = QStringLiteral( " Build Date: " );
        auto format = QLocale().dateFormat();
        if ( sortableDate )
            format = QStringLiteral( "yyyyMMdd" );

        if ( includeTZ )
            format += QStringLiteral( " " ) + QLocale().timeFormat();

        auto buildDateTime = this->buildDateTime( localTime );
        retVal += buildDateTime.toString( format );

        if ( includeTZ )
        {
            auto abbr = buildDateTime.timeZoneAbbreviation();
            if ( !abbr.isEmpty() )
            {
                auto tzString = abbr;
                if ( abbr.startsWith( QStringLiteral( "Pacific" ), Qt::CaseInsensitive ) || abbr.startsWith( QStringLiteral( "Mountain" ), Qt::CaseInsensitive ) || abbr.startsWith( QStringLiteral( "Central" ), Qt::CaseInsensitive ) || abbr.startsWith( QStringLiteral( "Eastern" ), Qt::CaseInsensitive ) )
                {
                    tzString = abbr.front().toUpper();
                    if ( buildDateTime.isDaylightTime() )
                        tzString += QStringLiteral( "DT" );
                    else
                        tzString += QStringLiteral( "ST" );
                }
                retVal += QStringLiteral( " " ) + tzString;
            }
        }
        return retVal;
    }

    QString CVersionInfoData::getVersionText( bool full ) const
    {
        auto retVal = QString::number( majorVersion() ) + QStringLiteral( "." );
        if ( forceMinorVersionTwoDigits() && ( minorVersion() < 10 ) )
            retVal += '0';
        retVal += QString::number( minorVersion() );
        if ( full )
            retVal += QStringLiteral( "." ) + patchVersion();
        if ( modified() )
            retVal += QStringLiteral( "*" );
        if ( !ahead().isEmpty() && !modified() )
            retVal += QStringLiteral( "*" );
        retVal += ahead();
        return retVal;
    }

    QString CVersionInfoData::getVersionTextEX( bool localTime, bool full, bool sortableDate ) const
    {
        auto retVal = getVersionText( full );
        if ( full )
        {
            auto dateString = getBuildDateText( localTime, full, sortableDate );
            if ( !dateString.isEmpty() )
                retVal += QStringLiteral( " - " );
            retVal += dateString;
        }

        return retVal;
    }

    QString CVersionInfoData::getWindowTitle( bool verbose /*= true*/, bool homePage /*= true */ ) const
    {
        auto retVal = appName();
        if ( verbose )
        {
            retVal += QStringLiteral( " -" );
        }
        retVal += QStringLiteral( " v" ) + getVersionTextEX( true, true, false );
        if ( verbose && homePage )
        {
            retVal += QStringLiteral( " - https://" ) + this->homePage();
        }
        return retVal;
    }

    void CVersionInfoData::setupApplication( bool useProductHomepage ) const
    {
        qApp->setApplicationName( appName() );
        qApp->setApplicationVersion( getVersionTextEX( false, true, false ) );
        qApp->setOrganizationName( vendor() );
        qApp->setOrganizationDomain( useProductHomepage ? productHomePage() : homePage() );
    }
}
