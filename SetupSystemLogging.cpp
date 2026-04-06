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

Q_LOGGING_CATEGORY( t42utils_base, "Towel42Utils", QtMsgType::QtDebugMsg )
namespace NTowel42Utils
{
    namespace NSystemLogging
    {
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

            qCInfo( t42utils_base );

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

        void setupSystemLogging( const QString &appName, const QStringList &otherPrefixes, bool uiMode )
        {
            Q_ASSERT_X( sAppName.isEmpty() && sOtherPrefixes.isEmpty(), "setupLogging", "Only call NTowel42Utils::NSystemLogging::setupLogging once" );

            sAppName = appName;
            sOtherPrefixes = otherPrefixes;
            sOtherPrefixes << QString( sAppName ).remove( " " );

            if ( !uiMode )
                setupSystemLogging();
            else
            {
                CSetupSystemLoggingDlg dlg( sAppName, nullptr );
                dlg.exec();
            }
        }

        void setupSystemLogging()
        {
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

    }
}