#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H
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

#include "Towel42UtilsExport.h"

#include <QString>
#include <QTimeZone>

class QDateTime;

namespace NTowel42Utils
{
    class TOWEL42_UTILS_EXPORT CTimeStamp
    {
    public:
        enum ETimeStampFormat
        {
            eNone = 0x000,
            eIncludeSeparator = 0x008,
            eShortFormat = 0x010,   // shortformat and long format are mutually exclusive, if both set, short is picked
            eLongFormat = 0x020,
            eShowMSec = 0x040,   // if the locale format already shows tz or msec, these options are redundent
            eShowTZ = 0x080   // if seconds are not shown, they are added
        };

        CTimeStamp( const QTimeZone &tz = {}, ETimeStampFormat format = ETimeStampFormat::eNone );   // uses locales date and time format
        CTimeStamp( const QString &timeFormat, const QTimeZone &tz = {}, ETimeStampFormat format = ETimeStampFormat::eNone );   // no date format
        CTimeStamp( const QString &timeFormat, const QString &dateFormat, const QTimeZone &tz = {}, ETimeStampFormat format = ETimeStampFormat::eNone );

        QString timeStamp( const QDateTime &dt );
        QString formatString() const;

        void setDateFormat( const QString &dateFormat );
        void setShowSecs( bool showSecs );
        void setShowMSecs( bool showMSecs );
        void setShowTZ( bool showTZ );

        static QDateTime currentDateTime( const QTimeZone &tz );
        static QDateTime currentDateTime();   // for local tz

        static QString getTimeStamp( const QDateTime &dt, const QTimeZone &tz, bool showTZ, bool includeSeparator );
        static QString getTimeStamp( const QTimeZone &tz, bool showTZ, bool includeSeparator );   // uses current date time
        static QString getTimeStamp( bool showTZ, bool includeSeparator );   // uses current date time and local time zone

    private:
        bool shortFormat() const;
        static bool shortFormat( ETimeStampFormat format );

        std::pair< int, int > findTZLocation( const QString &str ) const;
        void computeLocations();
        void computeInsertInfo();

        bool hasTime() const;

        std::pair< int, int > insertSubFormat( const QString &format );
        std::pair< int, int > insertSubFormat( const QChar &sep, const QString &format );
        void setInsertPos( int pos );

        QString fTimeFormat;
        QString fDateFormat;
        std::optional< std::pair< int, int > > fHourPos;
        std::optional< std::pair< int, int > > fMinPos;
        std::optional< std::pair< int, int > > fSecPos;
        std::optional< std::pair< int, int > > fMsecPos;
        std::optional< std::pair< int, int > > fAPPos;
        std::optional< std::pair< int, int > > fTZPos;

        ETimeStampFormat fTSFormat{ ETimeStampFormat::eNone };
        QChar fSep{ ':' };
        bool fAppend{ true };
        int fInsertPos{ -1 };
        QTimeZone fTimeZone;
    };
}
#endif