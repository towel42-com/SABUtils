/*
 * (c) Copyright 2004 - 2025 Blue Pearl Software Inc.
 * All rights reserved.
 *
 * This source code belongs to Blue Pearl Software Inc.
 * It is considered trade secret and confidential, and is not to be used
 * by parties who have not received written authorization
 * from Blue Pearl Software Inc.
 *
 * Only authorized users are allowed to use, copy and modify
 * this software provided that the above copyright notice
 * remains in all copies of this software.
 *
 *
 * $Author: simon $ - $Revision: 66418 $ - $Date: 2025-01-13 12:32:35 -0800 (Mon, 13 Jan 2025) $
 * $HeadURL: http://bpsvn/svn/trunk/Shared/DBUtils/DBUtils.cpp $
 *
 *
*/
#ifdef TOWEL42_QSQL_SUPPORT

    #include "DBUtils.h"
    #include <cassert>

    #include <QSqlQuery>
    #include <QSqlRecord>
    #include <QSqlError>
    #include <QDebug>
    #include <QMap>
    #include <QRegularExpression>
    #include <QStringList>
    #include <QThread>

namespace NTowel42Utils
{

    QString getThreadName()
    {
        auto currThread = QThread::currentThread();
        QString retVal = QString( "THREAD: %1_%2" ).arg( reinterpret_cast< uintptr_t >( currThread ) ).arg( currThread ? currThread->objectName() : "" );
        return retVal;
    }

    bool runCmd( QSqlQuery &query )
    {
        if ( !query.exec() )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }

        return true;
    }

    bool validateQuery( QSqlQuery &query )
    {
        QRegularExpression regEx( R"__((\?)|(\:\w*))__" );
        if ( !regEx.isValid() )
            return false;

        auto cmd = query.lastQuery();
        int numParam = 0;
        int pos = 0;
        auto ii = regEx.globalMatch( cmd );
        while ( ii.hasNext() )
        {
            auto match = ii.next();
            numParam++;
            query.addBindValue( QString( "%1" ).arg( numParam ) );
            auto curr = match.capturedTexts()[ 0 ];
            if ( curr[ 0 ] == ':' )
            {
                Q_ASSERT( curr.toLower() == curr );
                if ( curr.toLower() != curr )
                    return false;
            }
        }

        auto boundValues = query.boundValues();
        if ( boundValues.size() != numParam )
            return false;

        if ( !query.exec() )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            return false;
        }

        if ( query.lastError().type() != QSqlError::NoError )
            return false;
        return true;
    }

    bool clearDatabase( QSqlDatabase &db, bool close )
    {
        if ( !db.isOpen() )
            return true;

        QSqlQuery query( db );

        bool aOK = runCmd( query, "SELECT tbl_name from sqlite_master where type='table' and name NOT LIKE 'sqlite%'" );
        Q_ASSERT( aOK );
        QSet< QString > tables;
        while ( query.next() )
        {
            tables.insert( query.value( 0 ).toString() );
        }

        for ( QSet< QString >::iterator ii = tables.begin(); aOK && ii != tables.end(); ++ii )
        {
            aOK = runCmd( query, QString( "DROP TABLE IF EXISTS %1" ).arg( ( *ii ) ) );
            query.finish();
        }
        if ( close )
            db.close();
        return true;
    }

    bool validateParams( const QSqlQuery &query, std::size_t numParams )
    {
        QRegularExpression regEx( R"__((\?)|(\:\w*))__" );
        if ( !regEx.isValid() )
            return false;

        QString cmd = query.lastQuery();
        std::size_t numParam = 0;

        auto ii = regEx.globalMatch( cmd );
        while ( ii.hasNext() )
        {
            auto match = ii.next();
            numParam++;
            auto curr = match.capturedTexts()[ 0 ];
            if ( curr[ 0 ] == ':' )
            {
                Q_ASSERT( curr.toLower() == curr );
                if ( curr.toLower() != curr )
                    return false;
            }
        }

        auto keys = query.boundValueNames();
        for ( auto &&key : keys )
        {
            Q_ASSERT( key.toLower() == key );
            if ( key.toLower() != key )
                return false;
        }
        Q_ASSERT( query.boundValues().size() == numParams );
        return ( query.boundValues().size() == numParams );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const std::map< QString, QVariant > &namedParams )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }

        for ( auto &&ii : namedParams )
        {
            query.bindValue( ii.first, ii.second );
        }

    #ifdef _DEBUG
        validateParams( query, namedParams.size() );
    #endif
        return runCmd( query );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const QMap< QString, QVariant > &namedParams )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }

        for ( QMap< QString, QVariant >::const_iterator ii = namedParams.begin(); ii != namedParams.end(); ++ii )
        {
            query.bindValue( ii.key(), ii.value() );
        }

    #ifdef _DEBUG
        validateParams( query, namedParams.size() );
    #endif
        return runCmd( query );
    }

    bool runCmd( QSqlQuery &query, const QList< QVariant > &params )
    {
        for ( int ii = 0; ii < params.count(); ++ii )
            query.bindValue( ii, params[ ii ] );

    #ifdef _DEBUG
        validateParams( query, params.size() );
    #endif
        return runCmd( query );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const std::list< QVariantList > &params )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }

        for ( auto && param : params )
            query.addBindValue( param );

    #ifdef _DEBUG
        validateParams( query, params.size() );
    #endif

        if ( !query.execBatch() )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }
        return true;
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const QList< QVariantList > &params )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }

        for ( int ii = 0; ii < params.count(); ++ii )
            query.addBindValue( params[ ii ] );

    #ifdef _DEBUG
        validateParams( query, params.size() );
    #endif

        if ( !query.execBatch() )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }
        return true;
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const std::list< QVariant > &params )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }

        for ( auto &&ii : params )
        {
            query.addBindValue( ii );
        }

    #ifdef _DEBUG
        validateParams( query, params.size() );
    #endif

        return runCmd( query );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const QList< QVariant > &params )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            qDebug() << getThreadName() << ": " << query.lastError().driverText();
            qDebug() << getThreadName() << ": " << query.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }

        for ( int ii = 0; ii < params.count(); ++ii )
            query.bindValue( ii, params[ ii ] );

    #ifdef _DEBUG
        validateParams( query, params.size() );
    #endif

        return runCmd( query );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const QVariant &param )
    {
        return runCmd( query, cmd, QList< QVariant >() << param );
    }

    bool transaction( QSqlDatabase &db )
    {
        if ( !db.transaction() )
        {
            qDebug() << getThreadName() << ": " << db.lastError().driverText();
            qDebug() << getThreadName() << ": " << db.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }
        return true;
    }

    bool commit( QSqlDatabase &db )
    {
        if ( !db.commit() )
        {
            qDebug() << getThreadName() << ": " << db.lastError().driverText();
            qDebug() << getThreadName() << ": " << db.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }
        return true;
    }

    bool rollback( QSqlDatabase &db )
    {
        if ( !db.rollback() )
        {
            qDebug() << getThreadName() << ": " << db.lastError().driverText();
            qDebug() << getThreadName() << ": " << db.lastError().databaseText();
            Q_ASSERT( 0 );
            return false;
        }
        return true;
    }

    bool tableExists( QSqlQuery &query, const QString &tableName, QSet< QString > *columns )
    {
        bool retVal = false;
        if ( runCmd( query, "PRAGMA table_info('" + tableName + "');" ) )
        {
            while ( query.next() )
            {
                retVal = true;
                if ( columns )
                    columns->insert( query.value( 1 ).toString().toLower() );
                else
                    break;
            }
        }
        return retVal;
    }

    bool AddColumn( QSqlQuery &query, const QString &tableName, const QString &columnName, const QString &columnDef, bool *colAdded )
    {
        if ( colAdded )
            *colAdded = false;
        QSet< QString > columns;
        if ( !tableExists( query, tableName, &columns ) )
            return true;

        if ( columns.contains( columnName.toLower() ) )
            return true;

        if ( colAdded )
            *colAdded = true;
        return runCmd( query, "ALTER TABLE '" + tableName + "' ADD " + columnDef );
    }
}

