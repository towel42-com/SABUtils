#ifndef __SVGUTILS_H
#define __SVGUTILS_H

#include "Towel42UtilsExport.h"
#include <optional>
#include <QSize>

class QPixmap;
class QString;
class QByteArray;
class QSvgRenderer;
class QIODevice;
namespace NTowel42Utils
{
    TOWEL42_UTILS_EXPORT std::optional< QPixmap > getSVG( const QString &svgFile, const std::optional< QSize > &sz = {} );
    TOWEL42_UTILS_EXPORT std::optional< QPixmap > getSVG( const QByteArray &data, const std::optional< QSize > &sz = {} );
    TOWEL42_UTILS_EXPORT std::optional< QPixmap > getSVG( QSvgRenderer *renderer, const std::optional< QSize > &sz = {} );
    TOWEL42_UTILS_EXPORT bool isSVG( const QByteArray &data );
    TOWEL42_UTILS_EXPORT bool isSVG( QIODevice *device );
}
#endif
