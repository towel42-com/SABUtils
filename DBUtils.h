// The MIT License( MIT )
//
// Copyright( c ) 2022 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __DBUTILS_H
#define __DBUTILS_H

#include "DBUtilsFwd.h"

#include "Towel42UtilsExport.h"

#ifdef QT_SQL_LIB
class QString;
class QSqlQuery;
class QSqlDatabase;
class QDebug;
class QTextStream;
class QSqlError;

    #include <QVariant>
    #include <QList>
    #include <QMap>
    #include <list>
    #include <unordered_map>
    #include <cstdint>
    #include <QSqlDatabase>

namespace NTowel42Utils
{
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const QMap< QString, QVariant > &namedParams );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const TParameterVariantMap &namedParams );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const TParameterStringMap &namedParams );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const QString &paramName, const QVariant &paramValue );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const std::pair< QString, QVariant > &param );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd );

    TOWEL42_UTILS_EXPORT bool reportError( const QSqlError &error, bool assert = true );
    TOWEL42_UTILS_EXPORT bool reportError( const QSqlQuery &query, bool assert = true );
    TOWEL42_UTILS_EXPORT bool reportError( const QSqlDatabase &db, bool assert = true );

    TOWEL42_UTILS_EXPORT bool transaction( QSqlDatabase &db );
    TOWEL42_UTILS_EXPORT bool commit( QSqlDatabase &db );
    TOWEL42_UTILS_EXPORT bool rollback( QSqlDatabase &db );

    TOWEL42_UTILS_EXPORT bool tableExists( QSqlQuery &query, const QString &tableName, std::list< QString > *columns = nullptr );
    TOWEL42_UTILS_EXPORT bool addColumn( QSqlQuery &query, const QString &tableName, const QString &columnName, const QString &columnDef, bool *colAdded = nullptr );
    TOWEL42_UTILS_EXPORT bool renameTable( QSqlQuery &query, const QString &oldTableName, const QString &newTableName );
    TOWEL42_UTILS_EXPORT bool renameColumn( QSqlDatabase &db, const QString &tableName, const QString &oldColumnName, const QString &newColumnName );
    TOWEL42_UTILS_EXPORT bool importTable( QSqlQuery &query, const QString &srcTableName, const QString &destTableName, const std::unordered_map< QString, QString > &columnMapping, const std::unordered_map< QString, QString > &srcSelectMapping );
    TOWEL42_UTILS_EXPORT bool dropTable( QSqlQuery &query, const QString &tableName );
    TOWEL42_UTILS_EXPORT std::optional< QString > backupTable( QSqlQuery &query, const QString &tableName );   // if successful returns the new table name

    TOWEL42_UTILS_EXPORT std::list< SColumnInfo > columnInfoForTable( QSqlQuery &query, const QString &tableName );

    TOWEL42_UTILS_EXPORT QStringList paramsInCmd( const QString &cmdText, bool namedOnly );

    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, std::size_t numParams, bool assert = true );
    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, const TParameterVariantMap &params, bool assert = true );
    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, const TParameterStringMap &params, bool assert = true );
    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, const QMap< QString, QVariant > &params, bool assert = true );
    TOWEL42_UTILS_EXPORT bool validateQuery( QSqlQuery &query, bool assert = true );

    TOWEL42_UTILS_EXPORT bool validateOnly( QSqlQuery &query, const QString &cmd, bool assert );

    TOWEL42_UTILS_EXPORT bool clearDatabase( QSqlDatabase &db, bool close );

    TOWEL42_UTILS_EXPORT bool validateSQLITEInstalled( QString *msg );
    TOWEL42_UTILS_EXPORT bool validateDriverInstalled( const QString &driver, QString *msg );

    TOWEL42_UTILS_EXPORT QString convertQtToSQLDateTimeFormat( const QString &qtFormat );

    TOWEL42_UTILS_EXPORT SDBVersion sqliteVersion( QSqlQuery &query );

    template< typename T >
    bool hasValue( const std::shared_ptr< NTowel42Utils::TParameterVariantMap > &values, const QString &label )
    {
        if ( !values )
            return false;

        auto pos = values->find( label );
        if ( pos == values->end() )
            return false;

        if constexpr ( std::is_same_v< QString, T > || std::is_same_v< QVariant, T > )
            return true;

        if ( !( *pos ).second.canConvert< T >() )
            return false;

        return true;
    }

    template< typename T >
    T getValue( const std::shared_ptr< NTowel42Utils::TParameterVariantMap > &values, const QString &label )
    {
        if ( !hasValue< T >( values, label ) )
            return {};

        auto pos = values->find( label );
        if ( pos == values->end() )
            return {};
        auto var = ( *pos ).second;
        return var.value< T >();
    }

    TOWEL42_UTILS_EXPORT QString enumStringForValue( const TEnumValueVector &enumValues, int value );
    TOWEL42_UTILS_EXPORT std::optional< int > enumValueForString( const TEnumValueVector &enumValues, const QString &string );
}
TOWEL42_UTILS_EXPORT QDebug &operator<<( QDebug &dbg, const QSqlQuery &query );
TOWEL42_UTILS_EXPORT QTextStream &operator<<( QTextStream &ds, const QSqlQuery &query );

#endif

#endif
