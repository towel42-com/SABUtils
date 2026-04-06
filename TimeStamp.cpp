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

#include "TimeStamp.h"
#include <chrono>
//#include <format>

#include <QLocale>
#include <QDateTime>

namespace NTowel42Utils
{
    CTimeStamp::CTimeStamp( const QString &timeFormat, const QTimeZone &tz, ETimeStampFormat format ) :
        fTimeFormat( timeFormat ),
        fTSFormat( format ),
        fTimeZone( tz )
    {
        computeLocations();
        computeInsertInfo();
    }

    CTimeStamp::CTimeStamp( const QString &timeFormat, const QString &dateFormat, const QTimeZone &tz, ETimeStampFormat format ) :
        CTimeStamp( timeFormat, tz, format )
    {
        fDateFormat = dateFormat;
    }

    CTimeStamp::CTimeStamp( const QTimeZone &tz, ETimeStampFormat format ) :
        CTimeStamp( QLocale().timeFormat( shortFormat( format ) ? QLocale::FormatType::ShortFormat : QLocale::FormatType::LongFormat ), QLocale().dateFormat( shortFormat( format ) ? QLocale::FormatType::ShortFormat : QLocale::FormatType::LongFormat ), tz, format )
    {
    }

    bool CTimeStamp::shortFormat() const
    {
        return shortFormat( fTSFormat );
    }

    bool CTimeStamp::shortFormat( ETimeStampFormat format )
    {
        return ( format & ETimeStampFormat::eShortFormat ) != 0;
    }

    QString CTimeStamp::formatString() const
    {
        if ( fDateFormat.isEmpty() )
            return fTimeFormat;

        return fDateFormat + u' ' + fTimeFormat;
    }

    void CTimeStamp::setDateFormat( const QString &dateFormat )
    {
        fDateFormat = dateFormat;
    }

    int nextPos( const QString &format, int pos )
    {
        auto currChar = format[ pos ];
        while ( pos < format.length() && ( format[ pos ] == currChar ) )
        {
            pos++;
        }
        if ( pos )
            pos--;
        return pos;
    }

    void CTimeStamp::computeLocations()
    {
        bool inSingleQuote = false;
        for ( int ii = 0; ii < fTimeFormat.length(); ++ii )
        {
            if ( fTimeFormat[ ii ] == '\'' )
            {
                inSingleQuote = !inSingleQuote;
                continue;
            }
            if ( inSingleQuote )
                continue;
            if ( fTimeFormat[ ii ] == 'z' )
            {
                auto start = ii;
                ii = nextPos( fTimeFormat, ii );
                fMsecPos = std::make_pair( start, ii );
            }
            else if ( fTimeFormat[ ii ] == 's' )
            {
                auto start = ii;
                ii = nextPos( fTimeFormat, ii );
                fSecPos = std::make_pair( start, ii );
            }
            else if ( ( fTimeFormat[ ii ].toLower() == 'm' ) )
            {
                auto start = ii;
                ii = nextPos( fTimeFormat, ii );
                fMinPos = std::make_pair( start, ii );
            }
            else if ( ( fTimeFormat[ ii ].toLower() == 'h' ) )
            {
                auto start = ii;
                ii = nextPos( fTimeFormat, ii );
                fHourPos = std::make_pair( start, ii );
            }
            else if ( fTimeFormat[ ii ] == 't' )
            {
                auto start = ii;
                ii = nextPos( fTimeFormat, ii );
                fTZPos = std::make_pair( start, ii );
            }
            else if ( fTimeFormat[ ii ].toLower() == 'a' )
            {
                if ( ( ( ii + 1 ) < ( fTimeFormat.length() - 1 ) ) && ( fTimeFormat[ ii + 1 ].toLower() == 'p' ) )
                {
                    auto start = ii;
                    ++ii;
                    fAPPos = std::make_pair( start, ii );
                }
            }
        }
    }

    void CTimeStamp::computeInsertInfo()
    {
        if ( !hasTime() )
            return;

        std::pair< int, int > lastTimeSection;
        if ( fHourPos.value().first < fMinPos.value().first )
        {
            // hour<sep>min
            lastTimeSection = fMinPos.value();
            fSep = fTimeFormat[ fHourPos.value().second + 1 ];
            fAppend = true;
        }
        else
        {
            // min<sep>hour
            lastTimeSection = fMinPos.value();
            fSep = fTimeFormat[ fMinPos.value().second + 1 ];
            fAppend = false;
        }

        if ( fAppend )
        {
            if ( fSecPos.has_value() && ( fSecPos.value().first > lastTimeSection.first ) )
                lastTimeSection = fSecPos.value();
            if ( fMsecPos.has_value() && ( fMsecPos.value().first > lastTimeSection.first ) )
                lastTimeSection = fMsecPos.value();
            if ( fAPPos.has_value() && ( fAPPos.value().first > lastTimeSection.first ) )
                lastTimeSection = fAPPos.value();

            setInsertPos( lastTimeSection.second + 1 );
        }
        else
        {
            if ( fSecPos.has_value() && ( fSecPos.value().first < lastTimeSection.first ) )
                lastTimeSection = fSecPos.value();
            if ( fMsecPos.has_value() && ( fMsecPos.value().first < lastTimeSection.first ) )
                lastTimeSection = fMsecPos.value();
            if ( fAPPos.has_value() && ( fAPPos.value().first < lastTimeSection.first ) )
                lastTimeSection = fAPPos.value();

            setInsertPos( lastTimeSection.first );
        }
    }

