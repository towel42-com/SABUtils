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
#include <QUrl>

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

    QString CVersionInfoData::getVersionText( bool verbose ) const
    {
        auto version = QStringList()   //
                       << QString::number( majorVersion() )   //
                       << QString( "%1" ).arg( minorVersion(), forceMinorVersionTwoDigits() ? 2 : 0, 10, QChar( '0' ) )   //
                       << QString( "%1" ).arg( patchVersion(), forceMinorVersionTwoDigits() ? 2 : 0, 10, QChar( '0' ) )   //
            ;
        auto retVal = version.join( "." );
        if ( verbose )
            retVal += "." + getGitVersionAndStatus();
        return retVal;
    }

    QString CVersionInfoData::getGitVersionAndStatus() const
    {
        QString retVal = gitVersion();
        if ( modified() || !ahead().isEmpty() )
            retVal += QStringLiteral( "*" );
        retVal += ahead();
        return retVal;
    }

    QString CVersionInfoData::getVersionTextEX( bool localTime, bool full, bool sortableDate ) const
    {
        auto retVal = getVersionText( true );
        if ( full )
        {
            auto dateString = getBuildDateText( localTime, full, sortableDate );
            if ( !dateString.isEmpty() )
                retVal += QStringLiteral( " - " );
            retVal += dateString;
        }

        return retVal;
    }

    QString CVersionInfoData::getWindowTitle( bool verbose /*= true*/, EHomePageType homePageType /*= true */ ) const
    {
        auto retVal = appName();
        if ( verbose )
        {
            retVal += QStringLiteral( " -" );
        }
        retVal += QStringLiteral( " v" ) + getVersionTextEX( true, true, false );
        if ( verbose )
        {
            auto homePageText = homePage( homePageType );
            if ( !homePageText.isEmpty() )
                retVal += QStringLiteral( " - " ) + homePageText;
        }
        return retVal;
    }

    void CVersionInfoData::setupApplication( EHomePageType homePage ) const
    {
        qApp->setApplicationName( appName() );
        qApp->setApplicationVersion( getVersionTextEX( false, true, false ) );
        qApp->setOrganizationName( vendor() );
        qApp->setOrganizationDomain( this->homePage( homePage ) );
    }

    QString CVersionInfoData::homePage( EHomePageType homePage ) const
    {
        if ( homePage == EHomePageType::eNone )
            return {};

        auto urlObj = QUrl( homePage == EHomePageType::eProduct ? productHomePage() : vendorHomePage() );
        if ( urlObj.scheme().isEmpty() )
            urlObj.setScheme( QStringLiteral( "https" ) );
        return urlObj.toString();
    }

    int compare( const SVersion &lhs, const SVersion &rhs )
    {
        if ( lhs.fMajor != rhs.fMajor )
            return ( lhs.fMajor < rhs.fMajor ) ? -1 : 1;
        if ( lhs.fMinor != rhs.fMinor )
            return ( lhs.fMinor < rhs.fMinor ) ? -1 : 1;
        if ( lhs.fPatch != rhs.fPatch )
            return ( lhs.fPatch < rhs.fPatch ) ? -1 : 1;
        if ( lhs.fReleaseDate != rhs.fReleaseDate )
            return ( lhs.fReleaseDate < rhs.fReleaseDate ) ? -1 : 1;
        return 0;
    }

#if __cplusplus >= 202002L
    int operator<= > ( const SVersion &lhs, const SVersion &rhs )
    {
        return compare( lhs, rhs );
    }
#endif

    bool operator>( const SVersion &lhs, const SVersion &rhs )
    {
        return compare( lhs, rhs ) > 0;
    }

    bool operator<( const SVersion &lhs, const SVersion &rhs )
    {
        return compare( lhs, rhs ) < 0;
    }

    bool operator==( const SVersion &lhs, const SVersion &rhs )
    {
        return compare( lhs, rhs ) == 0;
    }

    bool operator!=( const SVersion &lhs, const SVersion &rhs )
    {
        return !operator==( lhs, rhs );
    }

    SVersion::SVersion( const QString &tagName, const QString &createdDate )
    {
        setVersionInfo( tagName, createdDate );
    }

    SVersion::SVersion( int major, int minor, int patch ) :
        fMajor( major ),
        fMinor( minor ),
        fPatch( patch )
    {
    }

    QString SVersion::toString( bool verbose ) const
    {
        QString retVal;
        if ( verbose )
            retVal = QStringLiteral( "Version: %1.%2.%3 - Release Date: %4" );
        else
            retVal = QStringLiteral( "%1.%2.%3" );

        retVal = retVal.arg( fMajor ).arg( fMinor ).arg( fPatch );
        if ( verbose )
            retVal = retVal.arg( fReleaseDate.toString() );
        return retVal;
    }

    QString SThirdPartyData::row() const
    {
        return QString( R"(<tr>)"
                        R"(<td style="white-space=nowrap;">%1</td>)"
                        R"(<td style="white-space=nowrap;">%2</td>)"
                        R"(<td style="white-space=nowrap;"><a href="%3">%4</a></td>)"
                        R"(<td style="white-space=nowrap;"><a href="%5">%6</a></td>)"
                        R"(<td style="white-space=nowrap;"><a href="%7">%8</a></td>)"
                        R"(</tr>)" )
            .arg( fCompany, fProduct, fLicenseURL, fLicense, fSourceURL, fSource, fPatchURL, fPatchDesc );
    }

}
