// The MIT License( MIT )
//
// Copyright( c ) 2022 Scott Aron Bloom
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

#include "WidgetChanged.h"
#include "DelayLineEdit.h"

#include <QMetaMethod>
#include <QObject>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QWidget>
#include <QAbstractItemView>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QAbstractButton>
#include <QTextEdit>
#include <QLabel>
#include <QToolButton>
#include <QScrollBar>
#include <QDialogButtonBox>
#ifdef QT_WEBVIEW_LIB
    #include <QtWebEngineWidgets/QWebEngineView>
#endif

#include <QDebug>
#include <unordered_set>

namespace NTowel42Utils
{
    void setupModelChanged( const QAbstractItemModel *model, const QObject *, const std::function< void( QObject * ) > &member, bool isExcluded /* = false*/ )
    {
        if ( !model )
            return;

        if ( !isExcluded )
        {
            QObject::connect( model, &QAbstractItemModel::dataChanged, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
            QObject::connect( model, &QAbstractItemModel::modelReset, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
            QObject::connect( model, &QAbstractItemModel::rowsInserted, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
            QObject::connect( model, &QAbstractItemModel::rowsRemoved, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
            QObject::connect( model, &QAbstractItemModel::rowsMoved, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
            QObject::connect( model, &QAbstractItemModel::columnsInserted, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
            QObject::connect( model, &QAbstractItemModel::columnsRemoved, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
            QObject::connect( model, &QAbstractItemModel::columnsMoved, [ = ]() { member( const_cast< QAbstractItemModel * >( model ) ); } );
        }
    }

    void setupModelChanged( const QAbstractItemModel *model, const QObject *reciever, const char *member, bool isExcluded /* = false*/ )
    {
        if ( !model || !reciever )
            return;

        QObject::disconnect( model, SIGNAL( dataChanged( const QModelIndex &, const QModelIndex & ) ), reciever, member );
        QObject::disconnect( model, SIGNAL( modelReset() ), reciever, member );
        QObject::disconnect( model, SIGNAL( rowsInserted( const QModelIndex &, int, int ) ), reciever, member );
        QObject::disconnect( model, SIGNAL( rowsRemoved( const QModelIndex &, int, int ) ), reciever, member );
        QObject::disconnect( model, SIGNAL( rowsMoved( const QModelIndex &, int, int, const QModelIndex &, int ) ), reciever, member );
        QObject::disconnect( model, SIGNAL( columnsInserted( const QModelIndex &, int, int ) ), reciever, member );
        QObject::disconnect( model, SIGNAL( columnsRemoved( const QModelIndex &, int, int ) ), reciever, member );
        QObject::disconnect( model, SIGNAL( columnsMoved( const QModelIndex &, int, int, const QModelIndex &, int ) ), reciever, member );

        if ( !isExcluded )
        {
            QObject::connect( model, SIGNAL( dataChanged( const QModelIndex &, const QModelIndex & ) ), reciever, member );
            QObject::connect( model, SIGNAL( modelReset() ), reciever, member );
            QObject::connect( model, SIGNAL( rowsInserted( const QModelIndex &, int, int ) ), reciever, member );
            QObject::connect( model, SIGNAL( rowsRemoved( const QModelIndex &, int, int ) ), reciever, member );
            QObject::connect( model, SIGNAL( rowsMoved( const QModelIndex &, int, int, const QModelIndex &, int ) ), reciever, member );
            QObject::connect( model, SIGNAL( columnsInserted( const QModelIndex &, int, int ) ), reciever, member );
            QObject::connect( model, SIGNAL( columnsRemoved( const QModelIndex &, int, int ) ), reciever, member );
            QObject::connect( model, SIGNAL( columnsMoved( const QModelIndex &, int, int, const QModelIndex &, int ) ), reciever, member );
        }
    }

    void setupModelChanged( const QAbstractItemModel *model, const QObject *reciever, const QMetaMethod &member, bool isExcluded /*= false*/ )
    {
        Q_ASSERT_X( member.isValid(), "setupModelChanged", "Member is invalid" );
        if ( !model || !reciever || !member.isValid() )
            return;

        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::dataChanged ), reciever, member );
        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::modelReset ), reciever, member );
        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::rowsInserted ), reciever, member );
        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::rowsRemoved ), reciever, member );
        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::rowsMoved ), reciever, member );
        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::columnsInserted ), reciever, member );
        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::columnsRemoved ), reciever, member );
        QObject::disconnect( model, QMetaMethod::fromSignal( &QAbstractItemModel::columnsMoved ), reciever, member );

        if ( !isExcluded )
        {
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::dataChanged ), reciever, member );
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::modelReset ), reciever, member );
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::rowsInserted ), reciever, member );
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::rowsRemoved ), reciever, member );
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::rowsMoved ), reciever, member );
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::columnsInserted ), reciever, member );
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::columnsRemoved ), reciever, member );
            QObject::connect( model, QMetaMethod::fromSignal( &QAbstractItemModel::columnsMoved ), reciever, member );
        }
    }

    template< typename T >
    void setupWidgetChanged( QAbstractItemView *view, std::unordered_map< QObject *, bool > *handled, const QWidget *parentWidget, T member, bool isExcluded )
    {
        QAbstractItemModel *model = view->model();
        if ( !model )
            return;
        QSortFilterProxyModel *sortModel = dynamic_cast< QSortFilterProxyModel * >( model );
        if ( sortModel )
            model = sortModel->sourceModel();

        if ( handled )
        {
            if ( handled->find( model ) != handled->end() )
                return;
            handled->insert( { model, false } );
        }

        setupModelChanged( model, parentWidget, member, isExcluded );
    }

    void setupWidgetChanged( QGroupBox *groupBox, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        QObject::disconnect( groupBox, SIGNAL( clicked() ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( groupBox, SIGNAL( clicked() ), parentWidget, member );
    }

    void setupWidgetChanged( QGroupBox *groupBox, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        QObject::disconnect( groupBox, QMetaMethod::fromSignal( &QGroupBox::clicked ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( groupBox, QMetaMethod::fromSignal( &QGroupBox::clicked ), parentWidget, member );
    }

    void setupWidgetChanged( QGroupBox *groupBox, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
            QObject::connect( groupBox, &QGroupBox::clicked, [ = ]() { member( groupBox ); } );
    }

    void setupWidgetChanged( QComboBox *comboBox, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        QObject::disconnect( comboBox, SIGNAL( currentIndexChanged( int ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( comboBox, SIGNAL( currentIndexChanged( int ) ), parentWidget, member );
    }

    void setupWidgetChanged( QComboBox *comboBox, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        QObject::disconnect( comboBox, QMetaMethod::fromSignal( qOverload< int >( &QComboBox::currentIndexChanged ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( comboBox, QMetaMethod::fromSignal( qOverload< int >( &QComboBox::currentIndexChanged ) ), parentWidget, member );
    }

    void setupWidgetChanged( QComboBox *comboBox, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
            QObject::connect( comboBox, &QComboBox::currentIndexChanged, [ = ]() { member( comboBox ); } );
    }

    void setupWidgetChanged( NTowel42Utils::CDelayLineEdit *delayLineEdit, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        if ( delayLineEdit->isReadOnly() )
            return;

        QObject::disconnect( delayLineEdit, SIGNAL( sigTextChangedAfterDelay( const QString & ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( delayLineEdit, SIGNAL( sigTextChangedAfterDelay( const QString & ) ), parentWidget, member );
    }

    void setupWidgetChanged( NTowel42Utils::CDelayLineEdit *delayLineEdit, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( delayLineEdit->isReadOnly() )
            return;

        if ( !isExcluded )
            QObject::connect( delayLineEdit, &NTowel42Utils::CDelayLineEdit::sigTextChangedAfterDelay, [ = ]() { member( delayLineEdit ); } );
    }

    void setupWidgetChanged( NTowel42Utils::CDelayLineEdit *delayLineEdit, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        if ( delayLineEdit->isReadOnly() )
            return;

        QObject::disconnect( delayLineEdit, QMetaMethod::fromSignal( &NTowel42Utils::CDelayLineEdit::sigTextChangedAfterDelay ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( delayLineEdit, QMetaMethod::fromSignal( &NTowel42Utils::CDelayLineEdit::sigTextChangedAfterDelay ), parentWidget, member );
    }

    void setupWidgetChanged( QLineEdit *lineEdit, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        if ( lineEdit->isReadOnly() )
            return;

        QObject::disconnect( lineEdit, SIGNAL( textChanged( const QString & ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( lineEdit, SIGNAL( textChanged( const QString & ) ), parentWidget, member );
    }

    void setupWidgetChanged( QLineEdit *lineEdit, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( lineEdit->isReadOnly() )
            return;

        if ( !isExcluded )
            QObject::connect( lineEdit, &QLineEdit::textChanged, [ = ]() { member( lineEdit ); } );
    }

    void setupWidgetChanged( QLineEdit *lineEdit, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        if ( lineEdit->isReadOnly() )
            return;

        QObject::disconnect( lineEdit, QMetaMethod::fromSignal( &QLineEdit::textChanged ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( lineEdit, QMetaMethod::fromSignal( &QLineEdit::textChanged ), parentWidget, member );
    }

    void setupWidgetChanged( QPlainTextEdit *plainTextEdit, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        if ( plainTextEdit->isReadOnly() )
            return;

        QObject::disconnect( plainTextEdit, SIGNAL( textChanged() ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( plainTextEdit, SIGNAL( textChanged() ), parentWidget, member );
    }

    void setupWidgetChanged( QPlainTextEdit *plainTextEdit, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        if ( plainTextEdit->isReadOnly() )
            return;

        QObject::disconnect( plainTextEdit, QMetaMethod::fromSignal( &QPlainTextEdit::textChanged ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( plainTextEdit, QMetaMethod::fromSignal( &QPlainTextEdit::textChanged ), parentWidget, member );
    }

    void setupWidgetChanged( QPlainTextEdit *plainTextEdit, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( plainTextEdit->isReadOnly() )
            return;

        if ( !isExcluded )
            QObject::connect( plainTextEdit, &QPlainTextEdit::textChanged, [ = ]() { member( plainTextEdit ); } );
    }

    void setupWidgetChanged( QTextEdit *textEdit, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        if ( textEdit->isReadOnly() )
            return;

        QObject::disconnect( textEdit, SIGNAL( textChanged() ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( textEdit, SIGNAL( textChanged() ), parentWidget, member );
    }

    void setupWidgetChanged( QTextEdit *textEdit, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        if ( textEdit->isReadOnly() )
            return;

        QObject::disconnect( textEdit, QMetaMethod::fromSignal( &QTextEdit::textChanged ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( textEdit, QMetaMethod::fromSignal( &QTextEdit::textChanged ), parentWidget, member );
    }

    void setupWidgetChanged( QTextEdit *textEdit, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( textEdit->isReadOnly() )
            return;

        if ( !isExcluded )
            QObject::connect( textEdit, &QTextEdit::textChanged, [ = ]() { member( textEdit ); } );
    }

    void setupWidgetChanged( QSpinBox *spinBox, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        QObject::disconnect( spinBox, SIGNAL( valueChanged( const QString & ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( spinBox, SIGNAL( valueChanged( const QString & ) ), parentWidget, member );
    }

    void setupWidgetChanged( QSpinBox *spinBox, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        QObject::disconnect( spinBox, QMetaMethod::fromSignal( qOverload< int >( &QSpinBox::valueChanged ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( spinBox, QMetaMethod::fromSignal( qOverload< int >( &QSpinBox::valueChanged ) ), parentWidget, member );
    }

    void setupWidgetChanged( QSpinBox *spinBox, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
            QObject::connect( spinBox, &QSpinBox::valueChanged, [ = ]() { member( spinBox ); } );
    }

    void setupWidgetChanged( QDoubleSpinBox *doubleSpinBox, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        QObject::disconnect( doubleSpinBox, SIGNAL( valueChanged( const QString & ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( doubleSpinBox, SIGNAL( valueChanged( const QString & ) ), parentWidget, member );
    }

    void setupWidgetChanged( QDoubleSpinBox *doubleSpinBox, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        QObject::disconnect( doubleSpinBox, QMetaMethod::fromSignal( qOverload< double >( &QDoubleSpinBox::valueChanged ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( doubleSpinBox, QMetaMethod::fromSignal( qOverload< double >( &QDoubleSpinBox::valueChanged ) ), parentWidget, member );
    }

    void setupWidgetChanged( QDoubleSpinBox *doubleSpinBox, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
            QObject::connect( doubleSpinBox, &QDoubleSpinBox::valueChanged, [ = ]() { member( doubleSpinBox ); } );
    }

    void setupWidgetChanged( QAbstractButton *button, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        // Q_ASSERT( checkBox || radioButton );
        QObject::disconnect( button, SIGNAL( clicked() ), parentWidget, member );
        QObject::disconnect( button, SIGNAL( toggled( bool ) ), parentWidget, member );
        if ( !isExcluded )
        {
            QObject::connect( button, SIGNAL( clicked() ), parentWidget, member );
            QObject::connect( button, SIGNAL( toggled( bool ) ), parentWidget, member );
        }
        auto checkBox = dynamic_cast< QCheckBox * >( button );
        if ( checkBox )
        {
            QObject::disconnect( checkBox, SIGNAL( stateChanged( int ) ), parentWidget, member );
            if ( !isExcluded )
                QObject::connect( checkBox, SIGNAL( stateChanged( int ) ), parentWidget, member );
        }
    }

    void setupWidgetChanged( QAbstractButton *button, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        // Q_ASSERT( checkBox || radioButton );
        QObject::disconnect( button, QMetaMethod::fromSignal( &QAbstractButton::clicked ), parentWidget, member );
        QObject::disconnect( button, QMetaMethod::fromSignal( &QAbstractButton::toggled ), parentWidget, member );
        if ( !isExcluded )
        {
            QObject::connect( button, QMetaMethod::fromSignal( &QAbstractButton::clicked ), parentWidget, member );
            QObject::connect( button, QMetaMethod::fromSignal( &QAbstractButton::toggled ), parentWidget, member );
        }
        auto checkBox = dynamic_cast< QCheckBox * >( button );
        if ( button->isCheckable() || checkBox )
        {
            QObject::disconnect( checkBox, QMetaMethod::fromSignal( &QCheckBox::checkStateChanged ), parentWidget, member );
            if ( !isExcluded )
                QObject::connect( checkBox, QMetaMethod::fromSignal( &QCheckBox::checkStateChanged ), parentWidget, member );
        }
    }

    void setupWidgetChanged( QAbstractButton *button, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
        {
            QObject::connect( button, &QAbstractButton::clicked, [ = ]() { member( button ); } );
            QObject::connect( button, &QAbstractButton::toggled, [ = ]() { member( button ); } );
        }

        auto checkBox = dynamic_cast< QCheckBox * >( button );
        if ( button->isCheckable() || checkBox )
        {
            if ( !isExcluded )
                QObject::connect( checkBox, &QCheckBox::checkStateChanged, [ = ]() { member( checkBox ); } );
        }
    }

    void setupWidgetChanged( QTimeEdit *timeEdit, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        QObject::disconnect( timeEdit, SIGNAL( timeChanged( const QTime & ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( timeEdit, SIGNAL( timeChanged( const QTime & ) ), parentWidget, member );
    }

    void setupWidgetChanged( QTimeEdit *timeEdit, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        QObject::disconnect( timeEdit, QMetaMethod::fromSignal( &QTimeEdit::timeChanged ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( timeEdit, QMetaMethod::fromSignal( &QTimeEdit::timeChanged ), parentWidget, member );
    }

    void setupWidgetChanged( QTimeEdit *timeEdit, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
            QObject::connect( timeEdit, &QTimeEdit::timeChanged, [ = ]() { member( timeEdit ); } );
    }

    void setupWidgetChanged( QDateEdit *dateEdit, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        QObject::disconnect( dateEdit, SIGNAL( dateChanged( const QDate & ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( dateEdit, SIGNAL( dateChanged( const QDate & ) ), parentWidget, member );
    }

    void setupWidgetChanged( QDateEdit *dateEdit, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        QObject::disconnect( dateEdit, QMetaMethod::fromSignal( &QDateEdit::dateChanged ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( dateEdit, QMetaMethod::fromSignal( &QDateEdit::dateChanged ), parentWidget, member );
    }

    void setupWidgetChanged( QDateEdit *dateEdit, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
            QObject::connect( dateEdit, &QDateEdit::dateChanged, [ = ]() { member( dateEdit ); } );
    }

    void setupWidgetChanged( QDateTimeEdit *dateTimeEdit, const QWidget *parentWidget, const char *member, bool isExcluded )
    {
        QObject::disconnect( dateTimeEdit, SIGNAL( dateTimeChanged( const QDateTime & ) ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( dateTimeEdit, SIGNAL( dateTimeChanged( const QDateTime & ) ), parentWidget, member );
    }

    void setupWidgetChanged( QDateTimeEdit *dateTimeEdit, const QWidget *parentWidget, const QMetaMethod &member, bool isExcluded )
    {
        QObject::disconnect( dateTimeEdit, QMetaMethod::fromSignal( &QDateTimeEdit::dateTimeChanged ), parentWidget, member );
        if ( !isExcluded )
            QObject::connect( dateTimeEdit, QMetaMethod::fromSignal( &QDateTimeEdit::dateTimeChanged ), parentWidget, member );
    }

    void setupWidgetChanged( QDateTimeEdit *dateTimeEdit, const std::function< void( QObject * ) > &member, bool isExcluded )
    {
        if ( !isExcluded )
            QObject::connect( dateTimeEdit, &QDateTimeEdit::dateTimeChanged, [ = ]() { member( dateTimeEdit ); } );
    }

    bool excludeWidget( bool excludeAll, const std::set< QWidget * > &excludedWidgets, QWidget *widget, const QWidget *parentWidget, bool &isSkipWidget, std::unordered_map< QObject *, bool > *handled )
    {
        if ( handled )
        {
            auto pos = handled->find( widget );
            if ( pos != handled->end() )
                return ( *pos ).second;
            handled->insert( { widget, false } );
        }

        QString className = widget->metaObject()->className();
        // qDebug() << "Testing-" << widget << "-" << className << widget->objectName();

        QAbstractButton *button = dynamic_cast< QAbstractButton * >( widget );
        auto checkBox = dynamic_cast< QCheckBox * >( widget );
        auto radioButton = dynamic_cast< QRadioButton * >( widget );
        QLabel *label = dynamic_cast< QLabel * >( widget );
        QToolButton *tb = dynamic_cast< QToolButton * >( widget );
        QDialogButtonBox *dbb = dynamic_cast< QDialogButtonBox * >( widget );
        QScrollBar *scrollbar = dynamic_cast< QScrollBar * >( widget );
#ifdef QT_WEBVIEW_LIB
        auto webview = dynamic_cast< QWebEngineView * >( widget );
#else
        QWidget *webview = nullptr;
#endif
        if ( label || tb || dbb || ( button && !checkBox && !radioButton ) || scrollbar || webview || ( className == "QColumnViewGrip" ) || ( className == "QFrame" ) || ( className == "QTableCornerButton" ) || ( className == "QWidget" ) || ( className == "QSplitter" ) || ( className == "QRubberBand" ) || ( className == "QSplitterHandle" ) || ( className == "QToolBar" ) || ( className == "QToolBarSeparator" ) || ( className == "QMenu" ) || ( className == "QMenuBar" ) || ( className == "QDockWidget" ) || ( className == "QStackedWidget" ) || ( className == "QChartView" ) || ( widget->objectName() == QString( "qt_scrollarea_viewport" ) ) || ( widget->objectName() == QString( "qt_scrollarea_hcontainer" ) ) || ( widget->objectName() == QString( "qt_scrollarea_vcontainer" ) ) || ( className.indexOf( "private", 0, Qt::CaseInsensitive ) != -1 ) )
        {
            isSkipWidget = true;
            // qDebug() << "|---> Skipped Widget" << widget << "-" << widget->metaObject()->className() << widget->objectName();
            if ( handled )
                ( *handled )[ widget ] = true;
            return true;
        }

        QWidget *parent = widget->parentWidget();
        bool isExcluded = excludeAll || ( excludedWidgets.find( widget ) != excludedWidgets.end() );
        if ( !isExcluded && parent && ( parent != parentWidget ) )
            return excludeWidget( excludeAll, excludedWidgets, parent, parentWidget, isSkipWidget, handled );
        // if ( isExcluded )
        //     qDebug() << "|---> Excluded Widget" << widget << "-" << widget->metaObject()->className() << widget->objectName();
        // else
        //     qDebug() << "|---> NOT Excluded Widget" << widget << "-" << widget->metaObject()->className() << widget->objectName();
        if ( handled )
        {
            ( *handled )[ widget ] = isExcluded;
        }
        return isExcluded;
    }

    void setupWidgetChanged( const QWidget *parentWidget, QWidget *child, const char *member, const std::set< QWidget * > &excludedWidgets, bool excludeAll, std::unordered_map< QObject *, bool > *handled )
    {
        bool isSkipWidget = false;
        bool isExcluded = excludeWidget( excludeAll, excludedWidgets, child, parentWidget, isSkipWidget, handled );

        auto view = dynamic_cast< QAbstractItemView * >( child );
        auto groupBox = dynamic_cast< QGroupBox * >( child );
        auto comboBox = dynamic_cast< QComboBox * >( child );
        auto lineEdit = dynamic_cast< QLineEdit * >( child );
        auto delayLineEdit = dynamic_cast< NTowel42Utils::CDelayLineEdit * >( child );
        auto spinBox = dynamic_cast< QSpinBox * >( child );
        auto doubleSpinBox = dynamic_cast< QDoubleSpinBox * >( child );
        auto timeEdit = dynamic_cast< QTimeEdit * >( child );
        auto dateEdit = dynamic_cast< QDateEdit * >( child );
        auto dateTimeEdit = dynamic_cast< QDateTimeEdit * >( child );
        auto checkBox = dynamic_cast< QCheckBox * >( child );
        auto plainTextEdit = dynamic_cast< QPlainTextEdit * >( child );
        auto radioButton = dynamic_cast< QRadioButton * >( child );
        auto button = dynamic_cast< QAbstractButton * >( child );
        auto textEdit = dynamic_cast< QTextEdit * >( child );

        if ( view )
        {
            setupWidgetChanged( view, handled, parentWidget, member, isExcluded );
        }
        else if ( groupBox )
        {
            setupWidgetChanged( groupBox, parentWidget, member, isExcluded );
        }
        else if ( comboBox )
        {
            setupWidgetChanged( comboBox, parentWidget, member, isExcluded );
        }
        else if ( delayLineEdit )
        {
            setupWidgetChanged( delayLineEdit, parentWidget, member, isExcluded );
        }
        else if ( lineEdit )
        {
            setupWidgetChanged( lineEdit, parentWidget, member, isExcluded );
        }
        else if ( plainTextEdit )
        {
            setupWidgetChanged( plainTextEdit, parentWidget, member, isExcluded );
        }
        else if ( textEdit )
        {
            setupWidgetChanged( textEdit, parentWidget, member, isExcluded );
        }
        else if ( spinBox )
        {
            setupWidgetChanged( spinBox, parentWidget, member, isExcluded );
        }
        else if ( doubleSpinBox )
        {
            setupWidgetChanged( doubleSpinBox, parentWidget, member, isExcluded );
        }
        else if ( button )
        {
            setupWidgetChanged( button, parentWidget, member, isExcluded );
        }
        else if ( checkBox )
        {
            setupWidgetChanged( checkBox, parentWidget, member, isExcluded );
        }
        else if ( radioButton )
        {
            setupWidgetChanged( radioButton, parentWidget, member, isExcluded );
        }
        else if ( timeEdit )
        {
            setupWidgetChanged( timeEdit, parentWidget, member, isExcluded );
        }
        else if ( dateEdit )
        {
            setupWidgetChanged( dateEdit, parentWidget, member, isExcluded );
        }
        else if ( dateTimeEdit )
        {
            setupWidgetChanged( dateTimeEdit, parentWidget, member, isExcluded );
        }
#ifndef QT_NO_NDEBUG
        else if ( isSkipWidget )
            return;
        else
        {
            qDebug() << "UNHANDLED-" << child << "-" << child->metaObject()->className() << child->objectName();
        }
#endif
    }

    void setupWidgetChanged( const QWidget *parentWidget, const char *member, const std::set< QWidget * > &excludedWidgets /*= {}*/, bool excludeAll /*= false*/ )
    {
        if ( !parentWidget )
            return;

        std::unordered_map< QObject *, bool > handled;
        QList< QWidget * > children = parentWidget->findChildren< QWidget * >();
        for ( QWidget *child : children )
        {
            setupWidgetChanged( parentWidget, child, member, excludedWidgets, excludeAll, &handled );
        }
    }

    void setupWidgetChanged( const QWidget *parentWidget, QWidget *child, const QMetaMethod &member, const std::set< QWidget * > &excludeWidgets /*= {}*/, bool excludeAll /*= false*/, std::unordered_map< QObject *, bool > *handled )
    {
        bool isSkipWidget = false;
        bool isExcluded = excludeWidget( excludeAll, excludeWidgets, child, parentWidget, isSkipWidget, handled );

        auto view = dynamic_cast< QAbstractItemView * >( child );
        auto groupBox = dynamic_cast< QGroupBox * >( child );
        auto comboBox = dynamic_cast< QComboBox * >( child );
        auto lineEdit = dynamic_cast< QLineEdit * >( child );
        auto delayLineEdit = dynamic_cast< NTowel42Utils::CDelayLineEdit * >( child );
        auto spinBox = dynamic_cast< QSpinBox * >( child );
        auto doubleSpinBox = dynamic_cast< QDoubleSpinBox * >( child );
        auto timeEdit = dynamic_cast< QTimeEdit * >( child );
        auto dateEdit = dynamic_cast< QDateEdit * >( child );
        auto dateTimeEdit = dynamic_cast< QDateTimeEdit * >( child );
        auto checkBox = dynamic_cast< QCheckBox * >( child );
        auto plainTextEdit = dynamic_cast< QPlainTextEdit * >( child );
        auto radioButton = dynamic_cast< QRadioButton * >( child );
        auto button = dynamic_cast< QAbstractButton * >( child );
        auto textEdit = dynamic_cast< QTextEdit * >( child );

        if ( view )
        {
            setupWidgetChanged( view, handled, parentWidget, member, isExcluded );
        }
        else if ( groupBox )
        {
            setupWidgetChanged( groupBox, parentWidget, member, isExcluded );
        }
        else if ( comboBox )
        {
            setupWidgetChanged( comboBox, parentWidget, member, isExcluded );
        }
        else if ( delayLineEdit )
        {
            setupWidgetChanged( delayLineEdit, parentWidget, member, isExcluded );
        }
        else if ( lineEdit )
        {
            setupWidgetChanged( lineEdit, parentWidget, member, isExcluded );
        }
        else if ( plainTextEdit )
        {
            setupWidgetChanged( plainTextEdit, parentWidget, member, isExcluded );
        }
        else if ( textEdit )
        {
            setupWidgetChanged( textEdit, parentWidget, member, isExcluded );
        }
        else if ( spinBox )
        {
            setupWidgetChanged( spinBox, parentWidget, member, isExcluded );
        }
        else if ( doubleSpinBox )
        {
            setupWidgetChanged( doubleSpinBox, parentWidget, member, isExcluded );
        }
        else if ( button )
        {
            setupWidgetChanged( button, parentWidget, member, isExcluded );
        }
        else if ( checkBox )
        {
            setupWidgetChanged( checkBox, parentWidget, member, isExcluded );
        }
        else if ( radioButton )
        {
            setupWidgetChanged( radioButton, parentWidget, member, isExcluded );
        }
        else if ( timeEdit )
        {
            setupWidgetChanged( timeEdit, parentWidget, member, isExcluded );
        }
        else if ( dateEdit )
        {
            setupWidgetChanged( dateEdit, parentWidget, member, isExcluded );
        }
        else if ( dateTimeEdit )
        {
            setupWidgetChanged( dateTimeEdit, parentWidget, member, isExcluded );
        }
#ifndef QT_NO_NDEBUG
        else if ( isSkipWidget )
            return;
        else
        {
            qDebug() << "UNHANDLED-" << child << "-" << child->metaObject()->className() << child->objectName();
        }
#endif
    }

    void setupWidgetChanged( const QWidget *parentWidget, const QMetaMethod &member, const std::set< QWidget * > &excludedWidgets /*= {}*/, bool excludeAll /*= false*/ )
    {
        if ( !parentWidget || !member.isValid() )
            return;

        std::unordered_map< QObject *, bool > handled;
        auto children = parentWidget->findChildren< QWidget * >();
        for ( QWidget *child : children )
        {
            setupWidgetChanged( parentWidget, child, member, excludedWidgets, excludeAll, &handled );
        }
    }

    void setupWidgetChanged( QWidget *child, const std::function< void( QObject * ) > &member, const std::set< QWidget * > &excludedWidgets /*= {}*/, bool excludeAll /*= false*/, std::unordered_map< QObject *, bool > *handled /*= nullptr*/ )
    {
        if ( !child )
            return;

        auto parentWidget = child->parentWidget();

        bool isSkipWidget = false;
        bool isExcluded = excludeWidget( excludeAll, excludedWidgets, child, parentWidget, isSkipWidget, handled );

        auto view = dynamic_cast< QAbstractItemView * >( child );
        auto groupBox = dynamic_cast< QGroupBox * >( child );
        auto comboBox = dynamic_cast< QComboBox * >( child );
        auto lineEdit = dynamic_cast< QLineEdit * >( child );
        auto delayLineEdit = dynamic_cast< NTowel42Utils::CDelayLineEdit * >( child );
        auto spinBox = dynamic_cast< QSpinBox * >( child );
        auto doubleSpinBox = dynamic_cast< QDoubleSpinBox * >( child );
        auto timeEdit = dynamic_cast< QTimeEdit * >( child );
        auto dateEdit = dynamic_cast< QDateEdit * >( child );
        auto dateTimeEdit = dynamic_cast< QDateTimeEdit * >( child );
        auto checkBox = dynamic_cast< QCheckBox * >( child );
        auto plainTextEdit = dynamic_cast< QPlainTextEdit * >( child );
        auto radioButton = dynamic_cast< QRadioButton * >( child );
        auto button = dynamic_cast< QAbstractButton * >( child );
        auto textEdit = dynamic_cast< QTextEdit * >( child );

        if ( view )
        {
            setupWidgetChanged( view, handled, nullptr, member, isExcluded );
        }
        else if ( groupBox )
        {
            setupWidgetChanged( groupBox, member, isExcluded );
        }
        else if ( comboBox )
        {
            setupWidgetChanged( comboBox, member, isExcluded );
        }
        else if ( delayLineEdit )
        {
            setupWidgetChanged( delayLineEdit, member, isExcluded );
        }
        else if ( lineEdit )
        {
            setupWidgetChanged( lineEdit, member, isExcluded );
        }
        else if ( plainTextEdit )
        {
            setupWidgetChanged( plainTextEdit, member, isExcluded );
        }
        else if ( textEdit )
        {
            setupWidgetChanged( textEdit, member, isExcluded );
        }
        else if ( spinBox )
        {
            setupWidgetChanged( spinBox, member, isExcluded );
        }
        else if ( doubleSpinBox )
        {
            setupWidgetChanged( doubleSpinBox, member, isExcluded );
        }
        else if ( button )
        {
            setupWidgetChanged( button, member, isExcluded );
        }
        else if ( checkBox )
        {
            setupWidgetChanged( checkBox, member, isExcluded );
        }
        else if ( radioButton )
        {
            setupWidgetChanged( radioButton, member, isExcluded );
        }
        else if ( timeEdit )
        {
            setupWidgetChanged( timeEdit, member, isExcluded );
        }
        else if ( dateEdit )
        {
            setupWidgetChanged( dateEdit, member, isExcluded );
        }
        else if ( dateTimeEdit )
        {
            setupWidgetChanged( dateTimeEdit, member, isExcluded );
        }
#ifndef QT_NO_NDEBUG
        else if ( isSkipWidget )
            return;
        else
        {
            qDebug() << "UNHANDLED-" << child << "-" << child->metaObject()->className() << child->objectName();
        }
#endif
    }
}