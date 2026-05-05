#if defined( TOWEL42_GIFSUPPORT ) && defined( TOWEL42_GIFSUPPORT )
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

#ifndef _BIFToGIFWriterDlg_H
#define _BIFToGIFWriterDlg_H

#include "Towel42UtilsExport.h"

class QProgressDialog;
class QFileInfo;

#include <QDialog>
#include <memory>

namespace NTowel42Utils
{
    namespace NBIF
    {
        class CFile;
    }

    namespace Ui
    {
        class CBIFToGIFWriterDlg;
    }
    class TOWEL42_UTILS_EXPORT CBIFToGIFWriterDlg : public QDialog
    {
        Q_OBJECT
    public:
        CBIFToGIFWriterDlg( QWidget *parent = nullptr );
        #ifdef TOWEL42_BIFSUPPORT
        CBIFToGIFWriterDlg::CBIFToGIFWriterDlg( std::shared_ptr< NBIF::CFile > bifFile, QWidget *parent = nullptr );
        CBIFToGIFWriterDlg::CBIFToGIFWriterDlg( std::shared_ptr< NBIF::CFile > bifFile, int delayInMSec, QWidget *parent = nullptr );
        #endif

        ~CBIFToGIFWriterDlg();

        static bool saveToGIF( QWidget *parent, const QString &fileName, const QList< QFileInfo > images, std::size_t startFrame, std::size_t endFrame, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled );
        static bool saveToGIF( QWidget *parent, const QString &fileName, const QList< QFileInfo > images, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled );
        static bool saveToGIF( QWidget *parent, const QString &fileName, const QList< QImage > images, std::size_t startFrame, std::size_t endFrame, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled );
        static bool saveToGIF( QWidget *parent, const QString &fileName, const QList< QImage > images, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled );

        #ifdef TOWEL42_BIFSUPPORT
        void setBIF( std::shared_ptr< NBIF::CFile > bifFile );
        std::shared_ptr< NBIF::CFile > bifFile() const { return fBIF; }
        #endif
        // speed multiplier uses the delay built into the BIF file
        void setSpeedMultipler( int multiplier );   // delay or multiplier (setting the bif as well) override each other, last one set wins
        void setDelay( int msec );
        int delay() const;

        void setDither( bool dither );
        bool dither() const;

        void setFlipImage( bool flipImage );
        bool flipImage() const;

        void setLoopCount( int loopCount );
        int loopCount() const;

        void setStartFrame( std::size_t startFrame );
        std::size_t startFrame() const;

        void setEndFrame( std::size_t endFrame );
        std::size_t endFrame() const;

        virtual void accept() override;
        std::size_t numFramesToSave() const;

        void setUseNew( bool useNew );
        bool useNew() const;
    Q_SIGNALS:
    public Q_SLOTS:
        void slotSelectGIFFile();
        void slotStartFrameChanged();
        void slotEndFrameChanged();
    private Q_SLOTS:
        void slotUpdateFileName();

    private:
        void updateDelay();
        bool saveToGIF();

        int fMultipler{ 1 };

    std::shared_ptr< NBIF::CFile > fBIF;
    std::unique_ptr < Ui::CBIFToGIFWriterDlg> fImpl;
    };
}
    #endif
#endif
