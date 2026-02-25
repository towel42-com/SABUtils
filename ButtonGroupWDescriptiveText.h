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

#ifndef __BUTTONGROUPWDESCRIPTIVETEXT_H
#define __BUTTONGROUPWDESCRIPTIVETEXT_H

#include "Towel42UtilsExport.h"

#include <QWidget>
#include <optional>

class QRadioButton;
class QLineEdit;
class QButtonGroup;
class QLabel;
class QHBoxLayout;
class QTextEdit;

namespace NTowel42Utils
{
    class TOWEL42_UTILS_EXPORT CButtonGroupWDescriptiveText : public QWidget
    {
        Q_OBJECT;
        Q_PROPERTY( int value READ propValue WRITE setPropValue );

    public:
        enum EValue
        {
            eYes = 0,
            eNo = 1,
            eNA = 2
        };

        CButtonGroupWDescriptiveText( QWidget *parent = nullptr );
        ~CButtonGroupWDescriptiveText();

        void setAcceptRejectCondemn( QTextEdit *te );   // short cut to building it all yourself
        void setCustomButtonList( const QStringList &buttonNames, bool rebuild = true );
        void setPlainText( QTextEdit *pte, bool rebuild = true );
        void setHasNA( bool hasNA, bool rebuild = true );
        void setHasText( bool hasText, bool rebuild = true );
        void setLabel( QLabel *label );
        void setDescText( const QString &text, bool rebuild = true );
        bool setValue( int value, const QString &desc );
        std::optional< int > value() const;
        void setNoYesSwapped( bool swapped );
        void setAlwaysRequiresText( bool requiresText );
        void setNeverRequiresText( bool requiresText );
        void addButton( const QString &text, bool rebuild = true );
        bool aOK() const;

        bool setPropValue( std::optional< int > value );
        int propValue() const;
        QString text() const;
        void setText( const QString &text );

        QLabel *label() const { return fLabel; }
        QLineEdit *lineEdit() const { return fText; }
    Q_SIGNALS:
        void sigChanged();

    public Q_SLOTS:
        void slotChanged();

    private:
        bool aOK( bool *textMissing ) const;
        void rebuild();
        void addButtons( const QStringList &buttonText, bool addSpacer );
        void nameObjects();

        bool fHasNA{ false };
        bool fHasText{ true };
        bool fNoYesSwapped{ false };
        bool fAlwaysRequiresText{ false };
        bool fNeverRequiresText{ false };
        std::optional< QString > fLabelDesc;
        QLineEdit *fText{ nullptr };
        QLabel *fLabelForText{ nullptr };

        std::vector< QRadioButton * > fButtons;
        QStringList fBaseButtonText;
        QStringList fExtraButtonText;

        QButtonGroup *fButtonGroup{ nullptr };
        QHBoxLayout *fHorizontalLayout{ nullptr };

        QLabel *fLabel{ nullptr };
        QTextEdit *fTextEdit{ nullptr };
    };
}
#endif
