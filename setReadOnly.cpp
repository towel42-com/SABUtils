// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
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

namespace NTowel42Utils
{
    void setReadOnlyInternal( QWidget *parentWidget, QWidget *childWidget, bool readOnly )
    {
        auto groupBox = dynamic_cast< QGroupBox * >( childWidget );
        auto lineEdit = dynamic_cast< QLineEdit * >( childWidget );
        auto spinBox = dynamic_cast< QAbstractSpinBox * >( childWidget );
        auto plainTextEdit = dynamic_cast< QPlainTextEdit * >( childWidget );
        auto button = dynamic_cast< QAbstractButton * >( childWidget );
        auto textEdit = dynamic_cast< QTextEdit * >( childWidget );

        if ( groupBox )
        {
            if ( groupBox->isCheckable() )
                groupBox->setDisabled( readOnly );
            else
                setReadOnly( groupBox, readOnly );
        }
        else if ( lineEdit )
        {
            lineEdit->setReadOnly( readOnly );
        }
        else if ( plainTextEdit )
        {
            plainTextEdit->setReadOnly( readOnly );
        }
        else if ( textEdit )
        {
            textEdit->setReadOnly( readOnly );
        }
        else if ( spinBox )
        {
            spinBox->setReadOnly( true );
        }
        else if ( button )
        {
            if ( !dynamic_cast< QDialogButtonBox * >( parentWidget ) )
                button->setDisabled( readOnly );
        }
        else
        {
            auto idx = childWidget->metaObject()->indexOfProperty( "readOnly" );
            if ( idx == -1 )
            {
                childWidget->setDisabled( readOnly );
                qDebug() << "SetReadOnly: UNHANDLED-" << childWidget << "-" << childWidget->metaObject()->className() << childWidget->objectName();
            }
            else
            {
                childWidget->setProperty( "readOnly", readOnly );
            }
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
