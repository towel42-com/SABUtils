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

#include "SetupSystemLogging.h"
#include "SetupSystemLogging_pri.h"
#include "SetupSystemLoggingDlg.h"
#include "TimeStamp.h"

#include <QLatin1StringView>
#include <QLoggingCategory>
#include <QDateTime>
#include <QFile>
#include <QCoreApplication>

Q_LOGGING_CATEGORY( t42utils_base, "Towel42Utils", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch, "Towel42Utils.autoFetch", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch_isVisible, "Towel42Utils.autoFetch.isVisible", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch_findFirstVisible, "Towel42Utils.autoFetch.findFirstVisible", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoFetch_expandingIndex, "Towel42Utils.autoFetch.expandingIndex", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoSize, "Towel42Utils.autoSize", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_autoTabStop, "Towel42Utils.autoTabStop", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_backgroundFileCheck, "Towel42Utils.backgroundFileCheck", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_buttonEnabler, "Towel42Utils.buttonEnabler", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dbUtils, "Towel42Utils.dbUtils", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dbUtils_reportError, "Towel42Utils.dbUtils.reportError", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dbUtils_validateQuery, "Towel42Utils.dbUtils.validateQuery", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_doubleProgressDlg, "Towel42Utils.doubleProgressDlg", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_downloadFile, "Towel42Utils.downloadFile", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_fileUtils, "Towel42Utils.fileUtils", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_findAllFiles, "Towel42Utils.findAllFiles", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_GitHubGetVersions, "Towel42Utils.GitHubGetVersions", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_MD5, "Towel42Utils.MD5", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_getDate, "Towel42Utils.getDate", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_appendToLog, "Towel42Utils.appendToLog", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_toDate, "Towel42Utils.toDate", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_dumpRow, "Towel42Utils.dumpRow", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_regExReplace, "Towel42Utils.regExReplace", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_setReadOnly, "Towel42Utils.setReadOnly", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_systemInfo, "Towel42Utils.systemInfo", QtMsgType::QtDebugMsg );
Q_LOGGING_CATEGORY( t42utils_widgetChanged, "Towel42Utils.widgetChanged", QtMsgType::QtDebugMsg );

namespace NTowel42Utils
{
    namespace NSystemLogging
    {
        void initTowel42UtilCategories()
        {
            qCInfo( t42utils_base );
            qCInfo( t42utils_autoFetch );
            qCInfo( t42utils_autoFetch_isVisible );
            qCInfo( t42utils_autoFetch_findFirstVisible );
            qCInfo( t42utils_autoFetch_expandingIndex );
            qCInfo( t42utils_autoSize );
            qCInfo( t42utils_autoTabStop );
            qCInfo( t42utils_backgroundFileCheck );
            qCInfo( t42utils_buttonEnabler );
            qCInfo( t42utils_dbUtils );
            qCInfo( t42utils_dbUtils_reportError );
            qCInfo( t42utils_dbUtils_validateQuery );
            qCInfo( t42utils_doubleProgressDlg );
            qCInfo( t42utils_downloadFile );
            qCInfo( t42utils_fileUtils );
            qCInfo( t42utils_findAllFiles );
            qCInfo( t42utils_GitHubGetVersions );
            qCInfo( t42utils_MD5 );
            qCInfo( t42utils_getDate );
            qCInfo( t42utils_appendToLog );
            qCInfo( t42utils_toDate );
            qCInfo( t42utils_dumpRow );
            qCInfo( t42utils_regExReplace );
            qCInfo( t42utils_setReadOnly );
            qCInfo( t42utils_systemInfo );
            qCInfo( t42utils_widgetChanged );
        }

        static QString sAppName;
        static QStringList sOtherPrefixes;

        std::list< QLoggingCategory * > sCategories;
        void getCategories( QLoggingCategory *category )
        {
            sCategories.push_back( category );
        }

        void init()
        {
            static bool sBeenInitialized = false;
            if ( sBeenInitialized )
                return;
            sBeenInitialized = true;
            initTowel42UtilCategories();

            auto sOldCategoryFilter = QLoggingCategory::installFilter( getCategories );
            QLoggingCategory::installFilter( sOldCategoryFilter );

            auto allPrefixes = QStringList() << sOtherPrefixes << "Towel42Utils";
            sCategories.remove_if(
                [ allPrefixes ]( const QLoggingCategory *category )
                {
                    if ( !category )
                        return true;

                    for ( const auto &prefix : allPrefixes )
                    {
                        auto currName = QLatin1StringView( category->categoryName() );
                        if ( currName.startsWith( prefix ) )
                        {
                            return false;
                        }
                    }
                    return true;
                } );

            sCategories.sort(   //
                []( const QLoggingCategory *lhs, const QLoggingCategory *rhs )   //
                {   //
                    return QLatin1StringView( lhs->categoryName() ) < QLatin1StringView( rhs->categoryName() );
                } );

            sQtCategories.sort();
        }

        std::list< SCategoryInfo > loggingCategoryNames( bool includeQt )
        {
            init();
            std::list< SCategoryInfo > retVal;
            for ( auto &&ii : sCategories )
            {
                retVal.emplace_back( ii );
            }
            if ( includeQt )
            {
                for ( auto &&ii : sQtCategories )
                {
                    retVal.emplace_back( ii );
                }
            }

            return retVal;
        }

