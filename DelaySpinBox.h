#ifndef __DELAYSPINBOX_H
#define __DELAYSPINBOX_H
// The MIT License( MIT )
//
// Copyright( c ) 2021 Scott Aron Bloom
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

#include <QSpinBox>
class QTimer;

class CDelaySpinBox : public QSpinBox
{
    Q_OBJECT;
public:
    explicit CDelaySpinBox( QWidget *parent = nullptr );
    explicit CDelaySpinBox( int delayMS, QWidget *parent = nullptr );
    ~CDelaySpinBox();

    void setDelay( int delayMS );

    void setValue( int val );

Q_SIGNALS:
    void sigValueChangedAfterDelay( int );
    void sigEditingFinishedAfterDelay();

#if QT_VERSION > QT_VERSION_CHECK(5,14,0)
    void sigTextChangedAfterDelay( const QString & );
#endif

public Q_SLOTS:
    void slotValueChanged();
    void slotEditingFinished();
#if QT_VERSION > QT_VERSION_CHECK(5,14,0)
    void slotTextChanged();
#endif

    void slotValueChangedTimerTimeout();
    void slotEditingFinishedTimerTimeout();
#if QT_VERSION > QT_VERSION_CHECK(5,14,0)
    void slotTextChangedTimerTimeout();
#endif

private:
    int fDelayMS{ 500 };
    QTimer *fValueChangedTimer{ nullptr };
    QTimer *fEditFinishedTimer{ nullptr };
#if QT_VERSION > QT_VERSION_CHECK(5,14,0)
    QTimer *fTextChangedTimer{ nullptr };
#endif
};

#endif
