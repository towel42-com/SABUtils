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

#include "SetReadOnlyComboBox.h"
#include <QLineEdit>

namespace NTowel42Utils
{
    CSetReadOnlyComboBox::CSetReadOnlyComboBox( QWidget *parent /*= nullptr */ ) :
        QComboBox( parent )
    {
    }

    void CSetReadOnlyComboBox::setReadOnly( bool readOnly )
    {
        fReadOnly = readOnly;
        setCursor( readOnly ? Qt::ForbiddenCursor : Qt::ArrowCursor );

        if ( lineEdit() )
            lineEdit()->setReadOnly( readOnly );
        setEditable( !readOnly );
    }

    void CSetReadOnlyComboBox::showPopup()
    {
        if ( fReadOnly )
            return;
        QComboBox::showPopup();
    }

}
