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

#include "setReadOnly.h"
#include "ButtonGroupWDescriptiveText.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QTextEdit>
#include <QTimeEdit>
#include <QWidget>
#include <QTabWidget>
#include <QLabel>

namespace NTowel42Utils
{
    void setReadOnlyInternal( QWidget *parentWidget, QWidget *childWidget, bool readOnly )
    {
        auto idx = childWidget->metaObject()->indexOfProperty( "readOnly" );
        if ( idx != -1 )
        {
            childWidget->setProperty( "readOnly", readOnly );
            childWidget->setCursor( readOnly ? Qt::ForbiddenCursor : Qt::ArrowCursor );
            return;
        }

        auto groupBox = dynamic_cast< QGroupBox * >( childWidget );
        auto button = dynamic_cast< QAbstractButton * >( childWidget );
        auto tabWidget = dynamic_cast< QTabWidget * >( childWidget );
        auto label = dynamic_cast< QLabel * >( childWidget );
        auto comboBox = dynamic_cast< QComboBox * >( childWidget );
        auto itemView = dynamic_cast< QAbstractItemView * >( childWidget );

        if ( label )
        {
            return;
        }
        else if ( groupBox )
        {
            if ( groupBox->isCheckable() )
                qWarning() << "Use CSetReadOnlyGroupBox" << groupBox;
            setReadOnly( groupBox, readOnly );
        }
        else if ( button )
        {
            if ( !dynamic_cast< QDialogButtonBox * >( parentWidget ) )
            {
                if ( button->cursor() == Qt::ArrowCursor )
                    button->setDisabled( readOnly );
            }
        }
        else if ( tabWidget )
        {
            for ( auto ii = 0; ii < tabWidget->count(); ++ii )
            {
                setReadOnly( tabWidget->widget( ii ), readOnly );
            }
        }
        else if ( comboBox )
        {
            qWarning() << "Use CSetReadOnlyComboBox" << comboBox;
            if ( comboBox->lineEdit() )
            {
                comboBox->lineEdit()->setReadOnly( true );
                comboBox->lineEdit()->setCursor( readOnly ? Qt::ForbiddenCursor : Qt::ArrowCursor );
            }
        }
        else if ( itemView )
        {
            itemView->setSelectionMode( readOnly ? QAbstractItemView::SelectionMode::NoSelection : QAbstractItemView::SelectionMode::SingleSelection );
            itemView->setCursor( readOnly ? Qt::ForbiddenCursor : Qt::ArrowCursor );
        }
        else
        {
            bool isSimpleWidget = childWidget->metaObject()->superClass()->className() == QStringLiteral( "QWidget" );
            isSimpleWidget = isSimpleWidget || childWidget->objectName() == QStringLiteral( "centralwidget" );
            if ( isSimpleWidget )
            {
                setReadOnly( childWidget, readOnly );
                return;
            }
            qDebug() << "SetReadOnly: UNHANDLED-" << childWidget << "-" << childWidget->metaObject()->className() << " - SuperClass: " << childWidget->metaObject()->superClass()->className() << " - " << childWidget->objectName();
            childWidget->setCursor( readOnly ? Qt::ForbiddenCursor : Qt::ArrowCursor );
        }
    }

    void setReadOnly( QWidget *parentWidget, bool readOnly )
    {
        if ( !parentWidget )
            return;

        std::unordered_map< QObject *, bool > handled;
        QList< QWidget * > children = parentWidget->findChildren< QWidget * >( Qt::FindChildOption::FindDirectChildrenOnly );
        for ( QWidget *child : children )
        {
            setReadOnlyInternal( parentWidget, child, readOnly );
        }
    }
}
