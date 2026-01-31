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

#ifndef __JSONUTILS_H
#define __JSONUTILS_H

#include "Towel42UtilsExport.h"
#include <QDate>
#include <set>
#include <list>
#include <unordered_map>
#include <QJsonObject>
#include <QJsonArray>

namespace NTowel42Utils
{
    template< typename T >
    inline QJsonValue toJson( const T &value )
    {
        return QJsonValue( value );
    }

    template<>
    inline QJsonValue toJson( const QDate &date )
    {
        QJsonObject retVal;
        if ( !date.isValid() )
            return {};
        retVal.insert( "year", date.year() );
        retVal.insert( "month", date.month() );
        retVal.insert( "day", date.day() );
        return retVal;
    }

    TOWEL42_UTILS_EXPORT QJsonValue toJson( const QStringList &value );

    template< typename T >
    inline QJsonValue toJson( const std::list< T > &value )
    {
        QJsonArray retVal;
        for ( auto &&ii : value )
        {
            auto curr = toJson( ii );
            retVal.append( curr );
        }
        return retVal;
    }

    template< typename T >
    inline QJsonValue toJson( const std::set< T > &value )
    {
        QJsonArray retVal;
        for ( auto &&ii : value )
        {
            auto curr = toJson( ii );
            retVal.append( curr );
        }
        return retVal;
    }

    template< typename T1, typename T2 >
    inline QJsonValue toJson( const std::pair< T1, T2 > &value )
    {
        QJsonArray retVal;

        auto first = toJson( value.first );
        retVal.append( first );

        auto second = toJson( value.second );
        retVal.append( second );

        return retVal;
    }

    template< typename T2 >
    inline QJsonValue toJson( const std::unordered_map< QString, T2 > &value )
    {
        QJsonObject retVal;
        for ( auto &&ii : value )
        {
            auto curr = toJson( ii );
            retVal[ ii.first ] = curr;
        }
        return retVal;
    }

    TOWEL42_UTILS_EXPORT bool fromJson( QStringList &value, const QJsonObject &val, const QString &keyName );
    TOWEL42_UTILS_EXPORT bool fromJson( bool &value, const QJsonObject &val, const QString &keyName );
    TOWEL42_UTILS_EXPORT bool fromJson( double &value, const QJsonObject &val, const QString &keyName );
    TOWEL42_UTILS_EXPORT bool fromJson( QString &value, const QJsonObject &val, const QString &keyName );
    TOWEL42_UTILS_EXPORT bool fromJson( int &value, const QJsonObject &val, const QString &keyName );

    TOWEL42_UTILS_EXPORT bool fromJson( QStringList &value, const QJsonValue &val );
    TOWEL42_UTILS_EXPORT bool fromJson( bool &value, const QJsonValue &val );
    TOWEL42_UTILS_EXPORT bool fromJson( double &value, const QJsonValue &val );
    TOWEL42_UTILS_EXPORT bool fromJson( QString &value, const QJsonValue &val );
    TOWEL42_UTILS_EXPORT bool fromJson( int &value, const QJsonValue &val );
    TOWEL42_UTILS_EXPORT bool fromJson( QDate &value, const QJsonValue &val );

    template< typename T >
    inline bool fromJson( std::list< T > &value, const QJsonValue &val )
    {
        value.clear();
        if ( !val.isArray() )
            return false;
        auto array = val.toArray();
        for ( int ii = 0; ii < array.count(); ++ii )
        {
            QJsonValue currValue = array.at( ii );
            T currObj;
            fromJson( currObj, currValue );
            value.push_back( currObj );
        }
        return true;
    }

    template< typename T >
    inline bool fromJson( std::list< T > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T >
    inline bool fromJson( std::set< T > &value, const QJsonValue &val )
    {
        value.clear();
        if ( !val.isArray() )
            return false;
        auto array = val.toArray();
        for ( int ii = 0; ii < array.count(); ++ii )
        {
            QJsonValue currValue = array.at( ii );
            T currObj;
            if ( !fromJson( currObj, currValue ) )
                return false;
            value.insert( currObj );
        }
        return true;
    }

    template< typename T >
    inline bool fromJson( std::set< T > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T1, typename T2 >
    inline bool fromJson( std::pair< T1, T2 > &value, const QJsonValue &val )
    {
        value = std::make_pair( T1(), T2() );
        if ( !val.isArray() )
            return false;
        auto array = val.toArray();
        if ( array.count() != 2 )
            return false;
        auto first = array.at( 0 );
        if ( !fromJson( value.first, first ) )
            return false;

        auto second = array.at( 1 );
        if ( !fromJson( value.second, second ) )
            return false;

        return true;
    }

    template< typename T1, typename T2 >
    inline bool fromJson( std::pair< T1, T2 > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T2 >
    inline bool fromJson( std::unordered_map< QString, T2 > &value, const QJsonValue &val )
    {
        value.clear();
        if ( !val.isObject() )
            return false;
        auto map = val.toObject();
        for ( auto &&ii = map.constBegin(); ii != map.constEnd(); ++ii )
        {
            auto key = ii.key();
            auto currValue = ii.value();
            T2 currObj;
            if ( !fromJson( currObj, currValue ) )
                return false;
            value[ key ] = currObj;
        }
        return true;
    }

    template< typename T2 >
    inline bool fromJson( std::unordered_map< QString, T2 > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T >
    inline bool fromJson( std::optional< T > &value, const QJsonValue &val )
    {
        value.reset();

        T tmpValue;
        if ( !fromJson( tmpValue, val ) )
            return false;

        value = tmpValue;
        return true;
    }

    template< typename T >
    inline bool fromJson( std::optional< T > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T >
    inline std::optional< T > fromJson( const QJsonValue &val )
    {
        T retVal;
        auto aOK = fromJson( retVal, val );
        if ( aOK )
            return retVal;
        return {};
    }
}
#endif
