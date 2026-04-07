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

#include "ButtonGroupWDescriptiveText.h"
#include "uiUtils.h"
#include "StringUtils.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QSpacerItem>
#include <QTextEdit>
#include <QWizard>

namespace NTowel42Utils
{
    CButtonGroupWDescriptiveText::CButtonGroupWDescriptiveText( QWidget *parent /*= nullptr*/ ) :
        QWidget( parent )
    {
        fButtonText.push_back( toString( eYes ).value() );
        fButtonText.push_back( toString( eNo ).value() );
        fButtonsThatRequireText.push_back( toString( eYes ).value() );

        rebuild();
        connect( this, &QWidget::objectNameChanged, this, &CButtonGroupWDescriptiveText::nameObjects );
    }

    CButtonGroupWDescriptiveText::~CButtonGroupWDescriptiveText()
    {
    }

    void CButtonGroupWDescriptiveText::addWizardDefaultProperty( QWizard *wizard )
    {
        if ( !wizard )
            return;

        wizard->setDefaultProperty( CButtonGroupWDescriptiveText().metaObject()->className(), "value", SIGNAL( sigChanged() ) );
    }

    void CButtonGroupWDescriptiveText::nameObjects()
    {
        if ( objectName().isEmpty() )
            setObjectName( "CButtonGroupWDescriptiveText" );

        auto base = objectName();
        if ( fHorizontalLayout1 )
            fHorizontalLayout1->setObjectName( base + "_horizontalLayout" );
        if ( fButtonGroup )
            fButtonGroup->setObjectName( base + "_buttonGroup" );
        if ( fDescriptiveTextLabel )
            fDescriptiveTextLabel->setObjectName( base + "_labelDesc" );
        if ( fDescriptiveText )
            fDescriptiveText->setObjectName( base + "_text" );
        for ( auto &&ii : fButtons )
        {
            auto buttonName = NTowel42Utils::NStringUtils::textToIdentifier( base + "_" + ii->text(), false );
            ii->setObjectName( buttonName );
        }
    }

    bool CButtonGroupWDescriptiveText::requiredTextMissing() const
    {
        auto id = fButtonGroup->checkedId();
        if ( id == -1 )
            return false;

        auto pos = fIDsThatRequireText.find( id );
        return pos != fIDsThatRequireText.end();
    }

    void CButtonGroupWDescriptiveText::setAcceptRejectCondemn( QTextEdit *pte )
    {
        setCustomButtonList( QStringList() << tr( "Accept" ) << tr( "Reject" ) << tr( "Condemn" ), false );
        fButtonsThatRequireText.clear();
        fButtonsThatRequireText.push_back( tr( "Reject" ) );
        fButtonsThatRequireText.push_back( tr( "Condemn" ) );
        setLongDescriptiveTextEdit( pte, false );
        setPlaceDescriptiveTextOnSeparateLine( pte == nullptr, true );
    }

    void CButtonGroupWDescriptiveText::setupBuddies( QWidget *widget )
    {
        auto labels = widget->findChildren< QLabel * >();
        for ( auto &&label : labels )
        {
            auto buddy = label->buddy();
            auto buttonGroup = qobject_cast< CButtonGroupWDescriptiveText * >( buddy );
            if ( buttonGroup )
            {
                buttonGroup->setLabel( label );
            }
        }
    }

    bool widgetNameIsEmpty( const QString &widgetName )
    {
        return widgetName.isEmpty() || ( widgetName.compare( QStringLiteral( "<EMPTY>" ), Qt::CaseInsensitive ) == 0 ) || ( widgetName.compare( QStringLiteral( "<NA>" ), Qt::CaseInsensitive ) == 0 );
    }

    void CButtonGroupWDescriptiveText::setLongDescriptionWidgetName( const QString &widgetName )
    {
        auto widget = widgetNameIsEmpty( widgetName ) ? nullptr : parentWidget()->findChild< QTextEdit * >( widgetName );
        fLongDescriptiveTextEdit.first = widgetName;
        setAcceptRejectCondemn( widget );
    }

    QString CButtonGroupWDescriptiveText::longDescriptionWidgetName() const
    {
        if ( fLongDescriptiveTextEdit.first.has_value() )
            return fLongDescriptiveTextEdit.first.value();
        return fLongDescriptiveTextEdit.second ? fLongDescriptiveTextEdit.second->objectName() : QString();
    }