        static QtMessageHandler sOriginalHandler = nullptr;
        static QString sFileName;
        static FILE *sFile = nullptr;
        void logToFile( QtMsgType type, const QMessageLogContext &context, const QString &msg )
        {
            auto message = NTowel42Utils::CTimeStamp::getTimeStamp( false, true ) + qFormatLogMessage( type, context, msg );

            if ( !sFile )
            {
                sFile = fopen( sFileName.toLocal8Bit(), "a" );
                fprintf( sFile, "%s\n", qPrintable( message ) );
                fflush( sFile );
            }
            if ( sOriginalHandler )
                sOriginalHandler( type, context, msg );
        }

        void setupSystemLogging( const QString &appName, const QStringList &otherPrefixes )
        {
            Q_ASSERT_X( sAppName.isEmpty() && sOtherPrefixes.isEmpty(), "setupLogging", "Only call NTowel42Utils::NSystemLogging::setupLogging once" );

            sAppName = appName;
            sOtherPrefixes = otherPrefixes;
            sOtherPrefixes << QString( sAppName ).remove( " " );

            auto args = QCoreApplication::arguments();
            std::optional< bool > uiMode;
            if ( args.contains( "--debug_system_logging" ) )
                uiMode = true;
            if ( !uiMode.has_value() )
            {
                if ( args.contains( "--nodebug_system_logging" ) )
                    uiMode = false;
            }
            if ( !uiMode.has_value() )
            {
                auto envVar = qgetenv( "TOWEL42_DEBUG_SYSTEM_LOGGING" );
                uiMode = !envVar.isEmpty() && ( envVar != QByteArrayView( "0" ) );
            }

            if ( uiMode.has_value() && uiMode.value() )
            {
                CSetupSystemLoggingDlg dlg( sAppName, nullptr );
                dlg.exec();
            }
            else
                setupSystemLogging();
        }

        void setDefaultLogCategories()
        {
            std::unordered_set< QString > rulesSet;
            auto rules = QStringList() << "*=false";
            rulesSet.insert( rules.front() );

            auto categoryNames = loggingCategoryNames( false );
            for ( auto &&curr : categoryNames )
            {
                auto currRules = curr.rules( true );
                for ( auto &&jj : currRules )
                {
                    if ( rulesSet.find( jj ) == rulesSet.end() )
                    {
                        rulesSet.insert( jj );
                        rules.push_back( jj );
                    }
                }
            }
            auto rulesText = rules.join( "\n" );
            qCDebug( t42utils_base ).noquote().nospace() << "Setting log filter rules to:\n" << rulesText;
            QLoggingCategory::setFilterRules( rulesText );
        }

        void setupSystemLogging()
        {
            setDefaultLogCategories();
            if ( sFile )
            {
                fclose( sFile );
                sFile = nullptr;
            }

            if ( qEnvironmentVariableIsSet( "T42_ENABLE_LOG_FILE" ) )
            {
                if ( sFileName.isEmpty() )
                {
                    auto envFileName = qgetenv( "T42_LOG_FILE" );
                    if ( envFileName.isEmpty() )
                    {
                        sFileName = QString( "%1_%2.log" ).arg( sAppName ).arg( QDateTime::currentDateTime().toString( "yyyyMMdd-HHmmss" ) );
                    }
                    else
                        sFileName = QString::fromUtf8( envFileName );
                }
                auto append = false;
                if ( qEnvironmentVariableIsSet( "TOWEL42_APPEND_TO_LOG" ) && ( qgetenv( "TOWEL42_APPEND_TO_LOG" ) == QByteArrayView( "0" ) ) )
                {
                    auto fi = QFile( sFileName );
                    if ( fi.exists() )
                    {
                        fi.remove();
                    }
                }
                auto prevHandler = qInstallMessageHandler( logToFile );
                if ( !sOriginalHandler )
                    sOriginalHandler = prevHandler;
                qCDebug( t42utils_base ) << "New Log File";
                qCDebug( t42utils_base ) << "========================================================================================================";
            }
        }

        SCategoryInfo::SCategoryInfo( QLoggingCategory *category, bool owned ) :
            fName( QLatin1StringView( category->categoryName() ) ),
            fDebug( category->isDebugEnabled() ),
            fInfo( category->isInfoEnabled() ),
            fWarning( category->isWarningEnabled() ),
            fCritical( category->isCriticalEnabled() )
        {
            if ( owned )
                delete category;
        }

        SCategoryInfo::SCategoryInfo( QLoggingCategory *category ) :
            SCategoryInfo( category, false )
        {
        }

        SCategoryInfo::SCategoryInfo( const QString &category ) :
            SCategoryInfo( new QLoggingCategory( category.toLatin1() ), true )
        {
        }

        QStringList SCategoryInfo::rules( bool enabled ) const
        {
            auto categoryList = fName.split( "." );
            auto retVal = QStringList();
            QString fullName;

            for ( auto &&ii : categoryList )
            {
                if ( !fullName.isEmpty() )
                    fullName += ".";
                fullName += ii;
                retVal << QString( "%1=%2" ).arg( fullName ).arg( enabled ? "true" : "false" );
            }
            return retVal;
        }

    }
}