// The MIT License( MIT )
//
// Copyright( c ) 2023 Towel 42 Development, LLC and Scott Aron Bloom
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

#ifndef __LINEEDITWITHSUFFIX_H
#define __LINEEDITWITHSUFFIX_H
#include "Towel42UtilsExport.h"

#include <QLineEdit>
class QLabel;
namespace NTowel42Utils
{
    class TOWEL42_UTILS_EXPORT CLineEditWithSuffix : public QLineEdit
    {
        Q_OBJECT;

        Q_PROPERTY( QString suffix READ suffix WRITE setSuffix )
    public:
        explicit CLineEditWithSuffix( QWidget *parent = nullptr );
        explicit CLineEditWithSuffix( const QString &text, QWidget *parent = nullptr );
        explicit CLineEditWithSuffix( const QString &text, const QString &suffix, QWidget *parent = nullptr );

        virtual ~CLineEditWithSuffix();

        void setSuffix( const QString &suffix );
        QString suffix() const;

        virtual bool event( QEvent *e ) override;

    Q_SIGNALS:
    public Q_SLOTS:
    protected:
        void positionSuffixLabel();
        QLabel *fSuffixLabel{ nullptr };
    };
}
#endif
