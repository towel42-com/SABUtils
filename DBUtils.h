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

    #include <QVariant>
    #include <QList>
    #include <QMap>
    #include <list>
    #include <map>

namespace NTowel42Utils
{
    bool runCmd( QSqlQuery &query, const QString &cmd, const QList< QVariantList > &params );
    bool runCmd( QSqlQuery &query, const QString &cmd, const QList< QVariant > &params );
    bool runCmd( QSqlQuery &query, const QString &cmd, const QMap< QString, QVariant > &namedParams );
    bool runCmd( QSqlQuery &query, const QString &cmd, const std::list< std::list< QVariant > > &params );
    bool runCmd( QSqlQuery &query, const QString &cmd, const std::list< QVariant > &params = std::list< QVariant >() );
    bool runCmd( QSqlQuery &query, const QString &cmd, const std::map< QString, QVariant > &namedParams );
    bool runCmd( QSqlQuery &query, const QString &cmd, const QVariant &param );
    bool runCmd( QSqlQuery &query );

    bool runCmd( QSqlQuery &query, const QList< QVariant > &params );

    bool transaction( QSqlDatabase &db );
    bool commit( QSqlDatabase &db );
    bool rollback( QSqlDatabase &db );

    bool tableExists( QSqlQuery &query, const QString &tableName, QSet< QString > *columns );
    bool AddColumn( QSqlQuery &query, const QString &tableName, const QString &columnName, const QString &columnDef, bool *colAdded = nullptr );
    bool validateParams( const QSqlQuery &query, int numParams );
    bool validateQuery( QSqlQuery &query );

    bool clearDatabase( QSqlDatabase &db, bool close );
}
QDebug &operator<<( QDebug &dbg, const QSqlQuery &query );
QTextStream &operator<<( QTextStream &ds, const QSqlQuery &query );

#endif

#endif
