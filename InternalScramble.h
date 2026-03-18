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
#include <type_traits>
#include <initializer_list>

#include <QByteArray>
namespace NTowel42Utils
{
    template< class T >
    struct is_8bits : std::integral_constant< bool, std::numeric_limits< T >::digits == 8 || std::numeric_limits< T >::digits == 7 > 
    {
    };

    template< class T >
    constexpr bool is_8bits_v = is_8bits< T >::value;


    template< typename T >
    inline std::enable_if_t< is_8bits_v< T >, T > swapNibbles( T value )
    {
        return ( ( ( value & 0x0F ) << 4 ) | ( ( value & 0xF0 ) >> 4 ) );
    };

    template< typename T1, typename T2 >
    inline std::enable_if_t< is_8bits_v< T1 > && is_8bits_v< T2 >, QByteArray > descramble( const T1 &lhs, const T2 &rhs )
    {
        return QByteArray( 1, NTowel42Utils::swapNibbles( lhs ) ) + QByteArray( 1, NTowel42Utils::swapNibbles( rhs ) );
    }

    template< typename T1, typename T2 >
    inline std::enable_if_t< is_8bits_v< T1 > && std::is_same_v< QByteArray, T2 >, QByteArray > descramble( const T1 &lhs, const T2 &rhs )
    {
        return QByteArray( 1, NTowel42Utils::swapNibbles( lhs ) ) + rhs;
    }

    template< typename T >
    inline QByteArray  descramble( T lhs )
    {
        static_assert( std::is_same_v< QChar, T > || is_8bits_v< T > );
        return QByteArray( 1, NTowel42Utils::swapNibbles( lhs ) );
    }

    template< typename T, typename... Targs >
    inline std::enable_if_t< ( sizeof...( Targs ) == 1 ), QByteArray > descramble( T lhs, T rhs, Targs... Fargs )
    {
        return descramble( lhs, descramble( rhs, Fargs... ) );
    }

    template< typename T, typename... Targs >
    inline std::enable_if_t< ( sizeof...( Targs ) >= 2 ), QByteArray > descramble( T lhs, Targs... Fargs )
    {
        return descramble( lhs, descramble( Fargs... ) );
    }

#define CHAR( x ) NTowel42Utils::swapNibbles( x )
}