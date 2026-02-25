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

#include "ButtonGroupWDescriptiveText.h"
#include "uiUtils.h"
#include "StringUtils.h"

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QSpacerItem>
#include <QTextEdit>

namespace NTowel42Utils
{
    CButtonGroupWDescriptiveText::CButtonGroupWDescriptiveText( QWidget *parent /*= nullptr*/ ) :
        QWidget( parent )
    {
        fBaseButtonText.push_back( tr( "Yes" ) );
        fBaseButtonText.push_back( tr( "No" ) );

        rebuild();
        connect( this, &QWidget::objectNameChanged, this, &CButtonGroupWDescriptiveText::nameObjects );
    }

    void CButtonGroupWDescriptiveText::nameObjects()
    {
        if ( objectName().isEmpty() )
            setObjectName( "CButtonGroupWDescriptiveText" );

        auto base = objectName();
        if ( fHorizontalLayout )
            fHorizontalLayout->setObjectName( base + "_horizontalLayout" );
        if ( fButtonGroup )
            fButtonGroup->setObjectName( base + "_buttonGroup" );
        if ( fLabelForText )
            fLabelForText->setObjectName( base + "_labelDesc" );
        if ( fText )
            fText->setObjectName( base + "_text" );
        for ( auto &&ii : fButtons )
        {
            auto buttonName = NTowel42Utils::NStringUtils::textToIdentifier( base + "_" + ii->text(), false );
            ii->setObjectName( buttonName );
        }
    }

    void CButtonGroupWDescriptiveText::rebuild()
    {
        delete fHorizontalLayout;
        delete fButtonGroup;
        for ( auto &&ii : fButtons )
            delete ii;
        fButtons.clear();
        delete fLabelForText;

        fHorizontalLayout = new QHBoxLayout( this );
        fHorizontalLayout->setContentsMargins( 0, 0, 0, 0 );

        fButtonGroup = new QButtonGroup( this );

        auto buttonText = fBaseButtonText << fExtraButtonText;

        addButtons( buttonText, fIsARC || !fHasText );

        if ( !fIsARC && fHasText )
        {
            if ( fLabelDesc.has_value() )
            {
                fLabelForText = new QLabel( this );
                fLabelForText->setText( fLabelDesc.value() );
                fHorizontalLayout->addWidget( fLabelForText );
            }
            fText = new QLineEdit( this );
            fHorizontalLayout->addWidget( fText );
            connect( fText, &QLineEdit::textChanged, this, &CButtonGroupWDescriptiveText::slotChanged );
        }
        nameObjects();
    }

    void CButtonGroupWDescriptiveText::addButtons( const QStringList &buttonText, bool addSpacer )
    {
        for ( auto &&ii : buttonText )
        {
            auto button = new QRadioButton( this );
            button->setText( ii );
            fButtonGroup->addButton( button );
            fHorizontalLayout->addWidget( button );
            connect( button, &QRadioButton::clicked, this, &CButtonGroupWDescriptiveText::slotChanged );
            fButtons.push_back( button );
        }

        if ( addSpacer )
            fHorizontalLayout->addSpacerItem( new QSpacerItem( 40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum ) );
    }

    CButtonGroupWDescriptiveText::~CButtonGroupWDescriptiveText()
    {
    }

    void CButtonGroupWDescriptiveText::setAcceptRejectCondemn( bool isARC, QTextEdit *pte )
    {
        fIsARC = isARC;
        fNoYesSwapped = true;
        fTextEdit = pte;
        fBaseButtonText.clear();

        fBaseButtonText.push_back( tr( "Accept" ) );
        fBaseButtonText.push_back( tr( "Reject" ) );
        fBaseButtonText.push_back( tr( "Condemn" ) );

        if ( fHasNA )
            fBaseButtonText.push_back( tr( "N/A" ) );

        connect( fTextEdit, &QTextEdit::textChanged, this, &CButtonGroupWDescriptiveText::slotChanged );

        rebuild();
    }

    void CButtonGroupWDescriptiveText::setHasNA( bool hasNA )
    {
        fHasNA = hasNA;
        fBaseButtonText.push_back( tr( "N/A" ) );

        rebuild();
    }

