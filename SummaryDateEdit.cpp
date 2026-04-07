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

#include "SummaryDateEdit.h"
#include <QDateTime>

namespace NTowel42Utils
{
    CSummaryDateEdit::CSummaryDateEdit( QWidget *parent /*= nullptr */ ) :
        QDateEdit( parent )
    {
        setCalendarPopup( false );
        setSummaryDateType( ESummaryDateType::eAnnual );
    }

    QString CSummaryDateEdit::textFromDateTime( const QDateTime &dt ) const
    {
        if ( fSummaryDateType != ESummaryDateType::eQuarterly )
            return QDateEdit::textFromDateTime( dt );

        auto quarter = 1 + ( ( dt.date().month() - 1 ) / 3 );
        return QStringLiteral( "Q%1 %2" ).arg( quarter ).arg( dt.date().year() );
    }

    QDateTime CSummaryDateEdit::dateTimeFromText( const QString &text ) const
    {
        if ( fSummaryDateType != ESummaryDateType::eQuarterly )
            return QDateEdit::dateTimeFromText( text );

        auto tmp = text.split( " " );
        if ( tmp.length() != 2 )
        {
            return dateTime();
        }

        bool aOK = false;
        auto year = tmp.back().toInt( &aOK );
        if ( !aOK )
            return dateTime();

        aOK = false;
        auto quarter = tmp.front().mid( 1 ).toInt( &aOK );
        if ( !aOK )
            return dateTime();
        auto month = ( quarter - 1 ) * 3 + 1;
        auto retVal = dateTime();
        retVal.setDate( QDate( year, month, 1 ) );
        return retVal;
    }

    void CSummaryDateEdit::stepBy( int steps )
    {
        if ( isReadOnly() )
            return;

        auto currSection = this->currentSection();
        auto curr = this->date();
        if ( currSection == QDateTimeEdit::YearSection )
            curr = date().addYears( steps );
        else if ( currSection == QDateTimeEdit::MonthSection )
            curr = date().addMonths( steps );
        else // if ( currentSection == QDateTimeEdit::NoSection ) is the q1 section
            curr = date().addMonths( steps * 3 );


        //if ( fDateType == ESummaryDateType::eAnnual )
        //    curr = date().addYears( steps );
        //else if ( fDateType == ESummaryDateType::eMonthly )
        //    curr = date().addMonths( steps );
        //else if( fDateType == ESummaryDateType::eQuarterly )
        //    curr = date().addMonths( steps * 3 );
        setDate( curr );
    }

    QAbstractSpinBox::StepEnabled CSummaryDateEdit::stepEnabled() const
    {
        if ( isReadOnly() )
            return StepNone;

        return { StepUpEnabled | StepDownEnabled };
    }

    void CSummaryDateEdit::setSummaryDateType( ESummaryDateType dateType )
    {
        if ( dateType == ESummaryDateType::eAnnual )
        {
            setDisplayFormat( "yyyy" );
        }
        else if ( dateType == ESummaryDateType::eQuarterly )
        {
            setDisplayFormat( "'Q'q yyyy" );
            setCurrentSection( QDateTimeEdit::YearSection );
        }
        else if ( dateType == ESummaryDateType::eMonthly )
        {
            setDisplayFormat( "MMM yyyy" );
        }
        fSummaryDateType = dateType;
    }
}