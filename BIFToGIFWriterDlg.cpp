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
#if defined( TOWEL42_GIFSUPPORT ) && defined( TOWEL42_GIFSUPPORT )

    #include "BIFToGIFWriterDlg.h"
    #include "BIFFile.h"
    #include "GIFWriter.h"
    #include "QtUtils.h"

    #include <QMessageBox>
    #include <QFileDialog>
    #include <QFileInfo>
    #include <QProgressDialog>

    #include "ui_BIFToGIFWriterDlg.h"

namespace NTowel42Utils
{
    CBIFToGIFWriterDlg::CBIFToGIFWriterDlg( std::shared_ptr< NBIF::CFile > bifFile, int delayInMSec, QWidget *parent ) :
        QDialog( parent ),
        fImpl( new Ui::CBIFToGIFWriterDlg )
    {
        Q_INIT_RESOURCE( Towel42Utils );
        Q_INIT_RESOURCE( BIFPlayerResources );
        fImpl->setupUi( this );

        connect( fImpl->selectGIFFile, &QToolButton::clicked, this, &CBIFToGIFWriterDlg::slotSelectGIFFile );
        connect( fImpl->startFrame, qOverload< int >( &QSpinBox::valueChanged ), this, &CBIFToGIFWriterDlg::slotStartFrameChanged );
        connect( fImpl->endFrame, qOverload< int >( &QSpinBox::valueChanged ), this, &CBIFToGIFWriterDlg::slotEndFrameChanged );
        setBIF( bifFile );
        setDelay( delayInMSec );
        setDither( true );
        setFlipImage( false );
        setLoopCount( 0 );

        connect( fImpl->useNew, &QCheckBox::clicked, this, &CBIFToGIFWriterDlg::slotUpdateFileName );
    #ifndef _DEBUG
        fImpl->useNew->setVisible( false );
    #endif
    }

    CBIFToGIFWriterDlg::CBIFToGIFWriterDlg( std::shared_ptr< NBIF::CFile > bifFile, QWidget *parent ) :
        CBIFToGIFWriterDlg( bifFile, 2, parent )
    {
    }

    CBIFToGIFWriterDlg::CBIFToGIFWriterDlg( QWidget *parent ) :
        CBIFToGIFWriterDlg( {}, parent )
    {
    }

    CBIFToGIFWriterDlg::~CBIFToGIFWriterDlg()
    {
    }

    void CBIFToGIFWriterDlg::setBIF( std::shared_ptr< NBIF::CFile > bifFile )
    {
        fBIF = bifFile;
        fImpl->startFrame->setValue( 1 );
        fImpl->startFrame->setMinimum( 1 );
        fImpl->startFrame->setMaximum( 1 );
        fImpl->endFrame->setMinimum( 1 );
        fImpl->endFrame->setValue( 1 );
        fImpl->endFrame->setMaximum( 1 );
        if ( fBIF )
        {
            fImpl->endFrame->setMaximum( (int)fBIF->imageCount() );
            fImpl->endFrame->setValue( (int)fBIF->imageCount() );
            slotUpdateFileName();
            updateDelay();
        }
    }

    void CBIFToGIFWriterDlg::slotUpdateFileName()
    {
        QString fn;
        if ( fBIF )
        {
            auto fi = QFileInfo( fBIF->fileName() );
            fn = QFileInfo( fi.absolutePath() + "/" + fi.completeBaseName() ).absoluteFilePath();
            if ( !useNew() )
                fn += ".old";
            fn += ".gif";
        }

        fImpl->fileName->setText( fn );
    }

    void CBIFToGIFWriterDlg::slotStartFrameChanged()
    {
        fImpl->endFrame->setMinimum( fImpl->startFrame->value() );
        if ( fImpl->startFrame->value() >= fImpl->endFrame->value() )
            fImpl->endFrame->setValue( fImpl->startFrame->value() );
    }

    void CBIFToGIFWriterDlg::slotEndFrameChanged()
    {
        fImpl->startFrame->setMaximum( fImpl->endFrame->value() );
        if ( fImpl->endFrame->value() <= fImpl->startFrame->value() )
            fImpl->endFrame->setValue( fImpl->startFrame->value() );
    }

    void CBIFToGIFWriterDlg::setSpeedMultipler( int multiplier )
    {
        fMultipler = multiplier;
        updateDelay();
    }

    void CBIFToGIFWriterDlg::setDelay( int delay )
    {
        if ( delay != -1 )
            fImpl->delay->setValue( delay );
    }

    int CBIFToGIFWriterDlg::delay() const
    {
        return fImpl->delay->value();
    }

    void CBIFToGIFWriterDlg::updateDelay()
    {
        if ( !fBIF )
            return;
        auto delay = fBIF->imageDelay();   // delay per frame;
        delay /= fMultipler;
        setDelay( delay );
    }

    void CBIFToGIFWriterDlg::setDither( bool dither )
    {
        fImpl->dither->setChecked( dither );
    }

    bool CBIFToGIFWriterDlg::dither() const
    {
        return fImpl->dither->isChecked();
    }

    void CBIFToGIFWriterDlg::setFlipImage( bool flipImage )
    {
        fImpl->flipImage->setChecked( flipImage );
    }

