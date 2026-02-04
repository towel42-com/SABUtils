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

#include "../ExcelFuncs.h"
#include "gtest/gtest.h"

namespace
{
#ifdef TOWEL42_QCORE_SUPPORT
    TEST( TestExcelFuncs, QtTest )
    {
        EXPECT_EQ( QDate( 2026, 1, 1 ), NTowel42Utils::closestWorkDay( QDate( 2026, 1, 1 ) ) );
        EXPECT_EQ( QDate( 2027, 12, 31 ), NTowel42Utils::closestWorkDay( QDate( 2028, 1, 1 ) ) );
        EXPECT_EQ( QDate( 2034, 1, 2 ), NTowel42Utils::closestWorkDay( QDate( 2034, 1, 1 ) ) );

        EXPECT_EQ( QDate( 2026, 1, 19 ), NTowel42Utils::nthDayOfMonth( 2026, 1, 3, 1 ) );
        EXPECT_EQ( QDate( 2026, 2, 16 ), NTowel42Utils::nthDayOfMonth( 2026, 2, 3, 1 ) );

        EXPECT_EQ( QDate( 2026, 5, 25 ), NTowel42Utils::nthDayOfMonth( 2026, 5, -1, 1 ) );
        EXPECT_EQ( QDate( 2026, 6, 19 ), NTowel42Utils::closestWorkDay( QDate( 2026, 6, 19 ) ) );
        EXPECT_EQ( QDate( 2026, 7, 3 ), NTowel42Utils::closestWorkDay( QDate( 2026, 7, 4 ) ) );
        EXPECT_EQ( QDate( 2026, 9, 7 ), NTowel42Utils::nthDayOfMonth( 2026, 9, 1, 1 ) );
        EXPECT_EQ( QDate( 2026, 10, 12 ), NTowel42Utils::nthDayOfMonth( 2026, 10, 2, 1 ) );
        EXPECT_EQ( QDate( 2026, 11, 11 ), NTowel42Utils::closestWorkDay( QDate( 2026, 11, 11 ) ) );
        auto thanksGiving = NTowel42Utils::nthDayOfMonth( 2026, 11, 4, 4 );
        EXPECT_EQ( QDate( 2026, 11, 26 ), thanksGiving );
        EXPECT_EQ( QDate( 2026, 11, 27 ), thanksGiving.addDays( 1 ) );
        EXPECT_EQ( QDate( 2026, 12, 24 ), NTowel42Utils::closestWorkDay( QDate( 2026, 12, 24 ) ) );
        EXPECT_EQ( QDate( 2026, 12, 25 ), NTowel42Utils::closestWorkDay( QDate( 2026, 12, 25 ) ) );
    }

    TEST( TestExcelFuncs, QtTestQuarterNum )
    {
        EXPECT_EQ( 1, NTowel42Utils::quarterNum( QDate( 2026, 1, 1 ) ) );
        EXPECT_EQ( 1, NTowel42Utils::quarterNum( QDate( 2028, 2, 1 ) ) );
        EXPECT_EQ( 1, NTowel42Utils::quarterNum( QDate( 2034, 3, 1 ) ) );

        EXPECT_EQ( 1, NTowel42Utils::quarterNum( QDate( 2026, 1, 10 ) ) );
        EXPECT_EQ( 1, NTowel42Utils::quarterNum( QDate( 2028, 2, 10 ) ) );
        EXPECT_EQ( 1, NTowel42Utils::quarterNum( QDate( 2034, 3, 10 ) ) );

        EXPECT_EQ( 2, NTowel42Utils::quarterNum( QDate( 2026, 4, 1 ) ) );
        EXPECT_EQ( 2, NTowel42Utils::quarterNum( QDate( 2028, 5, 1 ) ) );
        EXPECT_EQ( 2, NTowel42Utils::quarterNum( QDate( 2034, 6, 1 ) ) );

        EXPECT_EQ( 2, NTowel42Utils::quarterNum( QDate( 2026, 4, 10 ) ) );
        EXPECT_EQ( 2, NTowel42Utils::quarterNum( QDate( 2028, 5, 10 ) ) );
        EXPECT_EQ( 2, NTowel42Utils::quarterNum( QDate( 2034, 6, 10 ) ) );

        EXPECT_EQ( 3, NTowel42Utils::quarterNum( QDate( 2026, 7, 1 ) ) );
        EXPECT_EQ( 3, NTowel42Utils::quarterNum( QDate( 2028, 8, 1 ) ) );
        EXPECT_EQ( 3, NTowel42Utils::quarterNum( QDate( 2034, 9, 1 ) ) );

        EXPECT_EQ( 3, NTowel42Utils::quarterNum( QDate( 2026, 7, 10 ) ) );
        EXPECT_EQ( 3, NTowel42Utils::quarterNum( QDate( 2028, 8, 10 ) ) );
        EXPECT_EQ( 3, NTowel42Utils::quarterNum( QDate( 2034, 9, 10 ) ) );

        EXPECT_EQ( 4, NTowel42Utils::quarterNum( QDate( 2026, 10, 1 ) ) );
        EXPECT_EQ( 4, NTowel42Utils::quarterNum( QDate( 2028, 11, 1 ) ) );
        EXPECT_EQ( 4, NTowel42Utils::quarterNum( QDate( 2034, 12, 1 ) ) );

        EXPECT_EQ( 4, NTowel42Utils::quarterNum( QDate( 2026, 10, 10 ) ) );
        EXPECT_EQ( 4, NTowel42Utils::quarterNum( QDate( 2028, 11, 10 ) ) );
        EXPECT_EQ( 4, NTowel42Utils::quarterNum( QDate( 2034, 12, 10 ) ) );
    }

