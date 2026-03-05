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

#include "../CantorHash.h"
#include "gtest/gtest.h"

namespace NTowel42Utils
{
    template< typename T1, typename T2 >
    inline std::enable_if_t< !std::is_integral_v< T1 > || !std::is_integral_v< T2 >, size_t > cantorHash( const T1 & /*lhs*/, const T2 & /*rhs*/ )
    {
        return -1;
    }

    template< typename T1, typename T2 >
    inline std::enable_if_t< !std::is_integral_v< T1 > || !std::is_integral_v< T2 >, size_t > cantorHash( const std::pair< T1, T2 > &values )
    {
        return cantorHash( values.first, values.second );
    }
}

namespace
{
    TEST( TestCantorHash, TestCantorHash0 )
    {
        auto tmp = std::vector< int >( { 1, 2, 3 } );
        EXPECT_EQ( 0, NTowel42Utils::cantorHash( tmp.begin(), tmp.begin() ) );
    }

    TEST( TestCantorHash, TestCantorHash1 )
    {
        EXPECT_EQ( 1, NTowel42Utils::cantorHash( { 1 } ) );
        EXPECT_EQ( 3, NTowel42Utils::cantorHash( { 2 } ) );

        EXPECT_EQ( 1, NTowel42Utils::cantorHash( 1 ) );
        EXPECT_EQ( 3, NTowel42Utils::cantorHash( 2 ) );
    }

    TEST( TestCantorHash, TestCantorHash2 )
    {
        EXPECT_EQ( 8, NTowel42Utils::cantorHash( { 1, 2 } ) );
        EXPECT_EQ( 7, NTowel42Utils::cantorHash( { 2, 1 } ) );

        EXPECT_EQ( 8, NTowel42Utils::cantorHash( 1, 2 ) );
        EXPECT_EQ( 7, NTowel42Utils::cantorHash( 2, 1 ) );
        EXPECT_EQ( -1, NTowel42Utils::cantorHash( 1, 2.0 ) );

        EXPECT_EQ( 8, NTowel42Utils::cantorHash( std::make_pair( 1, 2 ) ) );
        EXPECT_EQ( 7, NTowel42Utils::cantorHash( std::make_pair( 2, 1 ) ) );
        EXPECT_EQ( -1, NTowel42Utils::cantorHash( std::make_pair( 1, 2.0 ) ) );

        EXPECT_EQ( 8, NTowel42Utils::cantorHash( std::make_tuple( 1, 2 ) ) );
        EXPECT_EQ( 7, NTowel42Utils::cantorHash( std::make_tuple( 2, 1 ) ) );
        EXPECT_EQ( -1, NTowel42Utils::cantorHash( std::make_tuple( 1, 2.0 ) ) );

        EXPECT_EQ( 18, NTowel42Utils::cantorHash( std::make_pair( 2, 3 ) ) );
        EXPECT_EQ( 32, NTowel42Utils::cantorHash( std::make_pair( 3, 4 ) ) );
        EXPECT_EQ( 627, NTowel42Utils::cantorHash( std::make_pair( 2, 32 ) ) );
        EXPECT_EQ( 198133, NTowel42Utils::cantorHash( std::make_pair( 1, 627 ) ) );
    }

    TEST( TestCantorHash, TestCantorHash3 )
    {
        EXPECT_EQ( 208, NTowel42Utils::cantorHash( { 1, 2, 3 } ) );
        EXPECT_EQ( 188, NTowel42Utils::cantorHash( { 1, 3, 2 } ) );
        EXPECT_EQ( 133, NTowel42Utils::cantorHash( { 2, 1, 3 } ) );
        EXPECT_EQ( 102, NTowel42Utils::cantorHash( { 2, 3, 1 } ) );
        EXPECT_EQ( 74, NTowel42Utils::cantorHash( { 3, 1, 2 } ) );
        EXPECT_EQ( 62, NTowel42Utils::cantorHash( { 3, 2, 1 } ) );

        EXPECT_EQ( 208, NTowel42Utils::cantorHash( std::list< int >( { 1, 2, 3 } ) ) );
        EXPECT_EQ( 188, NTowel42Utils::cantorHash( std::list< int >( { 1, 3, 2 } ) ) );
        EXPECT_EQ( 133, NTowel42Utils::cantorHash( std::list< int >( { 2, 1, 3 } ) ) );
        EXPECT_EQ( 102, NTowel42Utils::cantorHash( std::list< int >( { 2, 3, 1 } ) ) );
        EXPECT_EQ( 74, NTowel42Utils::cantorHash( std::list< int >( { 3, 1, 2 } ) ) );
        EXPECT_EQ( 62, NTowel42Utils::cantorHash( std::list< int >( { 3, 2, 1 } ) ) );

        EXPECT_EQ( 208, NTowel42Utils::cantorHash( std::make_tuple( 1, 2, 3 ) ) );
        EXPECT_EQ( 188, NTowel42Utils::cantorHash( std::make_tuple( 1, 3, 2 ) ) );
        EXPECT_EQ( 133, NTowel42Utils::cantorHash( std::make_tuple( 2, 1, 3 ) ) );
        EXPECT_EQ( 102, NTowel42Utils::cantorHash( std::make_tuple( 2, 3, 1 ) ) );
        EXPECT_EQ( 74, NTowel42Utils::cantorHash( std::make_tuple( 3, 1, 2 ) ) );
        EXPECT_EQ( 62, NTowel42Utils::cantorHash( std::make_tuple( 3, 2, 1 ) ) );

        EXPECT_EQ( 208, NTowel42Utils::cantorHash( 1, 2, 3 ) );
        EXPECT_EQ( 188, NTowel42Utils::cantorHash( 1, 3, 2 ) );
        EXPECT_EQ( 133, NTowel42Utils::cantorHash( 2, 1, 3 ) );
        EXPECT_EQ( 102, NTowel42Utils::cantorHash( 2, 3, 1 ) );
        EXPECT_EQ( 74, NTowel42Utils::cantorHash( 3, 1, 2 ) );
        EXPECT_EQ( 62, NTowel42Utils::cantorHash( 3, 2, 1 ) );
    }

