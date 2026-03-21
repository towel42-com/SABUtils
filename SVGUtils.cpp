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
    std::optional< QPixmap > getSVG( QSvgRenderer *renderer, const std::optional< QSize > &sz )
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
        return QPixmap::fromImage( image );
    }

    std::optional< QPixmap > getSVG( const QString &svgFile, const std::optional< QSize > &sz )
    {
        QSvgRenderer renderer( svgFile );
        return getSVG( &renderer, sz );
    }

    std::optional< QPixmap > getSVG( const QByteArray &svgData, const std::optional< QSize > &sz )
    {
        QSvgRenderer renderer( svgData );
        return getSVG( &renderer, sz );
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
