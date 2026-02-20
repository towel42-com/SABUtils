// The MIT License( MIT )
//
// Copyright( c ) 2022 Scott Aron Bloom
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

#include "Towel42UtilsExport.h"

#ifdef TOWEL42_QSQL_SUPPORT
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

namespace NTowel42Utils
{
    using TParameterVariantMap = std::unordered_map< QString, QVariant >;
    using TParameterStringMap = std::unordered_map< QString, QString >;

    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const QList< QVariantList > &params );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const QList< QVariant > &params );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const QMap< QString, QVariant > &namedParams );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const std::list< QVariantList > &params );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const std::list< QVariant > &params = std::list< QVariant >() );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const TParameterVariantMap &namedParams );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const TParameterStringMap &namedParams );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QString &cmd, const QVariant &param );
    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query );

    TOWEL42_UTILS_EXPORT bool runCmd( QSqlQuery &query, const QList< QVariant > &params );

    TOWEL42_UTILS_EXPORT void reportError( const QSqlError &error, bool assert = true );
    TOWEL42_UTILS_EXPORT void reportError( const QSqlQuery &query, bool assert = true );
    TOWEL42_UTILS_EXPORT void reportError( const QSqlDatabase &db, bool assert = true );

    TOWEL42_UTILS_EXPORT bool transaction( QSqlDatabase &db );
    TOWEL42_UTILS_EXPORT bool commit( QSqlDatabase &db );
    TOWEL42_UTILS_EXPORT bool rollback( QSqlDatabase &db );

    TOWEL42_UTILS_EXPORT bool tableExists( QSqlQuery &query, const QString &tableName, QSet< QString > *columns = nullptr );
    TOWEL42_UTILS_EXPORT bool addColumn( QSqlQuery &query, const QString &tableName, const QString &columnName, const QString &columnDef, bool *colAdded = nullptr );
    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, std::size_t numParams );
    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, const TParameterVariantMap & params );
    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, const TParameterStringMap &params );
    TOWEL42_UTILS_EXPORT bool validateParams( const QSqlQuery &query, const QMap< QString, QVariant > &params );
    TOWEL42_UTILS_EXPORT bool validateQuery( QSqlQuery &query );

    TOWEL42_UTILS_EXPORT bool clearDatabase( QSqlDatabase &db, bool close );

    TOWEL42_UTILS_EXPORT bool validateSQLITEInstalled( QString *msg );

}
TOWEL42_UTILS_EXPORT QDebug &operator<<( QDebug &dbg, const QSqlQuery &query );
TOWEL42_UTILS_EXPORT QTextStream &operator<<( QTextStream &ds, const QSqlQuery &query );

#endif

#endif