    void CButtonGroupWDescriptiveText::setPlaceDescriptiveTextOnSeparateLine( bool separateLine, bool rebuild )
    {
        fPlaceDescriptiveTextOnSeparateLine = separateLine;
        if ( rebuild )
            this->rebuild();
    }
    void CButtonGroupWDescriptiveText::setCustomButtonList( const QStringList &buttonNames, bool rebuild )
    {
        fButtonText = buttonNames;

        if ( fHasNA )
        {
            addNAToButtonList();
        }

        if ( rebuild )
            this->rebuild();
    }

    void CButtonGroupWDescriptiveText::addNAToButtonList()
    {
        if ( !fButtonText.contains( toString( eNA ).value() ) )
            fButtonText.push_back( toString( eNA ).value() );
    }

    void CButtonGroupWDescriptiveText::setLongDescriptiveTextEdit( QTextEdit *pte, bool rebuild )
    {
        fShowDescriptiveText = pte == nullptr;
        fLongDescriptiveTextEdit.second = pte;

        if ( fLongDescriptiveTextEdit.second )
            connect( fLongDescriptiveTextEdit.second, &QTextEdit::textChanged, this, &CButtonGroupWDescriptiveText::slotChanged );

        if ( rebuild )
            this->rebuild();
    }

    void CButtonGroupWDescriptiveText::setHasNA( bool hasNA, bool rebuild )
    {
        fHasNA = hasNA;
        addNAToButtonList();

        if ( rebuild )
            this->rebuild();
    }

    void CButtonGroupWDescriptiveText::setShowDescriptiveText( bool hasText, bool rebuild )
    {
        Q_ASSERT( !fLongDescriptiveTextEdit.second );
        fShowDescriptiveText = hasText;
        if ( rebuild )
            this->rebuild();
    }

    bool CButtonGroupWDescriptiveText::showDescriptiveText() const
    {
        return fShowDescriptiveText;
    }

    void CButtonGroupWDescriptiveText::setLabel( QLabel *label )
    {
        fBuddyLabel = label;
        if ( fBuddyLabel )
            fBuddyLabel->setBuddy( this );
        slotChanged();
    }

    QString CButtonGroupWDescriptiveText::descText() const
    {
        return fLabelDesc.value_or( QString() );
    }

    void CButtonGroupWDescriptiveText::setDescText( const QString &labelText, bool rebuild )
    {
        auto needsRebuild = fLabelDesc != labelText;
        fLabelDesc = labelText;
        if ( labelText.isEmpty() )
            fLabelDesc.reset();

        if ( needsRebuild && rebuild )
            this->rebuild();
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
        if ( value.has_value() )
        {
            auto button = fButtonGroup->button( value.value() );
            if ( button )
            {
                button->setChecked( true );
                fPreviousValue = value;
                slotChanged();
                return true;
            }
        }
        for ( auto ii : fButtons )
            ii->setChecked( false );
        fPreviousValue.reset();
        return false;
    }

    QString CButtonGroupWDescriptiveText::text() const
    {
        if ( fLongDescriptiveTextEdit.second )
        {
            return fLongDescriptiveTextEdit.second->toPlainText();
        }

        Q_ASSERT( fShowDescriptiveText && fDescriptiveText );
        if ( !fShowDescriptiveText || !fDescriptiveText )
            return {};
        return fDescriptiveText->text();
    }

    void CButtonGroupWDescriptiveText::setText( const QString &text )
    {
        if ( fLongDescriptiveTextEdit.second )
        {
            fLongDescriptiveTextEdit.second->setPlainText( text );
        }
        else
        {
            Q_ASSERT( fShowDescriptiveText && fDescriptiveText );
            if ( !fShowDescriptiveText || !fDescriptiveText )
                return;

            fDescriptiveText->setText( text );
        }
    }

    int CButtonGroupWDescriptiveText::propValue() const
    {
        return value().has_value() ? value().value() : -1;
    }

    QStringList CButtonGroupWDescriptiveText::buttonText() const
    {
        return fButtonText;
    }

    void CButtonGroupWDescriptiveText::setButtonsText( const QStringList &text, bool rebuild )
    {
        fButtonText = text;
        if ( fHasNA )
        {
            addNAToButtonList();
        }
        if ( rebuild )
            this->rebuild();
    }

    void CButtonGroupWDescriptiveText::addButton( const QString &text, bool rebuild )
    {
        fButtonText.push_back( text );
        if ( rebuild )
            this->rebuild();
    }

    void CButtonGroupWDescriptiveText::setReadOnly( bool readOnly, bool rebuild )
    {
        fReadOnly = readOnly;
        if ( rebuild )
            this->rebuild();
    }