    TEST( TestExcelFuncs, QtTestQuarterRange )
    {
        EXPECT_EQ( std::make_pair( QDate( 2026, 1, 1 ), QDate( 2026, 3, 31 ) ), NTowel42Utils::quarterRange( QDate( 2026, 1, 1 ) ) );
        EXPECT_EQ( std::make_pair( QDate( 2026, 4, 1 ), QDate( 2026, 6, 30 ) ), NTowel42Utils::quarterRange( QDate( 2026, 5, 1 ) ) );
        EXPECT_EQ( std::make_pair( QDate( 2026, 7, 1 ), QDate( 2026, 9, 30 ) ), NTowel42Utils::quarterRange( QDate( 2026, 8, 1 ) ) );
        EXPECT_EQ( std::make_pair( QDate( 2026, 10, 1 ), QDate( 2026, 12, 31 ) ), NTowel42Utils::quarterRange( QDate( 2026, 12, 31 ) ) );
    }

#endif
#if __cplusplus >= 202002L
    std::chrono::year_month_day createYMD( int year, int month, int day )
    {
        std::chrono::year_month_day retVal( static_cast< std::chrono::year >( year ), static_cast< std::chrono::month >( month ), std::chrono::day( day ) );
        return retVal;
    }

    TEST( TestExcelFuncs, StdTest )
    {
        EXPECT_EQ( createYMD( 2026, 1, 1 ), NTowel42Utils::closestWorkDay( createYMD( 2026, 1, 1 ) ) );
        EXPECT_EQ( createYMD( 2027, 12, 31 ), NTowel42Utils::closestWorkDay( createYMD( 2028, 1, 1 ) ) );
        EXPECT_EQ( createYMD( 2034, 1, 2 ), NTowel42Utils::closestWorkDay( createYMD( 2034, 1, 1 ) ) );

        EXPECT_EQ( createYMD( 2026, 1, 19 ), NTowel42Utils::nthDayOfMonth( 2026, 1, 3, 1 ) );
        EXPECT_EQ( createYMD( 2026, 2, 16 ), NTowel42Utils::nthDayOfMonth( 2026, 2, 3, 1 ) );

        EXPECT_EQ( createYMD( 2026, 5, 25 ), NTowel42Utils::nthDayOfMonth( 2026, 5, -1, 1 ) );
        EXPECT_EQ( createYMD( 2026, 6, 19 ), NTowel42Utils::closestWorkDay( createYMD( 2026, 6, 19 ) ) );
        EXPECT_EQ( createYMD( 2026, 7, 3 ), NTowel42Utils::closestWorkDay( createYMD( 2026, 7, 4 ) ) );
        EXPECT_EQ( createYMD( 2026, 9, 7 ), NTowel42Utils::nthDayOfMonth( 2026, 9, 1, 1 ) );
        EXPECT_EQ( createYMD( 2026, 10, 12 ), NTowel42Utils::nthDayOfMonth( 2026, 10, 2, 1 ) );
        EXPECT_EQ( createYMD( 2026, 11, 11 ), NTowel42Utils::closestWorkDay( createYMD( 2026, 11, 11 ) ) );
        auto thanksGiving = NTowel42Utils::nthDayOfMonth( 2026, 11, 4, 4 );
        EXPECT_EQ( createYMD( 2026, 11, 26 ), thanksGiving );
        EXPECT_EQ( createYMD( 2026, 11, 27 ), thanksGiving.addDays( 1 ) );
        EXPECT_EQ( createYMD( 2026, 12, 24 ), NTowel42Utils::closestWorkDay( createYMD( 2026, 12, 24 ) ) );
        EXPECT_EQ( createYMD( 2026, 12, 25 ), NTowel42Utils::closestWorkDay( createYMD( 2026, 12, 25 ) ) );
    }
#endif
}
