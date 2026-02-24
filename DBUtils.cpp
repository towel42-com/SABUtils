// The MIT License( MIT )
//
// Copyright( c ) 2020-2025 Towel 42 Development LLC and Scott Aron Bloom
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

#ifdef TOWEL42_QSQL_SUPPORT

    #include "DBUtils.h"
    #include <cassert>
    #include <unordered_map>
    #include <unordered_set>

    #include <QSqlQuery>
    #include <QSqlRecord>
    #include <QSqlError>
    #include <QDebug>
    #include <QMap>
    #include <QRegularExpression>
    #include <QStringList>
    #include <QThread>
    #include <tuple>

namespace NTowel42Utils
{
    QString getThreadName()
    {
        auto currThread = QThread::currentThread();
        QString retVal = QString( "THREAD: %1_%2" ).arg( reinterpret_cast< uintptr_t >( currThread ) ).arg( currThread ? currThread->objectName() : "" );
        return retVal;
    }

    void reportError( const QSqlError &error, bool assert )
    {
        if ( error.type() != QSqlError::NoError )
        {
            qDebug() << getThreadName() << ": " << error.driverText();
            qDebug() << getThreadName() << ": " << error.databaseText();
            if ( assert )
                Q_ASSERT( error.type() == QSqlError::NoError );
        }
    }
    void reportError( const QSqlQuery &query, bool assert )
    {
        reportError( query.lastError(), assert );
    }

    void reportError( const QSqlDatabase &db, bool assert )
    {
        reportError( db.lastError(), assert );
    }