    void CButtonGroupWDescriptiveText::setHasText( bool hasText )
    {
        fHasText = hasText;
        rebuild();
    }

    void CButtonGroupWDescriptiveText::setLabel( QLabel *label )
    {
        fLabel = label;
        slotChanged();
    }

    void CButtonGroupWDescriptiveText::setDescText( const QString &labelText )
    {
        auto needsRebuild = fLabelDesc != labelText;
        fLabelDesc = labelText;
        if ( labelText.isEmpty() )
            fLabelDesc.reset();

        if ( needsRebuild )
            rebuild();
    }

    bool CButtonGroupWDescriptiveText::setValue( int value, const QString &desc )
    {
        bool aOK = setPropValue( value );
        setText( desc );
        slotChanged();
        return aOK;
    }

    bool CButtonGroupWDescriptiveText::setPropValue( std::optional< int > value )
    {
        if ( value.has_value() && ( ( value.value() >= 0 ) && ( value < fButtons.size() ) ) )
        {
            fButtons[ value.value() ]->setChecked( true );
            return true;
        }
        else
        {
            for ( auto ii : fButtons )
                ii->setChecked( false );
            return false;
        }
    }

    QString CButtonGroupWDescriptiveText::text() const
    {
        if ( fTextEdit )
        {
            return fTextEdit->toPlainText();
        }

        Q_ASSERT( fHasText && fText );
        if ( !fHasText || !fText )
            return {};
        return fText->text();
    }

    void CButtonGroupWDescriptiveText::setText( const QString &text )
    {
        if ( fTextEdit )
        {
            fTextEdit->setPlainText( text );
        }
        else
        {
            Q_ASSERT( fHasText && fText );
            if ( !fHasText || !fText )
                return;

            fText->setText( text );
        }
    }

    int CButtonGroupWDescriptiveText::propValue() const
    {
        return value().has_value() ? value().value() : -1;
    }

    void CButtonGroupWDescriptiveText::setNoYesSwapped( bool swapped )
    {
        fNoYesSwapped = swapped;
        slotChanged();
    }

    void CButtonGroupWDescriptiveText::setAlwaysRequiresText( bool requiresText )
    {
        fAlwaysRequiresText = requiresText;
        slotChanged();
    }

    void CButtonGroupWDescriptiveText::setNeverRequiresText( bool requiresText )
    {
        fNeverRequiresText = requiresText;
        slotChanged();
    }

    void CButtonGroupWDescriptiveText::addButton( const QString &text )
    {
        fExtraButtonText.push_back( text );
        rebuild();
    }

    std::optional< int > CButtonGroupWDescriptiveText::value() const
    {
        for ( int ii = 0; ii < fButtons.size(); ++ii )
        {
            if ( fButtons[ ii ]->isChecked() )
                return ii;
        }
        return {};
    }

    bool CButtonGroupWDescriptiveText::aOK() const
    {
        return aOK( nullptr );
    }
    
    bool CButtonGroupWDescriptiveText::aOK( bool *textMissing ) const
    {
        auto lclTextMissing = text().isEmpty();
        if ( textMissing )
            *textMissing = lclTextMissing;

        auto val = value();
        if ( !val.has_value() )
            return false;


        auto noVal = fNoYesSwapped ? EValue::eYes : EValue::eNo;
        auto yesVal = fNoYesSwapped ? EValue::eNo : EValue::eYes;
        if ( fAlwaysRequiresText )
        {
            return !lclTextMissing;
        }

        if ( fNeverRequiresText )
            return true;

        if ( val == noVal )
            return true;
        if ( !fIsARC && ( val == EValue::eNA ) )
            return true;
        if ( fIsARC || ( val == yesVal ) )
        {
            return !lclTextMissing;
        }
        return false;
    }

    void CButtonGroupWDescriptiveText::slotChanged()
    {
        if ( !fLabel )
            return;

        bool textMissing = false;
        bool aOK = this->aOK( &textMissing );
        if ( fLabelForText )
            NTowel42Utils::setIsOK( aOK || !textMissing, fLabelForText );
        NTowel42Utils::setIsOK( aOK, fLabel );
        emit sigChanged();
    }
}