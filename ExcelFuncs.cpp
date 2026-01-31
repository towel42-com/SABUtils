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

#include "ExcelFuncs.h"
#include <QDate>
#include < unordered_set >

#if __cplusplus >= 202002L
    #include <chrono>
#endif

#ifdef TOWEL42_QCORE_SUPPORT
    #include <QDate>
    #include <QDebug>
#endif

namespace NTowel42Utils
{
#if __cplusplus >= 202002L
    std::chrono::year_month_day create( int year, int month, int day )
    {
        std::chrono::year_month_day retVal( static_cast< std::chrono::year >( year ), static_cast< std::chrono::month >( month ), std::chrono::day( day ) );
        return retVal;
    }

    std::chrono::year_month_day nthDayOfMonth( int year, int month, int nthDay, std::chrono::weekday dayOfWeek )
    {
        auto dt = std::chrono::sys_days( create( year, month, 1 ) );
        int offset = 1;
        if ( nthDay < 0 )
        {
            const std::chrono::month_day_last mdl{ std::chrono::month( month ) / std::chrono::last };
            const std::chrono::year_month_day_last ymdl{ std::chrono::year( year ), mdl };
            dt = ymdl;

            offset = -1;
            nthDay *= -1;
        }
        int currDayOfMonth = 0;
        if ( std::chrono::weekday( dt ) == dayOfWeek )
            currDayOfMonth++;
        while ( std::chrono::year_month_day( dt ).month() == std::chrono::month( month ) )
        {
            dt = dt + std::chrono::days( 1 );
            if ( std::chrono::weekday( dt ) == dayOfWeek )
                currDayOfMonth++;
            if ( currDayOfMonth == nthDay )
                break;
        }
        if ( currDayOfMonth > nthDay )
            return {};

        return dt;
    }

    std::chrono::year_month_day closestWorkDay( const std::chrono::year_month_day &date )
    {
        auto retVal = date;
        auto dayOfWeek = std::chrono::weekday( date );
        if ( dayOfWeek == std::chrono::Saturday )
            retVal = std::chrono::sys_days( retVal ) - std::chrono::days( 1 );
        else if ( dayOfWeek == std::chrono::Sunday )
            retVal = std::chrono::sys_days( retVal ) + std::chrono::days( 1 );
        return retVal;
    }

#endif
#ifdef TOWEL42_QCORE_SUPPORT
    QDate nthDayOfMonth( int year, int month, int nthDay, int dayOfWeek )
    {
        QDate dt( year, month, 1 );
        int offset = 1;
        if ( nthDay < 0 )
        {
            dt = QDate( year, month, 1 ).addMonths( 1 ).addDays( -1 );
            offset = -1;
            nthDay *= -1;
        }
        int currDayOfMonth = 0;
        if ( dt.dayOfWeek() == dayOfWeek )
            currDayOfMonth++;
        while ( dt.month() == month )
        {
            dt = dt.addDays( offset );
            if ( dt.dayOfWeek() == dayOfWeek )
                currDayOfMonth++;
            if ( currDayOfMonth == nthDay )
                break;
        }
        if ( currDayOfMonth > nthDay )
            return {};

        return dt;
    }

    QDate closestWorkDay( const QDate &date )
    {
        Q_ASSERT( date.isValid() );
        QDate retVal = date;
        auto dayOfWeek = date.dayOfWeek();
        if ( dayOfWeek == 6 )
            retVal = retVal.addDays( -1 );
        else if ( dayOfWeek == 7 )
            retVal = retVal.addDays( 1 );
        return retVal;
    }

    bool isWorkDay( const QDate &date )
    {
        return ( date.dayOfWeek() != 6 ) && ( date.dayOfWeek() != 7 );
    }

    int numberOfBusinessDays( const QDate &startDate, const QDate &endDate )
    {
        int retVal = 0;
        for ( auto ii = startDate; ii <= endDate; ii = ii.addDays( 1 ) )
        {
            if ( isWorkDay( ii ) )
                retVal++;
        }
        return retVal;
    }
    struct QDateHash
    {
        std::size_t operator()( const QDate &date ) const { return std::hash< qint64 >()( date.toJulianDay() ); }
    };

    bool isHoliday( const QDate &date, const std::unordered_set< QDate, QDateHash > &holidays )
    {
        auto pos = holidays.find( date );
        return ( holidays.find( date ) != holidays.end() );
    }

    bool isHoliday( const QDate &date, const std::list< QDate > &holidays )
    {
        return isHoliday( date, std::unordered_set< QDate, QDateHash >( { holidays.begin(), holidays.end() } ) );
    }

    int quarterNum( const QDate &date )
    {
        auto monthNum = date.month();
        if ( monthNum >= 1 && monthNum <= 3 )
            return 1;
        if ( monthNum >= 4 && monthNum <= 6 )
            return 2;
        if ( monthNum >= 7 && monthNum <= 9 )
            return 3;
        //if ( monthNum >= 10 && monthNum <= 12 )
        return 4;
    }

    int numberOfHolidayDays( const QDate &startDate, const QDate &endDate, const std::list< QDate > &holidaysList )
    {
        auto holidays = std::unordered_set< QDate, QDateHash >( { holidaysList.begin(), holidaysList.end() } );
        int retVal = 0;
        for ( auto ii = startDate; ii <= endDate; ii = ii.addDays( 1 ) )
        {
            if ( isHoliday( ii, holidays ) )
                retVal++;
        }
        return retVal;
    }

    int numberOfWorkDays( const QDate &startDate, const QDate &endDate, const std::list< QDate > &holidaysList )
    {
        auto holidays = std::unordered_set< QDate, QDateHash >( { holidaysList.begin(), holidaysList.end() } );
        int retVal = 0;
        for ( auto ii = startDate; ii <= endDate; ii = ii.addDays( 1 ) )
        {
            if ( isWorkDay( ii ) && !isHoliday( ii, holidays ) )
                retVal++;
        }
        return retVal;
    }

#endif
}
