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
#ifndef AVATARHANDLER_H
#define AVATARHANDLER_H

#include <QToolButton>
#include <memory>
#include <QByteArray>
#include <optional>

#include "Towel42UtilsExport.h"

namespace NTowel42Utils
{
    class TOWEL42_UTILS_EXPORT CAvatarHandler : public QToolButton
    {
        Q_OBJECT
        Q_PROPERTY( bool readOnly MEMBER fReadOnly NOTIFY sigReadOnlyChanged );

    public:
        CAvatarHandler( QWidget *parent = nullptr );
        ~CAvatarHandler();

        QByteArray avatarData() const;
        void setAvatarData( const QByteArray &AvatarData );

    Q_SIGNALS:
        void sigReadOnlyChanged();
    public Q_SLOTS:
    private Q_SLOTS:
        void slotSelectAvatar();
        void slotReadOnlyChanged();

    private:
        void setupUi();
        void loadDefaultAvatar();
        QSize computeMinimumSize() const;
        void setAvatarFile( const QString &fileName );
        void setAvatarPixmap( const QPixmap &pixmap );

        std::optional< QByteArray > fAvatarData;
        bool fReadOnly{ false };
    };
}
#endif
