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
#include <unordered_set>

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

    struct QDateHash
    {
        std::size_t operator()( const QDate &date ) const { return std::hash< qint64 >()( date.toJulianDay() ); }
    };

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
        auto isSaturday = ( date.dayOfWeek() == 6 );
        auto isSunday = ( date.dayOfWeek() == 7 );
        return !isSaturday && !isSunday;
    }

    bool isHoliday( const QDate &date, const std::unordered_map< QDate, double, QDateHash > &holidays, std::optional< double > &numHours )
    {
        auto pos = holidays.find( date );
        auto found = ( pos != holidays.end() );
        if ( found )
            numHours = ( *pos ).second;
        return found;
    }

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

    TDateRange quarterRange( const QDate &date )
    {
        auto quarterNum = NTowel42Utils::quarterNum( date );
        return quarterRange( quarterNum, date.year() );
    }

    TDateRange quarterRange( int quarterNum, int year )
    {
        auto startMonth = 1 + ( ( quarterNum - 1 ) * 3 );
        auto endMonth = startMonth + 2;

        auto startDate = QDate( year, startMonth, 1 );
        auto endDate = QDate( year, endMonth, 1 ).addMonths( 1 ).addDays( -1 );

        return { startDate, endDate };
    }

    int numberOfBusinessDays( const TDateRange &dateRange )
    {
        return numberOfBusinessDays( dateRange.first, dateRange.second );
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

    double numberOfHolidayDays( const TDateRange &dateRange, const THolidayDateList &holidaysList )
    {
        return numberOfHolidayDays( dateRange.first, dateRange.second, holidaysList );
    }

    double numberOfHolidayDays( const QDate &startDate, const QDate &endDate, const THolidayDateList &holidaysList )
    {
        auto holidays = std::unordered_map< QDate, double, QDateHash >( { holidaysList.begin(), holidaysList.end() } );
        double retVal = 0;
        for ( auto ii = startDate; ii <= endDate; ii = ii.addDays( 1 ) )
        {
            std::optional< double > numHours;
            if ( isHoliday( ii, holidays, numHours ) )
            {
                if ( numHours.has_value() )
                    retVal += ( numHours.value() / 8 );
                else
                    retVal += 1;
            }
        }
        return retVal;
    }

    double numberOfWorkDays( const TDateRange &dateRange, const THolidayDateList &holidaysList )
    {
        return numberOfWorkDays( dateRange.first, dateRange.second, holidaysList );
    }

    double numberOfWorkDays( const QDate &startDate, const QDate &endDate, const THolidayDateList &holidaysList )
    {
        auto holidays = std::unordered_map< QDate, double, QDateHash >( { holidaysList.begin(), holidaysList.end() } );
        double retVal = 0;
        for ( auto ii = startDate; ii <= endDate; ii = ii.addDays( 1 ) )
        {
            std::optional< double > numHours;
            if ( isWorkDay( ii ) )
            {
                retVal += 1;
                if ( isHoliday( ii, holidays, numHours ) )
                {
                    if ( numHours.has_value() )
                        retVal -= ( numHours.value() / 8 );
                }
            }
        }
        return retVal;
    }

    bool containsDate( const TDateRange &dateRange, const QDate &date )
    {
        return containsDate( dateRange.first, dateRange.second, date );
    }

    bool containsDate( const QDate &startDate, const QDate &endDate, const QDate &date )
    {
        return ( ( date >= startDate ) && ( date <= endDate ) );
    }

    bool containsDates( const TDateRange &dateRange, const std::list< QDate > &dates )
    {
        return containsDates( dateRange.first, dateRange.second, dates );
    }

    bool containsDates( const QDate &startDate, const QDate &endDate, const std::list< QDate > &dates )
    {
        for ( auto &&ii : dates )
        {
            if ( !containsDate( startDate, endDate, ii ) )
                return false;
        }
        return true;
    }

    bool isChildRangeOfOrIsDateRange( const TDateRange &parentDateRange, const TDateRange &childDateRange )
    {
        return isChildRangeOfOrIsDateRange( parentDateRange.first, parentDateRange.second, childDateRange.first, childDateRange.second );
    }

    bool isChildRangeOfOrIsDateRange( const QDate &parentStartDate, const QDate &parentEndDate, const QDate &childStartDate, const QDate &childEndDate )
    {
        if ( ( parentStartDate == childStartDate ) && ( parentEndDate == childEndDate ) )
            return true;

        if ( parentStartDate > childStartDate )
            return false;
        if ( parentEndDate < childEndDate )
            return false;
        return true;
    }

#endif
}