    std::optional< int > CButtonGroupWDescriptiveText::value() const
    {
        auto value = fButtonGroup->checkedId();
        if ( value == -1 )
            return {};
        return value;
    }

    void CButtonGroupWDescriptiveText::setButtonRequiresText( int id )
    {
        setButtonsThatRequireText( { id } );
    }

    void CButtonGroupWDescriptiveText::setButtonRequiresText( const QString &buttonText )
    {
        setButtonsThatRequireText( { buttonText } );
    }

    void CButtonGroupWDescriptiveText::setButtonsThatRequireText( const std::list< int > &ids )
    {
        if ( !fButtonGroup )
        {
            rebuild();
        }
        fButtonsThatRequireText.clear();
        for ( auto &&id : ids )
        {
            auto button = fButtonGroup->button( id );
            Q_ASSERT( button );
            if ( !button )
                return;
            fButtonsThatRequireText.push_back( button->text() );
        }
        rebuildIDRequiredTextMap();
    }

    void CButtonGroupWDescriptiveText::setButtonsThatRequireText( const QStringList &buttonsText )
    {
        if ( !fButtonGroup )
        {
            rebuild();
        }
        fButtonsThatRequireText = buttonsText;
        rebuildIDRequiredTextMap();
    }

    bool CButtonGroupWDescriptiveText::alwaysRequiresText() const
    {
        return fButtonsThatRequireText == fButtonText;
    }

    void CButtonGroupWDescriptiveText::setAlwaysRequiresText( bool alwaysRequiresText )
    {
        if ( !alwaysRequiresText )
            return;

        fButtonsThatRequireText.clear();
        fIDsThatRequireText.clear();
        for ( auto &&ii : fButtonText )
        {
            fButtonsThatRequireText.push_back( ii );
        }
        slotChanged();
    }

    bool CButtonGroupWDescriptiveText::neverRequiresText() const
    {
        return fButtonsThatRequireText.isEmpty();
    }

    void CButtonGroupWDescriptiveText::setNeverRequiresText( bool neverRequiresText )
    {
        if ( !neverRequiresText )
            return;
        fButtonsThatRequireText.clear();
        fIDsThatRequireText.clear();
        slotChanged();
    }

    QString CButtonGroupWDescriptiveText::textForValue( int value ) const
    {
        if ( value < 0 )
            return {};

        if ( value < fButtonText.count() )
            return fButtonText[ value ];
        return {};
    }

    QString CButtonGroupWDescriptiveText::textForReport( bool includeText ) const
    {
        QString retVal;

        auto val = value();
        if ( val.has_value() )
        {
            retVal += textForValue( val.value() );
        }
        if ( includeText && !text().isEmpty() )
            retVal += " - " + text();
        return retVal;
    }

    bool CButtonGroupWDescriptiveText::aOK() const
    {
        return aOK( nullptr );
    }

    bool CButtonGroupWDescriptiveText::aOK( bool *textMissing ) const
    {
        if ( !isEnabled() )
            return true;

        auto lclTextMissing = text().isEmpty();
        if ( textMissing )
            *textMissing = lclTextMissing;

        auto requiresText = this->requiredTextMissing();
        if ( requiresText )
            return !lclTextMissing;
        return value().has_value();
    }

    void CButtonGroupWDescriptiveText::slotButtonClicked()
    {
        if ( fReadOnly )
        {
            if ( fPreviousValue.has_value() && ( fButtonGroup->checkedId() != fPreviousValue.value() ) )
                setPropValue( fPreviousValue.value() );
        }
        else
            slotChanged();
    }

    void CButtonGroupWDescriptiveText::slotChanged()
    {
        if ( !fBuddyLabel )
            return;

        bool textMissing = false;
        bool aOK = !this->isEnabled() || this->aOK( &textMissing );
        if ( fDescriptiveTextLabel )
            NTowel42Utils::setIsOK( aOK || !textMissing, fDescriptiveTextLabel );
        NTowel42Utils::setIsOK( aOK, fBuddyLabel );
        if ( sender() )
            emit sigChanged();
    }

    std::optional< QString > CButtonGroupWDescriptiveText::toString( EValue value ) const
    {
        if ( value == eYes )
            return tr( "Yes" );
        else if ( value == eNo )
            return tr( "No" );
        else if ( value == eNA )
            return tr( "N/A" );
        else
            return {};
    }

    void CButtonGroupWDescriptiveText::rebuildIDRequiredTextMap()
    {
        fIDsThatRequireText.clear();
        for ( auto &&ii : fButtonGroup->buttons() )
        {
            if ( fButtonsThatRequireText.indexOf( ii->text() ) != -1 )
            {
                auto id = fButtonGroup->id( ii );
                Q_ASSERT( id != -1 );
                fIDsThatRequireText.insert( id );
            }
        }
    }