    bool runCmd( QSqlQuery &query )
    {
        if ( !query.exec() )
        {
            reportError( query );
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
            reportError( query, false );
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

    bool validateSQLITEInstalled( QString *msg )
    {
        if ( !QSqlDatabase::isDriverAvailable( "QSQLITE" ) )
        {
            if ( msg )
                *msg = QObject::tr( "Could not find Database Driver libraries.  Please re-install or contact support." );
            return false;
        }
        return true;
    }

    QString convertQtToSqliteDTFormat( const QString &qtFormat )
    {
        auto retVal = qtFormat;
        std::list< std::pair< QString, QString > > formats =   //
            {
                //
                { "dddd", "%d" },   // does not exist in sqlite
                { "ddd", "%d" },   // does not exist in sqlite
                { "dd", "%d" },   //
                { "d", "%d" },   // %e is not supported in all versions of sqlite
                { "MMMM", "%m" },   // DNE
                { "MMM", "%m" },   // DNE
                { "MM", "%m" },   //
                { "M", "%m" },   // DNE
                { "yyyy", "%Y" },   // 4 digit year
                { "yy", "%G" },   // 2 digit year
                { "hh", "%I" },   //
                { "h", "%l" },   //
                { "HH", "%H" },   //
                { "H", "%k" },   //
                { "mm", "%M" },   //
                { "m", "%M" },   // DNE
                { "ss", "%S" },   //
                { "s", "%S" },   // DNE
                { "zzz", "%f" },   //
                { "zz", "%f" },   // DNE
                { "z", "%f" },   // DNE
                { "aP", "%P" },   // DNE
                { "Pa", "%P" },   // DNE
                { "AP", "%P" },   //
                { "A", "%P" },   //  DNE
                { "ap", "%p" },   // DNE
                { "a", "%p" },   // DNE
                { "tttt", "" },   // DNE
                { "ttt", "" },   // DNE
                { "tt", "" },   // DNE
                { "t", "" },   // DNE
            };
        for ( auto &&ii : formats )
        {
            retVal = retVal.replace( QRegularExpression( "([^%]|^)" + ii.first ), R"__(\1)__" + ii.second );
        }
        return retVal;
    }

    bool validateParams( const QSqlQuery &query, std::size_t numParams )
    {
        if ( !query.boundValueNames().isEmpty() )
        {
            Q_ASSERT( query.boundValueNames().size() == query.boundValues().size() );
            Q_ASSERT( query.boundValueNames().size() == numParams );
        }

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
        auto numBoundWithValue = 0;
        for ( int ii = 0; ii < query.boundValues().count(); ++ii )
        {
            auto value = query.boundValues()[ ii ];
            if ( value.isNull() )
            {
                qDebug() << query.boundValueNames()[ ii ] << " has a null bound value.";
                continue;
            }
            numBoundWithValue++;
        }
        Q_ASSERT( numBoundWithValue == numParams );

        return ( ( numBoundWithValue == numParams ) && ( query.boundValues().size() == numParams ) && ( !query.boundValueNames().empty() && ( query.boundValueNames().size() == query.boundValues().size() ) ) );
    }

    bool validateParams( const QSqlQuery &query, const TParameterVariantMap &params )
    {
        TParameterVariantMap boundValueMap;
        for ( int ii = 0; ii < query.boundValues().count(); ++ii )
        {
            auto value = query.boundValues()[ ii ];
            auto name = query.boundValueNames()[ ii ];
            boundValueMap[ name ] = value;
        }

        QStringList boundNotParam;
        std::list< std::tuple< QString, QVariant, QVariant > > boundIncorrectly;
        for ( auto &&ii : boundValueMap )
        {
            auto pos = params.find( ii.first );
            if ( pos == params.end() )
                boundNotParam << ii.first;
            else
            {
                if ( ( *pos ).second != ii.second )
                    boundIncorrectly.emplace_back( ii.first, ii.second, ( *pos ).second );
            }
        }

        QStringList paramNoBound;
        for ( auto &&ii : params )
        {
            auto pos = boundValueMap.find( ii.first );
            if ( pos == boundValueMap.end() )
                boundNotParam << ii.first;
        }

        auto aOK = boundNotParam.empty();
        Q_ASSERT( boundNotParam.empty() );
        if ( !boundNotParam.empty() )
        {
            qDebug() << "The following are bound but not in param map: ";
            for ( auto &&ii : boundNotParam )
                qDebug() << ii;
        }

        aOK = aOK && boundNotParam.empty();
        Q_ASSERT( boundIncorrectly.empty() );
        if ( !boundIncorrectly.empty() )
        {
            qDebug() << "The following are bound incorrectly: ";
            for ( auto &&ii : boundNotParam )
                qDebug() << ii;
        }

        aOK = aOK && paramNoBound.empty();
        Q_ASSERT( paramNoBound.empty() );
        if ( !paramNoBound.empty() )
        {
            qDebug() << "The following are in the param map but not bound: ";
            for ( auto &&ii : paramNoBound )
                qDebug() << ii;
        }

        return aOK && validateParams( query, params.size() );
    }

    bool validateParams( const QSqlQuery &query, const TParameterStringMap &params )
    {
        TParameterVariantMap realParams;
        for ( auto &&ii : params )
        {
            realParams[ ii.first ] = ii.second;
        }
        return validateParams( query, realParams );
    }

    bool validateParams( const QSqlQuery &query, const QMap< QString, QVariant > &params )
    {
        TParameterVariantMap realParams;
        auto ii = QMapIterator( params );
        while ( ii.hasNext() )
        {
            ii.next();
            realParams[ ii.key() ] = ii.value();
        }
        return validateParams( query, realParams );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const NTowel42Utils::TParameterVariantMap &namedParams )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            reportError( query );
            return false;
        }

        for ( auto &&ii : namedParams )
        {
            query.bindValue( ii.first, ii.second );
        }

    #ifdef _DEBUG
        validateParams( query, namedParams );
    #endif
        return runCmd( query );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const TParameterStringMap &namedParams )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            reportError( query );
            return false;
        }

        for ( auto &&ii : namedParams )
        {
            query.bindValue( ii.first, ii.second );
        }

    #ifdef _DEBUG
        validateParams( query, namedParams );
    #endif
        return runCmd( query );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const QMap< QString, QVariant > &namedParams )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            reportError( query );
            return false;
        }

        for ( QMap< QString, QVariant >::const_iterator ii = namedParams.begin(); ii != namedParams.end(); ++ii )
        {
            query.bindValue( ii.key(), ii.value() );
        }

    #ifdef _DEBUG
        validateParams( query, namedParams );
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
            reportError( query );
            return false;
        }

        for ( auto &&param : params )
            query.addBindValue( param );

    #ifdef _DEBUG
        validateParams( query, params.size() );
    #endif

        if ( !query.execBatch() )
        {
            reportError( query );
            return false;
        }
        return true;
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const QList< QVariantList > &params )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            reportError( query );
            return false;
        }

        for ( int ii = 0; ii < params.count(); ++ii )
            query.addBindValue( params[ ii ] );

    #ifdef _DEBUG
        validateParams( query, params.size() );
    #endif

        if ( !query.execBatch() )
        {
            reportError( query );
            return false;
        }
        return true;
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const std::list< QVariant > &params )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            reportError( query );
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
            reportError( query );
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
            reportError( db );
            return false;
        }
        return true;
    }

    bool commit( QSqlDatabase &db )
    {
        if ( !db.commit() )
        {
            reportError( db );
            return false;
        }
        return true;
    }

    bool rollback( QSqlDatabase &db )
    {
        if ( !db.rollback() )
        {
            reportError( db );
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
        query.clear();
        return retVal;
    }

    bool addColumn( QSqlQuery &query, const QString &tableName, const QString &columnName, const QString &columnDef, bool *colAdded )
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