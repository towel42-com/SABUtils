// The MIT License( MIT )
//
// Copyright( c ) 2020 Scott Aron Bloom
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

#ifndef _SCROLLMESSAGEBOX_H
#define _SCROLLMESSAGEBOX_H

#include <QDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <memory>
class QAbstractButton;

namespace Ui {class CScrollMessageBox;};

class CScrollMessageBox : public QDialog
{
    Q_OBJECT

public:
    CScrollMessageBox( QWidget *parent = 0 );
    CScrollMessageBox( const QString & title, const QString & label, QWidget *parent = 0 );
    ~CScrollMessageBox();

    void setTitle( const QString & title );
    void setLabel( const QString & label );

    void setPlainText( const QString & text );
    void setHtmlText( const QString & text );
    void setIconLabel( const QMessageBox::Icon & icon );
    void setButtons( QDialogButtonBox::StandardButtons buttons );
private slots:
    void slotButtonClicked( QAbstractButton * btn );

private:
    std::unique_ptr< Ui::CScrollMessageBox > fImpl;
};

#endif 