    void CButtonGroupWDescriptiveText::addButtons( bool addSpacer )
    {
        for ( auto &&ii : fButtonText )
        {
            auto button = new QRadioButton( this );
            button->setText( ii );

            EValue id;
            if ( ii == toString( eYes ) )
                id = eYes;
            else if ( ii == toString( eNo ) )
                id = eNo;
            else if ( ii == toString( eNA ) )
                id = eNA;
            else
                id = static_cast< EValue >( eFirstCustomValue + fButtons.size() );
            fButtonGroup->addButton( button, id );
            fHorizontalLayout1->addWidget( button );
            fButtons.push_back( button );
        }

        rebuildIDRequiredTextMap();

        if ( addSpacer )
            fHorizontalLayout1->addSpacerItem( new QSpacerItem( 40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum ) );
    }

    void CButtonGroupWDescriptiveText::rebuild()
    {
        std::optional< int > currentCheckedId;
        if ( fButtonGroup && ( fButtonGroup->checkedId() != -1 ) )
            currentCheckedId = fButtonGroup->checkedId();

        delete fDescriptiveText;
        delete fDescriptiveTextLabel;
        for ( auto &&ii : fButtons )
            delete ii;
        fButtons.clear();
        delete fButtonGroup;
        delete fHorizontalLayout1;
        delete fHorizontalLayout2;
        delete fVerticalLayout;
        fIDsThatRequireText.clear();

        fVerticalLayout = nullptr;
        fHorizontalLayout1 = nullptr;
        fHorizontalLayout2 = nullptr;
        fButtonGroup = nullptr;
        fDescriptiveTextLabel = nullptr;
        fDescriptiveText = nullptr;

        fHorizontalLayout1 = new QHBoxLayout;
        fHorizontalLayout1->setContentsMargins( 0, 0, 0, 0 );
        fHorizontalLayout1->setObjectName( "fHorizontalLayout1" );
        auto descTextLayout = fHorizontalLayout1;

        if ( fPlaceDescriptiveTextOnSeparateLine )
        {
            fHorizontalLayout2 = new QHBoxLayout;
            fHorizontalLayout2->setContentsMargins( 0, 0, 0, 0 );
            fHorizontalLayout2->setObjectName( "fHorizontalLayout2" );

            fVerticalLayout = new QVBoxLayout( this );
            fVerticalLayout->setContentsMargins( 0, 0, 0, 0 );
            fVerticalLayout->setObjectName( "fVerticalLayout" );

            fVerticalLayout->addLayout( fHorizontalLayout1 );
            fVerticalLayout->addLayout( fHorizontalLayout2 );

            descTextLayout = fHorizontalLayout2;
        }
        else
            setLayout( fHorizontalLayout1 );

        fButtonGroup = new QButtonGroup( this );
        connect( fButtonGroup, &QButtonGroup::buttonClicked, this, &CButtonGroupWDescriptiveText::slotButtonClicked );

        addButtons( fPlaceDescriptiveTextOnSeparateLine || hasLongDescriptiveTextEdit() || !fShowDescriptiveText );

        if ( currentCheckedId.has_value() )
        {
            auto newButton = fButtonGroup->button( currentCheckedId.value() );
            if ( newButton )
                newButton->setChecked( true );
        }

        if ( !hasLongDescriptiveTextEdit() && fShowDescriptiveText )
        {
            if ( fLabelDesc.has_value() )
            {
                fDescriptiveTextLabel = new QLabel( this );
                fDescriptiveTextLabel->setText( fLabelDesc.value() );
                descTextLayout->addWidget( fDescriptiveTextLabel );
            }
            fDescriptiveText = new QLineEdit( this );
            descTextLayout->addWidget( fDescriptiveText );
            connect( fDescriptiveText, &QLineEdit::textChanged, this, &CButtonGroupWDescriptiveText::slotChanged );
        }

        if ( fLongDescriptiveTextEdit.second )
            fLongDescriptiveTextEdit.second->setReadOnly( fReadOnly );
        if ( fDescriptiveText )
            fDescriptiveText->setReadOnly( fReadOnly );

        nameObjects();
    }

    bool CButtonGroupWDescriptiveText::hasLongDescriptiveTextEdit()
    {
        return fLongDescriptiveTextEdit.second || !widgetNameIsEmpty( fLongDescriptiveTextEdit.first.value_or( {} ) );
    }

}