    bool CTimeStamp::hasTime() const
    {
        return fHourPos.has_value() && fMinPos.has_value();
    }

    std::pair< int, int > CTimeStamp::insertSubFormat( const QChar &sep, const QString &format )
    {
        auto startPos = fInsertPos;
        if ( fAppend )
        {
            fTimeFormat.insert( fInsertPos, QString( sep ) + format );
        }
        else
        {
            fTimeFormat.insert( fInsertPos, format + QString( sep ) );
        }
        auto insertPos = fInsertPos;
        if ( fAppend )
            insertPos += format.length() + 1;
        else
            insertPos -= format.length() + 1;
        setInsertPos( insertPos );
        return std::make_pair( startPos, fInsertPos );
    }

    std::pair< int, int > CTimeStamp::insertSubFormat( const QString &format )
    {
        return insertSubFormat( fSep, format );
    }

    void CTimeStamp::setShowSecs( bool showSecs )
    {
        if ( !showSecs || !hasTime() || fSecPos.has_value() )
            return;

        fSecPos = insertSubFormat( "ss" );
    }

    void CTimeStamp::setShowMSecs( bool showMSecs )
    {
        if ( showMSecs )
            fTSFormat = static_cast< ETimeStampFormat >( fTSFormat | ETimeStampFormat::eShowMSec );
        else
            fTSFormat = static_cast< ETimeStampFormat >( fTSFormat & ~ETimeStampFormat::eShowMSec );

        setShowSecs( showMSecs );

        if ( !showMSecs || !hasTime() || fMsecPos.has_value() )
            return;

        fMsecPos = insertSubFormat( QChar( '.' ), shortFormat() ? QStringLiteral( "zz" ) : QStringLiteral( "zzz" ) );
    }

    void CTimeStamp::setShowTZ( bool showTZ )
    {
        if ( showTZ )
            fTSFormat = static_cast< ETimeStampFormat >( fTSFormat | ETimeStampFormat::eShowTZ );
        else
            fTSFormat = static_cast< ETimeStampFormat >( fTSFormat & ~ETimeStampFormat::eShowTZ );

        if ( !showTZ || !hasTime() || fTZPos.has_value() )
            return;

        fTZPos = insertSubFormat( QChar( ' ' ), "'%TZ%'" );
    }

    QDateTime CTimeStamp::currentDateTime( const QTimeZone &tz )
    {
        auto retVal = QDateTime::currentDateTime();
        retVal = retVal.toTimeZone( tz );
        return retVal;
    }

    QDateTime CTimeStamp::currentDateTime()
    {
        return currentDateTime( QTimeZone( QTimeZone::LocalTime ) );
    }

    QString CTimeStamp::getTimeStamp( const QDateTime &dt, const QTimeZone &tz, bool showTZ, bool includeSeparator )
    {
        CTimeStamp ts(
            tz,   //
            static_cast< ETimeStampFormat >(
                ( includeSeparator ? ETimeStampFormat::eIncludeSeparator : 0x000 )   //
                | ( showTZ ? ETimeStampFormat::eShowTZ : 0x000 )   //
                | ETimeStampFormat::eShortFormat )   //
        );

        return ts.timeStamp( dt );
    }

    QString CTimeStamp::getTimeStamp( const QTimeZone &tz, bool showTZ, bool includeSeparator )
    {
        return getTimeStamp( currentDateTime( tz ), tz, showTZ, includeSeparator );
    }

    QString CTimeStamp::getTimeStamp( bool showTZ, bool includeSeparator )
    {
        return getTimeStamp( QTimeZone( QTimeZone::LocalTime ), showTZ, includeSeparator );
    }

    void CTimeStamp::setInsertPos( int pos )
    {
        fInsertPos = pos;
        if ( fInsertPos < 0 )
            fInsertPos = 0;
    }

    QString CTimeStamp::timeStamp( const QDateTime &dt )
    {
        if ( !dt.isValid() )
            return {};
        QLocale locale;
        QDateTime displayDT = dt;

        displayDT = dt.toTimeZone( fTimeZone );

        auto showMSec = ( fTSFormat & ETimeStampFormat::eShowMSec ) != 0;
        auto showTZ = ( fTSFormat & ETimeStampFormat::eShowTZ ) != 0;

        bool shortFormat = ( fTSFormat & ETimeStampFormat::eShortFormat ) != 0;
        auto formatType = shortFormat ? QLocale::FormatType::ShortFormat : QLocale::FormatType::LongFormat;

        QString formatStr;
        if ( !showMSec && !showTZ )
        {
            formatStr = locale.dateTimeFormat( formatType );
        }
        else
        {
            setShowMSecs( showMSec );
            setShowTZ( showTZ );

            formatStr = formatString();
        }
        auto str = dt.toString( formatStr );
        if ( fTZPos.has_value() )
        {
            auto pos = str.indexOf( "%TZ%" );
            assert( pos != -1 );
            if ( pos != -1 )
            {
                using namespace std::chrono;
                QTimeZone timeZone = fTimeZone;
                if ( !timeZone.isValid() )
                    timeZone = QTimeZone::utc();

                if ( !timeZone.isValid() )
                    timeZone = QTimeZone::systemTimeZone();

                QString tz;
                if ( timeZone.isValid() )
                {
                    tz = timeZone.abbreviation( QDateTime::currentDateTime() );
                }

                str.replace( pos, 4, tz );
            }
        }

        if ( ( fTSFormat & ETimeStampFormat::eIncludeSeparator ) != 0 )
            return QObject::tr( "%1 - " ).arg( str );

        return str;
    }
}
