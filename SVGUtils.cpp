#include "SVGUtils.h"

#include <QSvgRenderer>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QTextStream>
#include <QIODevice>
#include <QXmlStreamReader>

namespace NTowel42Utils
{
    std::optional< QImage > getSVGImage( QSvgRenderer *renderer, const std::optional< QSize > &sz /*= {} */ )
    {
        Q_ASSERT( renderer && renderer->isValid() );
        if ( !renderer || !renderer->isValid() )
            return {};

        auto renderToSize = renderer->defaultSize();
        if ( sz.has_value() )
            renderToSize = sz.value();

        QImage image( renderToSize, QImage::Format_ARGB32 );
        image.fill( Qt::transparent );
        QPainter painter( &image );
        renderer->render( &painter );
        return image;
    }

    std::optional< QPixmap > getSVG( QSvgRenderer *renderer, const std::optional< QSize > &sz )
    {
        auto image = getSVGImage( renderer, sz );
        if ( image.has_value() )
            return QPixmap::fromImage( image.value() );
        return {};
    }

    std::optional< QPixmap > getSVG( const QString &svgFile, const std::optional< QSize > &sz )
    {
        QSvgRenderer renderer( svgFile );
        return getSVG( &renderer, sz );
    }

    std::optional< QImage > getSVGImage( const QString &svgFile, const std::optional< QSize > &sz /*= {} */ )
    {
        QSvgRenderer renderer( svgFile );
        return getSVGImage( &renderer, sz );
    }

    std::optional< QPixmap > getSVG( const QByteArray &svgData, const std::optional< QSize > &sz )
    {
        QSvgRenderer renderer( svgData );
        return getSVG( &renderer, sz );
    }

    std::optional< QImage > getSVGImage( const QByteArray &svgData, const std::optional< QSize > &sz /*= {} */ )
    {
        QSvgRenderer renderer( svgData );
        return getSVGImage( &renderer, sz );
    }

    bool isSVG( const QByteArray &data )
    {
        QTextStream ts( data );
        return isSVG( ts.device() );
    }

    bool isSVG( QIODevice *device )
    {
        if ( !device->isOpen() )
        {
            if ( !device->open( QIODevice::ReadOnly ) )
                return false;
        }
        QXmlStreamReader xml( device );
        while ( !xml.atEnd() && !xml.isStartElement() )
        {
            xml.readNext();
        }

        return ( xml.isStartElement() && xml.name().toString().compare( "svg", Qt::CaseInsensitive ) == 0 );
    }

}
