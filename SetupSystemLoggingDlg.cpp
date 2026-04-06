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

#include "SetupSystemLoggingDlg.h"
#include "ui_SetupSystemLoggingDlg.h"

#include "SetupSystemLogging.h"
#include "AutoSize.h"

#include <QFileDialog>
#include <QLoggingCategory>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

namespace NTowel42Utils
{
    namespace NSystemLogging
    {
        CSetupSystemLoggingDlg::CSetupSystemLoggingDlg( const QString &appName, QWidget *parent ) :
            QDialog( parent ),
            fImpl( new Ui::CSetupSystemLoggingDlg )
        {
            fImpl->setupUi( this );
            fModel = new QStandardItemModel( this );

            fFilterModel = new QSortFilterProxyModel( this );
            fFilterModel->setSourceModel( fModel );
            fFilterModel->setRecursiveFilteringEnabled( true );
            connect( fImpl->filter, &QLineEdit::textChanged, this, &CSetupSystemLoggingDlg::slotFilterChanged );

            fImpl->categories->setModel( fFilterModel );

            slotLoadCategories();

            connect(
                fImpl->setLogFile, &QToolButton::clicked,
                [ = ]()
                {
                    auto file = QFileDialog::getSaveFileName( nullptr, QObject::tr( "Log File" ), appName + ".log", QObject::tr( "Log Files (*.log);;All Files (*.*)" ) );
                    if ( !file.isEmpty() )
                        fImpl->logFile->setText( file );
                } );

            connect( fImpl->showQtCategories, &QCheckBox::clicked, this, &CSetupSystemLoggingDlg::slotLoadCategories );
            connect(
                fImpl->checkAll, &QCheckBox::clicked,
                [ = ]()
                {
                    for ( int ii = 0; ii < fFilterModel->rowCount(); ++ii )
                    {
                        checkItem( fModel->item( ii ), true );
                    }
                } );
            connect(
                fImpl->uncheckAll, &QCheckBox::clicked,
                [ = ]()
                {
                    for ( int ii = 0; ii < fFilterModel->rowCount(); ++ii )
                    {
                        checkItem( fModel->item( ii ), false );
                    }
                } );
        }

        CSetupSystemLoggingDlg::~CSetupSystemLoggingDlg()
        {
        }

        QString CSetupSystemLoggingDlg::fullName( QStandardItem *item ) const
        {
            if ( !item )
                return {};

            auto retVal = fullName( item->parent() );
            if ( !retVal.isEmpty() )
                retVal += ".";
            retVal += item->text();

            return retVal;
        }

        QStringList CSetupSystemLoggingDlg::getRules( QStandardItem *item ) const
        {
            if ( !item )
                return {};

            QStringList retVal;
            if ( item->checkState() == Qt::Checked )
                retVal << fullName( item ) + "=true";
            else
                retVal << fullName( item ) + "=false";
            for ( auto ii = 0; ii < item->rowCount(); ++ii )
            {
                auto curr = getRules( item->child( ii ) );
                retVal << curr;
            }
            return retVal;
        }

        QString CSetupSystemLoggingDlg::getRules( bool forEnv ) const
        {
            auto rules = QStringList() << "*=false";

            for ( int ii = 0; ii < fModel->rowCount(); ++ii )
            {
                rules << getRules( fModel->item( ii ) );
            }
            return rules.join( forEnv ? ";" : "\n" );
        }

        QString CSetupSystemLoggingDlg::logFile() const
        {
            return fImpl->logFile->text();
        }

        void CSetupSystemLoggingDlg::slotFilterChanged( const QString &filter )
        {
            fImpl->checkAll->setText( filter.isEmpty() ? tr( "Check All" ) : tr( "Check All Visible" ) );
            fImpl->uncheckAll->setText( filter.isEmpty() ? tr( "Un-check All" ) : tr( "Un-check All Visible" ) );
            fFilterModel->setFilterFixedString( filter );
            fImpl->categories->expandAll();
            NTowel42Utils::autoSize( fImpl->categories );
        }

        void CSetupSystemLoggingDlg::slotLoadCategories()
        {
            fModel->clear();
            fItems.clear();
            fModel->setColumnCount( 5 );
            fModel->setHorizontalHeaderLabels( QStringList() << tr( "Logging Category" ) << tr( "Debug Enabled" ) << tr( "Info Enabled" ) << tr( "Warning Enabled" ) << tr( "Critical Enabled" ) );

            auto categoryNames = loggingCategoryNames( fImpl->showQtCategories->isChecked() );

            for ( auto &&ii : categoryNames )
            {
                loadCategory( ii );
            }
            fImpl->categories->expandAll();
            NTowel42Utils::autoSize( fImpl->categories );
        }

        void CSetupSystemLoggingDlg::loadCategory( const SCategoryInfo &category )
        {
            auto categoryList = category.fName.split( "." );
            QString fullName;
            for ( auto &&curr : categoryList )
            {
                QStandardItem *parent = nullptr;

                if ( !fullName.isEmpty() )
                {
                    auto pos = fItems.find( fullName );
                    if ( pos != fItems.end() )
                    {
                        parent = ( *pos ).second;
                    }
                }

                if ( !fullName.isEmpty() )
                    fullName += ".";
                fullName += curr;
                auto pos = fItems.find( fullName );
                if ( pos != fItems.end() )
                {
                    continue;
                }

                auto items = QList< QStandardItem * >();

                items << new QStandardItem( curr );
                items << new QStandardItem( category.fDebug ? QStringLiteral( "Yes" ) : QStringLiteral( "" ) );
                items << new QStandardItem( category.fInfo ? QStringLiteral( "Yes" ) : QStringLiteral( "" ) );
                items << new QStandardItem( category.fWarning ? QStringLiteral( "Yes" ) : QStringLiteral( "" ) );
                items << new QStandardItem( category.fCritical ? QStringLiteral( "Yes" ) : QStringLiteral( "" ) );

                if ( parent )
                    parent->appendRow( items );
                else
                    fModel->appendRow( items );
                items.front()->setCheckable( true );
                items.front()->setCheckState( Qt::Checked );

                fItems[ fullName ] = items.front();
            }
        }

        void CSetupSystemLoggingDlg::checkItem( QStandardItem *item, bool check )
        {
            return checkItem( item, [ check ]( QStandardItem * ) { return check; } );
        }

        void CSetupSystemLoggingDlg::checkItem( QStandardItem *item, const std::function< bool( QStandardItem *item ) > &function )
        {
            if ( !item )
                return;
            item->setCheckState( function( item ) ? Qt::Checked : Qt::Unchecked );
            for ( int ii = 0; ii < item->rowCount(); ++ii )
            {
                checkItem( item->child( ii ), function );
            }
        }

        void CSetupSystemLoggingDlg::accept()
        {
            auto fileName = logFile();
            if ( !fileName.isEmpty() )
            {
                qputenv( "T42_LOG_FILE", fileName.toLocal8Bit() );
                qputenv( "T42_ENABLE_LOG_FILE", "1" );
                qputenv( "T42_APPEND_TO_LOG", fImpl->appendToLogFile->isChecked() ? "1" : "0" );
                setupSystemLogging();
            }

            QLoggingCategory::setFilterRules( getRules( false ) );
            QLoggingCategory::installFilter( nullptr );
            QDialog::accept();
        }
    }
}