    bool CBIFToGIFWriterDlg::flipImage() const
    {
        return fImpl->flipImage->isChecked();
    }

    void CBIFToGIFWriterDlg::setLoopCount( int loopCount )
    {
        fImpl->loopCount->setValue( loopCount );
    }

    int CBIFToGIFWriterDlg::loopCount() const
    {
        return fImpl->loopCount->value();
    }

    void CBIFToGIFWriterDlg::slotSelectGIFFile()
    {
        auto fn = QFileDialog::getSaveFileName( this, tr( "GIF File Name" ), fImpl->fileName->text(), tr( "GIF Files (*.gif);;All Files (*.*)" ) );
        if ( fn.isEmpty() )
            return;
        fImpl->fileName->setText( fn );
    }

    void CBIFToGIFWriterDlg::accept()
    {
        if ( saveToGIF() )
            QDialog::accept();
    }

    std::size_t CBIFToGIFWriterDlg::numFramesToSave() const
    {
        return endFrame() - startFrame() + 1;
    }

    void CBIFToGIFWriterDlg::setStartFrame( std::size_t startFrame )
    {
        if ( startFrame == -1 )
            return;
        return fImpl->startFrame->setValue( static_cast< int >( startFrame ) );
    }

    std::size_t CBIFToGIFWriterDlg::startFrame() const
    {
        return fImpl->startFrame->value();
    }

    void CBIFToGIFWriterDlg::setEndFrame( std::size_t endFrame )
    {
        if ( endFrame == -1 )
            return;
        return fImpl->endFrame->setValue( static_cast< int >( endFrame ) );
    }

    std::size_t CBIFToGIFWriterDlg::endFrame() const
    {
        return fImpl->endFrame->value();
    }

    bool CBIFToGIFWriterDlg::useNew() const
    {
        return fImpl->useNew->isChecked();
    }

    void CBIFToGIFWriterDlg::setUseNew( bool useNew )
    {
        fImpl->useNew->setChecked( useNew );
        slotUpdateFileName();
    }

    bool CBIFToGIFWriterDlg::saveToGIF( QWidget *parent, const QString &fileName, const QList< QImage > imageFiles, std::size_t startFrame, std::size_t endFrame, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled )
    {
        if ( imageFiles.empty() )
            return false;

        QList< QImage > images;

        std::size_t max = imageFiles.count();
        endFrame = std::max( max - 1, endFrame );
        for ( auto ii = startFrame; ii <= endFrame; ++ii )
        {
            auto image = imageFiles[ ii ];
            if ( image.isNull() )
                return false;
            images.push_back( image );
        }

        return CGIFWriter::saveToGIF( parent, fileName, images, true, dither, flipImage, loopCount, delay, setRange, setCurr, wasCancelled );
    }

    bool CBIFToGIFWriterDlg::saveToGIF( QWidget *parent, const QString &fileName, const QList< QImage > images, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled )
    {
        return saveToGIF( parent, fileName, images, 0, images.count() - 1, dither, flipImage, loopCount, delay, setRange, setCurr, wasCancelled );
    }

    bool CBIFToGIFWriterDlg::saveToGIF( QWidget *parent, const QString &fileName, const QList< QFileInfo > imageFiles, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled )
    {
        return saveToGIF( parent, fileName, imageFiles, 0, imageFiles.count() - 1, dither, flipImage, loopCount, delay, setRange, setCurr, wasCancelled );
    }

    bool CBIFToGIFWriterDlg::saveToGIF( QWidget *parent, const QString &fileName, const QList< QFileInfo > imageFiles, std::size_t startFrame, std::size_t endFrame, bool dither, bool flipImage, int loopCount, int delay, std::function< void( size_t min, size_t max ) > setRange, std::function< void( size_t curr ) > setCurr, std::function< bool() > wasCancelled )
    {
        if ( imageFiles.empty() )
            return false;

        QList< QImage > images;

        std::size_t max = imageFiles.count();
        endFrame = std::max( max - 1, endFrame );
        for ( auto ii = startFrame; ii <= endFrame; ++ii )
        {
            auto fi = imageFiles[ ii ];
            auto file = QFile( fi.absoluteFilePath() );
            if ( !file.open( QFile::ReadOnly ) )
                return false;

            auto ba = file.readAll();
            auto image = QImage::fromData( ba );

            images.push_back( image );
        }

        return saveToGIF( parent, fileName, images, 0, images.count() - 1, dither, flipImage, loopCount, delay, setRange, setCurr, wasCancelled );
    }

    bool CBIFToGIFWriterDlg::saveToGIF()
    {
        bool wasCancelled = false;
        QProgressDialog dlg( this );
        dlg.setLabelText( tr( "Generating GIF" ) );
        dlg.setWindowModality( Qt::WindowModal );
        dlg.show();

        return saveToGIF(
            this, fImpl->fileName->text(), fBIF->images( startFrame() - 1, endFrame() - 1 ), dither(), flipImage(), loopCount(), delay(), [ &dlg ]( size_t min, size_t max ) { dlg.setRange( static_cast< int >( min ), static_cast< int >( max ) ); },
            [ &dlg ]( size_t curr )
            {
                dlg.setValue( static_cast< int >( curr ) );
                QApplication::processEvents();
            },
            [ &dlg ]() { return dlg.wasCanceled(); } );
    }
}
#endif