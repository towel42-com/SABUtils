#ifndef __SETUPSYSTEMLOGGING_H
#define __SETUPSYSTEMLOGGING_H
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

#ifdef QT_WIDGETS_LIB
    #include "Towel42UtilsExport.h"
    #include <QDialog>
    #include <unordered_map>

class QStandardItemModel;
class QSortFilterProxyModel;
class QStandardItem;

namespace NTowel42Utils
{
    namespace NSystemLogging
    {
        namespace Ui
        {
            class CSetupSystemLoggingDlg;
        }

        struct SCategoryInfo;
        class TOWEL42_UTILS_EXPORT CSetupSystemLoggingDlg : public QDialog
        {
            Q_OBJECT

        public:
            explicit CSetupSystemLoggingDlg( const QString &appName, QWidget *parent = nullptr );
            ~CSetupSystemLoggingDlg();

            QString fullName( QStandardItem *item ) const;
            QString getRules( bool forEnv ) const;
            QString logFile() const;

            virtual void accept() override;

        private Q_SLOTS:
            void slotFilterChanged( const QString &filter );
            void slotLoadCategories();

        private:
            QStringList getRules( QStandardItem *item ) const;
            void loadCategory( const SCategoryInfo &category );

            void checkItem( QStandardItem *item, const std::function< bool( QStandardItem *item ) > &function );
            void checkItem( QStandardItem *item, bool check );

            std::unique_ptr< Ui::CSetupSystemLoggingDlg > fImpl;
            std::unordered_map< QString, QStandardItem * > fItems;
            QStandardItemModel *fModel{ nullptr };
            QSortFilterProxyModel *fFilterModel{ nullptr };
        };
    }
}

#endif
#endif