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

#ifndef __EXCELFUNCS_H
#define __EXCELFUNCS_H

#include "Towel42UtilsExport.h"

#ifdef TOWEL42_QCORE_SUPPORT
    #include <QDate>
    #include <utility>
    #include <list>
#endif

#if __cplusplus >= 202002L
    #include <chrono>
#endif

namespace NTowel42Utils
{
#if __cplusplus >= 202002L
    TOWEL42_UTILS_EXPORT std::chrono::year_month_day nthDayOfMonth( int year, int month, int nthDay, std::chrono::weekday dayOfWeek );
    TOWEL42_UTILS_EXPORT std::chrono::year_month_day closestWorkDay( const std::chrono::year_month_day &date );
#endif
#ifdef TOWEL42_QCORE_SUPPORT
    using THolidayDateList = std::list< std::pair< QDate, double > >;

    TOWEL42_UTILS_EXPORT QDate nthDayOfMonth( int year, int month, int nthDay, int dayOfWeek );
    TOWEL42_UTILS_EXPORT QDate closestWorkDay( const QDate &date );
    TOWEL42_UTILS_EXPORT bool isWorkDay( const QDate &date );
    TOWEL42_UTILS_EXPORT bool isHoliday( const QDate &date, const std::list< QDate > &holidays );

    using TDateRange = std::pair< QDate, QDate >;
    TOWEL42_UTILS_EXPORT int quarterNum( const QDate &date );
    TOWEL42_UTILS_EXPORT TDateRange quarterRange( const QDate &date );
    TOWEL42_UTILS_EXPORT TDateRange quarterRange( int quarterNum, int year );

    TOWEL42_UTILS_EXPORT int numberOfBusinessDays( const TDateRange &dateRange );
    TOWEL42_UTILS_EXPORT int numberOfBusinessDays( const QDate &startDate, const QDate &endDate );

    TOWEL42_UTILS_EXPORT double numberOfHolidayDays( const TDateRange &dateRange, const THolidayDateList &holidays );
    TOWEL42_UTILS_EXPORT double numberOfHolidayDays( const QDate &startDate, const QDate &endDate, const THolidayDateList &holidays );

    TOWEL42_UTILS_EXPORT double numberOfWorkDays( const TDateRange &dateRange, const THolidayDateList &holidays );
    TOWEL42_UTILS_EXPORT double numberOfWorkDays( const QDate &startDate, const QDate &endDate, const THolidayDateList &holidays );

    TOWEL42_UTILS_EXPORT bool containsDate( const TDateRange &dateRange, const QDate &date );
    TOWEL42_UTILS_EXPORT bool containsDate( const QDate &startDate, const QDate &endDate, const QDate &date );

    TOWEL42_UTILS_EXPORT bool containsDates( const TDateRange &dateRange, const std::list< QDate > &dates );
    TOWEL42_UTILS_EXPORT bool containsDates( const QDate &startDate, const QDate &endDate, const std::list< QDate > &dates );

    TOWEL42_UTILS_EXPORT bool isChildRangeOfOrIsDateRange( const TDateRange &parentDateRange, const TDateRange &childDateRange );
    TOWEL42_UTILS_EXPORT bool isChildRangeOfOrIsDateRange( const QDate &parentStartDate, const QDate &parentEndDate, const QDate &childStartDate, const QDate &childEndDate );

#endif
}
#endif
