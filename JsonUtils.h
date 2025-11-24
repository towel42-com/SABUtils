// The MIT License( MIT )
//
// Copyright( c ) 2020-2021 Scott Aron Bloom
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

#include "SABUtilsExport.h"

#include <set>
#include <list>
#include <unordered_map>
#include <QJsonObject>
#include <QJsonArray>

namespace NSABUtils
{
    template< typename T >
    void toJson( const T &value, QJsonValue &obj )
    {
        obj = QJsonValue( value );
    }

    template< typename T >
    void toJson( const T &value, QJsonValueRef &obj )
    {
        obj = QJsonValue( value );
    }

    SABUTILS_EXPORT QJsonValue toJson( const QStringList &value );

    template< typename T >
    void toJson( const std::list< T > &value, QJsonValue &obj )
    {
        QJsonArray retVal;
        for ( auto &&ii : value )
        {
            QJsonValue curr;
            toJson( ii, curr );
            retVal.append( curr );
        }
        obj = retVal;
    }

    template< typename T >
    void toJson( const std::list< T > &value, QJsonValueRef &obj )
    {
        QJsonArray retVal;
        for ( auto &&ii : value )
        {
            QJsonValue curr;
            toJson( ii, curr );
            retVal.append( curr );
        }
        obj = retVal;
    }

    template< typename T >
    void toJson( const std::set< T > &value, QJsonValue &obj )
    {
        QJsonArray retVal;
        for ( auto &&ii : value )
        {
            QJsonValue curr;
            toJson( ii, curr );
            retVal.append( curr );
        }
        obj = retVal;
    }

    template< typename T >
    void toJson( const std::set< T > &value, QJsonValueRef &obj )
    {
        QJsonArray retVal;
        for ( auto &&ii : value )
        {
            QJsonValue curr;
            toJson( ii, curr );
            retVal.append( curr );
        }
        obj = retVal;
    }

    template< typename T1, typename T2 >
    void toJson( const std::pair< T1, T2 > &value, QJsonValue &obj )
    {
        QJsonArray retVal;

        QJsonValue first;
        toJson( value.first, first );
        retVal.append( first );

        QJsonValue second;
        toJson( value.second, second );
        retVal.append( second );

        obj = retVal;
    }

    template< typename T1, typename T2 >
    void toJson( const std::pair< T1, T2 > &value, QJsonValueRef &obj )
    {
        QJsonArray retVal;

        QJsonValue first;
        toJson( value.first, first );
        retVal.append( first );

        QJsonValue second;
        toJson( value.second, second );
        retVal.append( second );

        obj = retVal;
    }

    template< typename T2 >
    void toJson( const std::unordered_map< QString, T2 > &value, QJsonValue &obj )
    {
        QJsonObject retVal;
        for ( auto &&ii : value )
        {
            QJsonValue curr;
            toJson( ii.second, curr );
            retVal[ ii.first ] = curr;
        }
        obj = retVal;
    }

    template< typename T2 >
    void toJson( const std::unordered_map< QString, T2 > &value, QJsonValueRef &obj )
    {
        QJsonObject retVal;
        for ( auto &&ii : value )
        {
            QJsonValue curr;
            toJson( ii.second, curr );
            retVal[ ii.first ] = curr;
        }
        obj = retVal;
    }

    SABUTILS_EXPORT bool fromJson( QStringList &value, const QJsonObject &val, const QString &keyName );
    SABUTILS_EXPORT bool fromJson( bool &value, const QJsonObject &val, const QString &keyName );
    SABUTILS_EXPORT bool fromJson( double &value, const QJsonObject &val, const QString &keyName );
    SABUTILS_EXPORT bool fromJson( QString &value, const QJsonObject &val, const QString &keyName );
    SABUTILS_EXPORT bool fromJson( int &value, const QJsonObject &val, const QString &keyName );

    SABUTILS_EXPORT bool fromJson( QStringList &value, const QJsonValue &val );
    SABUTILS_EXPORT bool fromJson( bool &value, const QJsonValue &val );
    SABUTILS_EXPORT bool fromJson( double &value, const QJsonValue &val );
    SABUTILS_EXPORT bool fromJson( QString &value, const QJsonValue &val );
    SABUTILS_EXPORT bool fromJson( int &value, const QJsonValue &val );

    template< typename T >
    bool fromJson( std::list< T > &value, const QJsonValue &val )
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
    bool fromJson( std::list< T > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T >
    bool fromJson( std::set< T > &value, const QJsonValue &val )
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
    bool fromJson( std::set< T > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T1, typename T2 >
    bool fromJson( std::pair< T1, T2 > &value, const QJsonValue &val )
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
    bool fromJson( std::pair< T1, T2 > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T2 >
    bool fromJson( std::unordered_map< QString, T2 > &value, const QJsonValue &val )
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
    bool fromJson( std::unordered_map< QString, T2 > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }

    template< typename T >
    bool fromJson( std::optional< T > &value, const QJsonValue &val )
    {
        value.reset();

        T tmpValue;
        if ( !fromJson( tmpValue, val ) )
            return false;

        value = tmpValue;
        return true;
    }

    template< typename T >
    bool fromJson( std::optional< T > &value, const QJsonObject &obj, const QString &keyName )
    {
        auto objValue = obj[ keyName ];
        return fromJson( value, objValue );
    }
}
#endif
