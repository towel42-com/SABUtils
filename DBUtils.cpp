// The MIT License( MIT )
//
// Copyright( c ) 2020-2025 Towel 42 Development LLC and Scott Aron Bloom
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

#ifdef TOWEL42_QSQL_SUPPORT

    #include "DBUtils.h"
    #include <cassert>
    #include <unordered_map>
    #include <unordered_set>
    #include <list>

    #include <QSqlQuery>
    #include <QSqlDriver>
    #include <QSqlRecord>
    #include <QSqlError>
    #include <QSqlField>
    #include <QDebug>
    #include <QMap>
    #include <QRegularExpression>
    #include <QStringList>
    #include <QThread>
    #include <tuple>

namespace NTowel42Utils
{
    bool runCmd( QSqlQuery &query, const QString &cmd, const QString &paramName, const QVariant &paramValue )
    {
        return runCmd( query, cmd, std::make_pair( paramName, paramValue ) );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const std::pair< QString, QVariant > &param )
    {
        return runCmd( query, cmd, TParameterVariantMap( { { param.first, param.second } } ) );
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

    bool runCmd( QSqlQuery &query, const QString &cmd, const TParameterStringMap &namedParams )
    {
        NTowel42Utils::TParameterVariantMap params;
        for ( auto &&ii : namedParams )
        {
            params.insert( ii );
        }
        return runCmd( query, cmd, params );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const QMap< QString, QVariant > &namedParams )
    {
        NTowel42Utils::TParameterVariantMap params;
        for ( auto &&ii = namedParams.begin(); ii != namedParams.end(); ++ii )
        {
            params.emplace( ii.key(), ii.value() );
        }
        return runCmd( query, cmd, params );
    }

    bool runCmd( QSqlQuery &query, const TParameterVariantMap &params )
    {
        for ( auto &&ii : params )
        {
            query.bindValue( ii.first, ii.second );
        }

    #ifdef _DEBUG
        Q_ASSERT( query.boundValueNames().size() == params.size() );
        Q_ASSERT( query.boundValues().size() == params.size() );
        validateParams( query, params );
    #endif
        return runCmd( query );
    }

    bool validateOnly( QSqlQuery &query, const QString &cmd, bool assert )
    {
        query.clear();
        bool aOK = query.prepare( cmd );
        if ( !aOK )
        {
            reportError( query, assert );
        }

        NTowel42Utils::TParameterVariantMap params;
        auto cmdParams = paramsInCmd( cmd, true );
        for ( auto &&ii : cmdParams )
        {
            if ( assert )
                Q_ASSERT( params.find( ii ) == params.end() );

            if ( params.find( ii ) == params.end() )
            {
                if ( ii.endsWith( "_id" ) )
                    params[ ii ] = 1;
                else
                    params[ ii ] = ii.mid( 1 );
            }
            else
                qDebug() << "Param: " << ii << "is in the sql cmd twice";
        }

        for ( auto &&ii : params )
            query.bindValue( ii.first, ii.second );

        aOK = aOK && validateParams( query, params, assert );

        return aOK;
    }

    bool runCmd( QSqlQuery &query, const QString &cmd, const NTowel42Utils::TParameterVariantMap &namedParams )
    {
        query.clear();

        if ( !query.prepare( cmd ) )
        {
            reportError( query );
            return false;
        }

        return runCmd( query, namedParams );
    }

    bool runCmd( QSqlQuery &query, const QString &cmd )
    {
        query.clear();
        if ( !query.prepare( cmd ) )
        {
            reportError( query );
            return false;
        }

        return runCmd( query );
    }

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

    QStringList paramsInCmd( const QString &cmdText, bool namedOnly )
    {
        QStringList retVal;

        QRegularExpression regEx( R"__((\?)|(\:\w+))__" );
        if ( !regEx.isValid() )
            return retVal;

        auto ii = regEx.globalMatch( cmdText );
        while ( ii.hasNext() )
        {
            auto match = ii.next();
            auto curr = match.capturedTexts()[ 0 ];
            if ( curr.isEmpty() )
                continue;
            if ( !namedOnly || ( curr[ 0 ] == ':' ) )
                retVal << curr;
        }
        return retVal;
    }

    bool validateQuery( QSqlQuery &query, bool assert )
    {
        auto cmd = query.lastQuery();
        auto paramList = paramsInCmd( cmd, false );

        int numParam = 0;
        for ( auto &&currParam : paramList )
        {
            bool namedParam = ( currParam[ 0 ] == ':' );

            if ( !namedParam )
                query.addBindValue( QString( "%1" ).arg( ++numParam ) );
            else if ( currParam.toLower() != currParam )
            {
                if ( assert )
                    Q_ASSERT( currParam.toLower() == currParam );
                qDebug() << QString( "Parameter '%1' should be all lowercase" ).arg( currParam );
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
        return validateDriverInstalled( "QSQLITE", msg );
    }

    bool validateDriverInstalled( const QString &driver, QString *msg )
    {
        if ( !QSqlDatabase::isDriverAvailable( driver ) )
        {
            if ( msg )
                *msg = QObject::tr( "Could not find Database %1 libraries.  Please re-install or contact support." ).arg( driver );
            return false;
        }
        return true;
    }

    QString convertQtToSQLDateTimeFormat( const QString &qtFormat )
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

    bool validateParams( const QSqlQuery &query, std::size_t numParams, bool assert )
    {
        if ( !query.boundValueNames().isEmpty() )
        {
            if ( assert )
            {
                Q_ASSERT( query.boundValueNames().size() == query.boundValues().size() );
                Q_ASSERT( query.boundValueNames().size() == numParams );
            }
        }

        QString cmd = query.lastQuery();
        auto params = paramsInCmd( cmd, true );
        for ( auto &&curr : params )
        {
            if ( curr[ 0 ] == ':' )
            {
                if ( assert )
                    Q_ASSERT( curr.toLower() == curr );
                if ( curr.toLower() != curr )
                    return false;
            }
        }

        auto keys = query.boundValueNames();
        for ( auto &&key : keys )
        {
            if ( assert )
                Q_ASSERT( key.toLower() == key );
            if ( key.toLower() != key )
                return false;
        }
        auto numBoundWithValue = 0;
        for ( int ii = 0; ii < query.boundValues().count(); ++ii )
        {
            auto value = query.boundValues()[ ii ];
            if ( value.isNull() && ( value.userType() != QMetaType::QString ) )
            {
                qDebug() << query.boundValueNames()[ ii ] << " has a null bound value.";
                continue;
            }
            numBoundWithValue++;
        }
        //if ( assert )
        //    Q_ASSERT( numBoundWithValue == numParams );

        auto aOK = true;   //( numBoundWithValue == numParams );
        aOK = aOK && ( query.boundValues().size() == numParams );
        aOK = aOK && ( query.boundValueNames().empty() || ( query.boundValueNames().size() == query.boundValues().size() ) );
        return aOK;
    }

    bool validateParams( const QSqlQuery &query, const TParameterVariantMap &params, bool assert )
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
        if ( !boundNotParam.empty() )
        {
            qDebug() << "The following are bound but not in param map: ";
            for ( auto &&ii : boundNotParam )
                qDebug() << ii;
            if ( assert )
                Q_ASSERT( boundNotParam.empty() );
        }

        aOK = aOK && boundNotParam.empty();
        if ( assert )
            Q_ASSERT( boundIncorrectly.empty() );
        if ( !boundIncorrectly.empty() )
        {
            qDebug() << "The following are bound incorrectly: ";
            for ( auto &&ii : boundNotParam )
                qDebug() << ii;
        }

        aOK = aOK && paramNoBound.empty();
        if ( assert )
            Q_ASSERT( paramNoBound.empty() );
        if ( !paramNoBound.empty() )
        {
            qDebug() << "The following are in the param map but not bound: ";
            for ( auto &&ii : paramNoBound )
                qDebug() << ii;
        }

        return aOK && validateParams( query, params.size(), assert );
    }

    bool validateParams( const QSqlQuery &query, const TParameterStringMap &params )
    {
        TParameterVariantMap realParams;
        for ( auto &&ii : params )
        {
            realParams[ ii.first ] = ii.second;
        }
        return validateParams( query, realParams, true );
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

    bool tableExists( QSqlQuery &query, const QString &tableName, std::list< QString > *columns )
    {
        auto tables = query.driver()->tables( QSql::Tables );

        for ( auto &&ii : tables )
        {
            if ( ii.toLower() == tableName.toLower() )
            {
                if ( columns )
                {
                    auto record = QSqlDatabase::database().record( tableName );
                    for ( int ii = 0; ii < record.count(); ++ii )
                    {
                        columns->push_back( record.fieldName( ii ).toLower() );
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool addColumn( QSqlQuery &query, const QString &tableName, const QString &columnName, const QString &columnDef, bool *colAdded )
    {
        if ( colAdded )
            *colAdded = false;
        std::list< QString > columns;
        if ( !tableExists( query, tableName, &columns ) )
            return true;

        for ( auto &&ii : columns )
        {
            if ( ii.toLower() == columnName.toLower() )
                return true;
        }

        if ( colAdded )
            *colAdded = true;
        return runCmd( query, "ALTER TABLE '" + tableName + "' ADD " + columnDef );
    }

    bool renameTable( QSqlQuery &query, const QString &oldTableName, const QString &newTableName )
    {
        auto cmd = QString( "ALTER TABLE %1 RENAME TO %2" ).arg( oldTableName ).arg( newTableName );
        return runCmd( query, cmd );
    }

    std::optional< QString > backupTable( QSqlQuery &query, const QString &tableName )
    {
        std::optional< int > backupNum;
        QString newTableName;
        do
        {
            newTableName = tableName + "_";
            if ( backupNum.has_value() )
                newTableName += QString( "%1_" ).arg( backupNum.value() );
            else
                backupNum = 0;
            backupNum.value() = backupNum.value() + 1;
            newTableName += "bak";
        }
        while ( tableExists( query, newTableName ) );

        if ( !renameTable( query, tableName, newTableName ) )
        {
            return {};
        }
        return newTableName;
    }

    std::list< NTowel42Utils::SColumnInfo > columnInfoForTable( QSqlQuery &query, const QString &tableName )
    {
        auto cmd = QString( "PRAGMA table_info('%1')" ).arg( tableName );
        if ( !runCmd( query, cmd ) )
            return {};

        std::list< SColumnInfo > columns;
        while ( query.next() )
        {
            SColumnInfo curr;
            int ii = 0;
            auto colID = query.value( ii++ ).toInt();
            auto columnName = query.value( ii++ ).toString();
            auto typeName = query.value( ii++ ).toString();
            auto notNull = query.value( ii++ ).toBool();
            auto defaultValue = query.value( ii++ ).toString();
            auto primaryKey = query.value( ii++ ).toBool();
            columns.emplace_back( colID, columnName, typeName, notNull, defaultValue, primaryKey, QString() );
        }
        return columns;
    }

    QString createTableCommand( const QString &tableName, const std::list< NTowel42Utils::SColumnInfo > &columns, bool checkIfExists )
    {
        QString retVal = "CREATE TABLE ";
        if ( checkIfExists )
            retVal += "IF NOT EXISTS ";
        retVal += tableName + "( \n";
        bool first = true;
        for ( auto &&ii : columns )
        {
            if ( !first )
                retVal += ",";
            else
                retVal += " ";
            retVal += " ";

            retVal += ii.columnDef();
        }
        retVal += ")";
        return retVal;
    }

    bool renameColumn( const QSqlDatabase &db, const QString &tableName, const QString &oldColumnName, const QString &newColumnName )
    {
        QSqlQuery query( db );
        auto version = sqliteVersion( query );
        if ( version.fMajor > 25 )
        {
            // rename column exists

            auto cmd = QString( "ALTER TABLE %1 RENAME COLUMN %2 TO %3" ).arg( tableName ).arg( oldColumnName ).arg( newColumnName );
            return runCmd( query, cmd );
        }
        else
        {
            auto columns = columnInfoForTable( query, tableName );
            bool columnFound = false;
            for ( auto &&ii : columns )
            {
                if ( ii.fName.toLower() == oldColumnName.toLower() )
                {
                    ii.fName = oldColumnName;
                    columnFound = true;
                    break;
                }
            }
            if ( !columnFound )
                return false;

            CTransaction transaction( db );
            auto newTableName = backupTable( query, tableName );
            if ( !newTableName.has_value() )
            {
                transaction.setRollback();
                return false;
            }

            auto cmd = createTableCommand( tableName, columns, false );
            if ( !runCmd( query, cmd ) )
            {
                transaction.setRollback();
                return false;
            }

            if ( !importTable( query, newTableName.value(), tableName, { { oldColumnName, newColumnName } }, {} ) )
            {
                transaction.setRollback();
                return false;
            }
        }
        return true;
    }

    QString getColumnListing( const std::list< NTowel42Utils::SColumnInfo > &columns, const std::unordered_map< QString, QString > &mapping )
    {
        QString retVal;
        bool first = true;
        for ( auto &&ii : columns )
        {
            auto columnName = ii.fName;
            auto pos = mapping.find( columnName );
            if ( pos != mapping.end() )
                columnName = ( *pos ).second;

            if ( !first )
                retVal += ",";
            else
                retVal += " ";
            first = false;
            retVal += " " + columnName + "\n";
        }
        return retVal;
    }

    bool importTable( QSqlQuery &query, const QString &srcTableName, const QString &destTableName, const std::unordered_map< QString, QString > &columnMapping, const std::unordered_map< QString, QString > &srcSelectMapping )
    {
        if ( !tableExists( query, srcTableName ) || !tableExists( query, destTableName ) )
            return false;

        auto srcColumns = columnInfoForTable( query, srcTableName );
        auto destColumns = columnInfoForTable( query, destTableName );

        auto cmd = QString( "INSERT INTO %1(\n" ).arg( destTableName );
        cmd += getColumnListing( srcColumns, columnMapping );

        cmd += ")\n";
        cmd += "SELECT\n";
        cmd += getColumnListing( srcColumns, srcSelectMapping );
        cmd += "FROM " + srcTableName;

        return runCmd( query, cmd );
    }

    bool dropTable( QSqlQuery &query, const QString &tableName )
    {
        auto cmd = QString( "DROP TABLE IF EXISTS %1" ).arg( tableName );
        return runCmd( query, cmd );
    }

    SDBVersion sqliteVersion( QSqlQuery &query )
    {
        query.exec( "select sqlite_version();" );
        if ( !query.next() )
            return {};
        return query.value( 0 ).toString();
    }

    std::optional< int > lastInsertedKey( QSqlQuery &query, const QString & /*tableName*/ )
    {
        qDebug() << ( query.driver()->objectName() );
        qDebug() << ( QSqlDatabase().driverName() );

        auto cmd = QStringLiteral( "SELECT LAST_INSERT_ID();" );
        auto aOK = runCmd( query, cmd ) && query.next();
        if ( !aOK )
            return {};
        return query.value( 0 ).toInt();
    }

    QString enumStringForValue( const TEnumValueVector &enumValues, int value )
    {
        for ( int ii = 0; ii < enumValues.size(); ++ii )
        {
            if ( enumValues[ ii ].first == value )
                return enumValues[ ii ].second;
        }
        return {};
    }

    std::optional< int > enumValueForString( const TEnumValueVector &enumValues, const QString &string )
    {
        for ( int ii = 0; ii < enumValues.size(); ++ii )
        {
            if ( enumValues[ ii ].second == string )
                return enumValues[ ii ].first;
        }
        return {};
    }

    QString SColumnInfo::columnDef() const
    {
        QStringList retVal;

        retVal << fName;
        retVal << fType;
        if ( fNotNull )
            retVal << "NOT" << "NULL";
        if ( fPrimaryKey )
        {
            retVal << "PRIMARY" << "KEY" << "AUTOINCREMENT";
        }
        else
        {
            if ( fDefaultValue.isEmpty() )
            {
                retVal << "DEFAULT" << fDefaultValue;
            }
            if ( !fConstraint.isEmpty() )
                retVal << "CHECK(" << fConstraint << ")";
        }
        return retVal.join( " " );
    }

    SColumnInfo::SColumnInfo( int colID, const QString &name, const QString &colType, bool notNull, const QString &defValue, bool primKey, const QString &contraint ) :
        fColID( colID ),
        fName( name ),
        fType( colType ),
        fNotNull( notNull ),
        fDefaultValue( defValue ),
        fPrimaryKey( primKey ),
        fConstraint( contraint )
    {
    }

    CTransaction::CTransaction() :
        CTransaction( QSqlDatabase::database() )
    {
    }

    CTransaction::CTransaction( const QSqlDatabase &db ) :
        fDatabase( db )
    {
        transaction( fDatabase );
    }

    CTransaction::~CTransaction()
    {
        if ( fRollback )
            rollback( fDatabase );
        else
            commit( fDatabase );
    }

    SDBVersion::SDBVersion( const QString &str )
    {
        auto tmp = str.split( "." );
        Q_ASSERT( !tmp.isEmpty() );
        if ( tmp.isEmpty() )
            return;
        bool aOK = false;
        fMajor = tmp.front().toInt( &aOK );
        Q_ASSERT( aOK && !tmp.isEmpty() );
        if ( !aOK )
            return;
        tmp.pop_front();

        Q_ASSERT( !tmp.isEmpty() );
        if ( tmp.isEmpty() )
            return;
        fMinor = tmp.front().toInt( &aOK );
        Q_ASSERT( aOK && !tmp.isEmpty() );
        if ( !aOK )
            return;
        tmp.pop_front();

        Q_ASSERT( !tmp.isEmpty() );
        if ( tmp.isEmpty() )
            return;
        fPatch = tmp.front().toInt( &aOK );
        Q_ASSERT( aOK && !tmp.isEmpty() );
        if ( !aOK )
            return;
        tmp.pop_front();
        Q_ASSERT( tmp.isEmpty() );
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