QTextStream &operator<<( QTextStream &ds, const QSqlQuery &query )
{
    ds << "Query: '" + query.executedQuery() + "'";
    ds << " - Bindings : (";
    auto keys = query.boundValueNames();
    auto values = query.boundValues();
    Q_ASSERT( keys.size() == values.size() );

    for ( qsizetype ii = 0; ii < std::min( keys.size(), values.size() ); ++ii )
    {
        if ( ii != 0 )
            ds << ",";
        auto &&key = keys.at( ii );
        auto &&value = values.at( ii );
        ds << " " << key << "=";
        if ( value.isNull() )
            ds << "nullptr";
        else
            ds << value.toString();
    }
    ds << ")";
    ds << "\nQuery at ";
    if ( query.at() == QSql::BeforeFirstRow )
        ds << "BeforeFirstRow";
    else if ( query.at() == QSql::AfterLastRow )
        ds << "AfterLastRow";
    else
    {
        const QSqlRecord &rec = query.record();
        ds << query.at() << " Record Count: " << rec.count() << " Values: ( ";
        for ( int ii = 0; ii < rec.count(); ++ii )
        {
            if ( ii != 0 )
                ds << ",";
            ds << " " << rec.fieldName( ii ) << " = ";
            if ( rec.value( ii ).isNull() )
                ds << "nullptr";
            else
                ds << rec.value( ii ).toString();
        }
        ds << " )";
    }
    return ds;
}

QDebug &operator<<( QDebug &dbg, const QSqlQuery &query )
{
    QString result;
    QTextStream ts( &result );
    ts << query;
    dbg << result;
    return dbg;
}

#endif


