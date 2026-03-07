#include "SVGUtils.h"

#include <QSvgRenderer>
#include <QImage>
#include <QPainter>
#include <QPixmap>

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
}
