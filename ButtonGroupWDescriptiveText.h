// The MIT License( MIT )
//
// Copyright( c ) 2026 Towel 42 Development, LLC and Scott Aron Bloom
// SPDX-License-Identifier : MIT License
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
//
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
#include <unordered_set>
#include <unordered_map>

class QRadioButton;
class QLineEdit;
class QButtonGroup;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QTextEdit;
class QWizard;

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
            eNA = 2,
            eFirstCustomValue = 3
        };
        std::optional< QString > toString( EValue value ) const;

        CButtonGroupWDescriptiveText( QWidget *parent = nullptr );
        ~CButtonGroupWDescriptiveText();

        static void addWizardDefaultProperty( QWizard *wizard );
        void setAcceptRejectCondemn( QTextEdit *te );   // short cut to building it all yourself
        void setCustomButtonList( const QStringList &buttonNames, bool rebuild = true );
        void setLongDescriptiveTextEdit( QTextEdit *pte, bool rebuild = true );
        void setHasNA( bool hasNA, bool rebuild = true );
        void setShowDescriptiveText( bool showDescriptiveText, bool rebuild = true );
        void setLabel( QLabel *label );
        void setDescText( const QString &text, bool rebuild = true );
        bool setValue( int value, const QString &desc );
        std::optional< int > value() const;
        void setButtonRequiresText( int id );   // default is the eYes
        void setButtonRequiresText( const QString &buttonText );   // default is the "Yes" Button
        void setButtonsThatRequiresText( const std::list< int > &ids );   // default is the { eYes }
        void setButtonsThatRequiresText( const QStringList &buttonsText );   // default is the { "Yes" } Button

        void setPlaceDescriptiveTextOnSeparateLine( bool separateLine, bool rebuild = true );
        void setAlwaysRequiresText();
        void setNeverRequiresText();

        void addButton( const QString &text, bool rebuild = true );

        void setReadOnly( bool readOnly, bool rebuild = true );
        bool aOK() const;

        bool setPropValue( std::optional< int > value );
        int propValue() const;
        QString text() const;
        void setText( const QString &text );

        QLabel *label() const { return fBuddyLabel; }
        QLineEdit *lineEdit() const { return fDescriptiveText; }
        QLabel *descriptiveTextLabel() const { return fDescriptiveTextLabel; }

        QString textForReport( bool includeText ) const;
        QString textForValue( int value ) const;

    Q_SIGNALS:
        void sigChanged();

    public Q_SLOTS:
        void slotChanged();

    private:
        bool aOK( bool *textMissing ) const;
        void rebuild();
        void addButtons( const QStringList &buttonText, bool addSpacer );

    private:
        void rebuildIDRequiredTextMap();

        void nameObjects();

        bool fReadOnly{ false };
        bool fHasNA{ false };
        bool fShowDescriptiveText{ true };
        bool fPlaceDescriptiveTextOnSeparateLine{ false };

        std::optional< QString > fLabelDesc;
        QStringList fBaseButtonText;
        QStringList fExtraButtonText;

        // owned widgets
        QLineEdit *fDescriptiveText{ nullptr };   //
        QLabel *fDescriptiveTextLabel{ nullptr };
        std::vector< QRadioButton * > fButtons;

        QButtonGroup *fButtonGroup{ nullptr };
        QHBoxLayout *fHorizontalLayout1{ nullptr };
        QHBoxLayout *fHorizontalLayout2{ nullptr };
        QVBoxLayout *fVerticalLayout{ nullptr };

        // NOT OWNED
        QLabel *fBuddyLabel{ nullptr };   // the label outside the widget to change colors
        QTextEdit *fLongDescriptiveTextEdit{ nullptr };   // the TextEdit for more extensive details, unowned

        bool requiredTextMissing() const;
        std::unordered_set< QString > fButtonsThatRequireText;
        std::unordered_set< int > fIDsThatRequireText;
    };
}
#endif