    TEST( TestCantorHash, TestCantorHash4 )
    {
        EXPECT_EQ( 198133, NTowel42Utils::cantorHash( { 1, 2, 3, 4 } ) );
        EXPECT_EQ( 176713, NTowel42Utils::cantorHash( { 1, 2, 4, 3 } ) );
        EXPECT_EQ( 93959, NTowel42Utils::cantorHash( { 1, 3, 2, 4 } ) );
        EXPECT_EQ( 70874, NTowel42Utils::cantorHash( { 1, 3, 4, 2 } ) );
        EXPECT_EQ( 37399, NTowel42Utils::cantorHash( { 1, 4, 2, 3 } ) );
        EXPECT_EQ( 31373, NTowel42Utils::cantorHash( { 1, 4, 3, 2 } ) );

        EXPECT_EQ( 177903, NTowel42Utils::cantorHash( { 2, 1, 3, 4 } ) );
        EXPECT_EQ( 158200, NTowel42Utils::cantorHash( { 2, 1, 4, 3 } ) );
        EXPECT_EQ( 37947, NTowel42Utils::cantorHash( { 2, 3, 1, 4 } ) );
        EXPECT_EQ( 21942, NTowel42Utils::cantorHash( { 2, 3, 4, 1 } ) );
        EXPECT_EQ( 14362, NTowel42Utils::cantorHash( { 2, 4, 1, 3 } ) );
        EXPECT_EQ( 9042, NTowel42Utils::cantorHash( { 2, 4, 3, 1 } ) );

        EXPECT_EQ( 72386, NTowel42Utils::cantorHash( { 3, 1, 2, 4 } ) );
        EXPECT_EQ( 53624, NTowel42Utils::cantorHash( { 3, 1, 4, 2 } ) );
        EXPECT_EQ( 32381, NTowel42Utils::cantorHash( { 3, 2, 1, 4 } ) );
        EXPECT_EQ( 18332, NTowel42Utils::cantorHash( { 3, 2, 4, 1 } ) );
        EXPECT_EQ( 4091, NTowel42Utils::cantorHash( { 3, 4, 1, 2 } ) );
        EXPECT_EQ( 2999, NTowel42Utils::cantorHash( { 3, 4, 2, 1 } ) );

        EXPECT_EQ( 22786, NTowel42Utils::cantorHash( { 4, 1, 2, 3 } ) );
        EXPECT_EQ( 18716, NTowel42Utils::cantorHash( { 4, 1, 3, 2 } ) );
        EXPECT_EQ( 9586, NTowel42Utils::cantorHash( { 4, 2, 1, 3 } ) );
        EXPECT_EQ( 5773, NTowel42Utils::cantorHash( { 4, 2, 3, 1 } ) );
        EXPECT_EQ( 3155, NTowel42Utils::cantorHash( { 4, 3, 1, 2 } ) );
        EXPECT_EQ( 2273, NTowel42Utils::cantorHash( { 4, 3, 2, 1 } ) );

        EXPECT_EQ( 198133, NTowel42Utils::cantorHash( std::list< int >( { 1, 2, 3, 4 } ) ) );
        EXPECT_EQ( 176713, NTowel42Utils::cantorHash( std::list< int >( { 1, 2, 4, 3 } ) ) );
        EXPECT_EQ( 93959, NTowel42Utils::cantorHash( std::list< int >( { 1, 3, 2, 4 } ) ) );
        EXPECT_EQ( 70874, NTowel42Utils::cantorHash( std::list< int >( { 1, 3, 4, 2 } ) ) );
        EXPECT_EQ( 37399, NTowel42Utils::cantorHash( std::list< int >( { 1, 4, 2, 3 } ) ) );
        EXPECT_EQ( 31373, NTowel42Utils::cantorHash( std::list< int >( { 1, 4, 3, 2 } ) ) );

        EXPECT_EQ( 177903, NTowel42Utils::cantorHash( std::list< int >( { 2, 1, 3, 4 } ) ) );
        EXPECT_EQ( 158200, NTowel42Utils::cantorHash( std::list< int >( { 2, 1, 4, 3 } ) ) );
        EXPECT_EQ( 37947, NTowel42Utils::cantorHash( std::list< int >( { 2, 3, 1, 4 } ) ) );
        EXPECT_EQ( 21942, NTowel42Utils::cantorHash( std::list< int >( { 2, 3, 4, 1 } ) ) );
        EXPECT_EQ( 14362, NTowel42Utils::cantorHash( std::list< int >( { 2, 4, 1, 3 } ) ) );
        EXPECT_EQ( 9042, NTowel42Utils::cantorHash( std::list< int >( { 2, 4, 3, 1 } ) ) );

        EXPECT_EQ( 72386, NTowel42Utils::cantorHash( std::list< int >( { 3, 1, 2, 4 } ) ) );
        EXPECT_EQ( 53624, NTowel42Utils::cantorHash( std::list< int >( { 3, 1, 4, 2 } ) ) );
        EXPECT_EQ( 32381, NTowel42Utils::cantorHash( std::list< int >( { 3, 2, 1, 4 } ) ) );
        EXPECT_EQ( 18332, NTowel42Utils::cantorHash( std::list< int >( { 3, 2, 4, 1 } ) ) );
        EXPECT_EQ( 4091, NTowel42Utils::cantorHash( std::list< int >( { 3, 4, 1, 2 } ) ) );
        EXPECT_EQ( 2999, NTowel42Utils::cantorHash( std::list< int >( { 3, 4, 2, 1 } ) ) );

        EXPECT_EQ( 22786, NTowel42Utils::cantorHash( std::list< int >( { 4, 1, 2, 3 } ) ) );
        EXPECT_EQ( 18716, NTowel42Utils::cantorHash( std::list< int >( { 4, 1, 3, 2 } ) ) );
        EXPECT_EQ( 9586, NTowel42Utils::cantorHash( std::list< int >( { 4, 2, 1, 3 } ) ) );
        EXPECT_EQ( 5773, NTowel42Utils::cantorHash( std::list< int >( { 4, 2, 3, 1 } ) ) );
        EXPECT_EQ( 3155, NTowel42Utils::cantorHash( std::list< int >( { 4, 3, 1, 2 } ) ) );
        EXPECT_EQ( 2273, NTowel42Utils::cantorHash( std::list< int >( { 4, 3, 2, 1 } ) ) );

        EXPECT_EQ( 198133, NTowel42Utils::cantorHash( std::make_tuple( 1, 2, 3, 4 ) ) );
        EXPECT_EQ( 176713, NTowel42Utils::cantorHash( std::make_tuple( 1, 2, 4, 3 ) ) );
        EXPECT_EQ( 93959, NTowel42Utils::cantorHash( std::make_tuple( 1, 3, 2, 4 ) ) );
        EXPECT_EQ( 70874, NTowel42Utils::cantorHash( std::make_tuple( 1, 3, 4, 2 ) ) );
        EXPECT_EQ( 37399, NTowel42Utils::cantorHash( std::make_tuple( 1, 4, 2, 3 ) ) );
        EXPECT_EQ( 31373, NTowel42Utils::cantorHash( std::make_tuple( 1, 4, 3, 2 ) ) );

        EXPECT_EQ( 177903, NTowel42Utils::cantorHash( std::make_tuple( 2, 1, 3, 4 ) ) );
        EXPECT_EQ( 158200, NTowel42Utils::cantorHash( std::make_tuple( 2, 1, 4, 3 ) ) );
        EXPECT_EQ( 37947, NTowel42Utils::cantorHash( std::make_tuple( 2, 3, 1, 4 ) ) );
        EXPECT_EQ( 21942, NTowel42Utils::cantorHash( std::make_tuple( 2, 3, 4, 1 ) ) );
        EXPECT_EQ( 14362, NTowel42Utils::cantorHash( std::make_tuple( 2, 4, 1, 3 ) ) );
        EXPECT_EQ( 9042, NTowel42Utils::cantorHash( std::make_tuple( 2, 4, 3, 1 ) ) );

        EXPECT_EQ( 72386, NTowel42Utils::cantorHash( std::make_tuple( 3, 1, 2, 4 ) ) );
        EXPECT_EQ( 53624, NTowel42Utils::cantorHash( std::make_tuple( 3, 1, 4, 2 ) ) );
        EXPECT_EQ( 32381, NTowel42Utils::cantorHash( std::make_tuple( 3, 2, 1, 4 ) ) );
        EXPECT_EQ( 18332, NTowel42Utils::cantorHash( std::make_tuple( 3, 2, 4, 1 ) ) );
        EXPECT_EQ( 4091, NTowel42Utils::cantorHash( std::make_tuple( 3, 4, 1, 2 ) ) );
        EXPECT_EQ( 2999, NTowel42Utils::cantorHash( std::make_tuple( 3, 4, 2, 1 ) ) );

        EXPECT_EQ( 22786, NTowel42Utils::cantorHash( std::make_tuple( 4, 1, 2, 3 ) ) );
        EXPECT_EQ( 18716, NTowel42Utils::cantorHash( std::make_tuple( 4, 1, 3, 2 ) ) );
        EXPECT_EQ( 9586, NTowel42Utils::cantorHash( std::make_tuple( 4, 2, 1, 3 ) ) );
        EXPECT_EQ( 5773, NTowel42Utils::cantorHash( std::make_tuple( 4, 2, 3, 1 ) ) );
        EXPECT_EQ( 3155, NTowel42Utils::cantorHash( std::make_tuple( 4, 3, 1, 2 ) ) );
        EXPECT_EQ( 2273, NTowel42Utils::cantorHash( std::make_tuple( 4, 3, 2, 1 ) ) );

        EXPECT_EQ( 198133, NTowel42Utils::cantorHash( 1, 2, 3, 4 ) );
        EXPECT_EQ( 176713, NTowel42Utils::cantorHash( 1, 2, 4, 3 ) );
        EXPECT_EQ( 93959, NTowel42Utils::cantorHash( 1, 3, 2, 4 ) );
        EXPECT_EQ( 70874, NTowel42Utils::cantorHash( 1, 3, 4, 2 ) );
        EXPECT_EQ( 37399, NTowel42Utils::cantorHash( 1, 4, 2, 3 ) );
        EXPECT_EQ( 31373, NTowel42Utils::cantorHash( 1, 4, 3, 2 ) );

        EXPECT_EQ( 177903, NTowel42Utils::cantorHash( 2, 1, 3, 4 ) );
        EXPECT_EQ( 158200, NTowel42Utils::cantorHash( 2, 1, 4, 3 ) );
        EXPECT_EQ( 37947, NTowel42Utils::cantorHash( 2, 3, 1, 4 ) );
        EXPECT_EQ( 21942, NTowel42Utils::cantorHash( 2, 3, 4, 1 ) );
        EXPECT_EQ( 14362, NTowel42Utils::cantorHash( 2, 4, 1, 3 ) );
        EXPECT_EQ( 9042, NTowel42Utils::cantorHash( 2, 4, 3, 1 ) );

        EXPECT_EQ( 72386, NTowel42Utils::cantorHash( 3, 1, 2, 4 ) );
        EXPECT_EQ( 53624, NTowel42Utils::cantorHash( 3, 1, 4, 2 ) );
        EXPECT_EQ( 32381, NTowel42Utils::cantorHash( 3, 2, 1, 4 ) );
        EXPECT_EQ( 18332, NTowel42Utils::cantorHash( 3, 2, 4, 1 ) );
        EXPECT_EQ( 4091, NTowel42Utils::cantorHash( 3, 4, 1, 2 ) );
        EXPECT_EQ( 2999, NTowel42Utils::cantorHash( 3, 4, 2, 1 ) );

        EXPECT_EQ( 22786, NTowel42Utils::cantorHash( 4, 1, 2, 3 ) );
        EXPECT_EQ( 18716, NTowel42Utils::cantorHash( 4, 1, 3, 2 ) );
        EXPECT_EQ( 9586, NTowel42Utils::cantorHash( 4, 2, 1, 3 ) );
        EXPECT_EQ( 5773, NTowel42Utils::cantorHash( 4, 2, 3, 1 ) );
        EXPECT_EQ( 3155, NTowel42Utils::cantorHash( 4, 3, 1, 2 ) );
        EXPECT_EQ( 2273, NTowel42Utils::cantorHash( 4, 3, 2, 1 ) );

        EXPECT_EQ( -1, NTowel42Utils::cantorHash( std::make_tuple( 1, 2.0, 3, 4.0 ) ) );
    }
}

int main( int argc, char **argv )
{
    ::testing::InitGoogleTest( &argc, argv );
    int retVal = RUN_ALL_TESTS();
    return retVal;
}
