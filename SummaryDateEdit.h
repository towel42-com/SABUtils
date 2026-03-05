// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __SUMMARYDATEEDIT_H
#define __SUMMARYDATEEDIT_H

#include "Towel42UtilsExport.h"

#include <QDateEdit>

namespace NTowel42Utils
{
    enum class ESummaryDateType
    {
        eAnnual,
        eQuarterly,
        eMonthly
    };

    class TOWEL42_UTILS_EXPORT CSummaryDateEdit : public QDateEdit
    {
        Q_OBJECT;

    public:
        explicit CSummaryDateEdit( QWidget *parent = nullptr );
        explicit CSummaryDateEdit( const QDate &date, QWidget *parent = nullptr );
        ~CSummaryDateEdit() = default;

        void setType( ESummaryDateType dateType );

        virtual QString textFromDateTime( const QDateTime &dt ) const override;
        virtual QDateTime dateTimeFromText( const QString &text ) const override;
        virtual void stepBy( int steps ) override;
        virtual QAbstractSpinBox::StepEnabled stepEnabled() const override;
    public Q_SLOTS:
    Q_SIGNALS:

    protected:
    private:
        ESummaryDateType fDateType{ ESummaryDateType::eAnnual };
    };
}
#endif
