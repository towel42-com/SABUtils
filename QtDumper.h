#ifndef __QTDUMPER_H
#define __QTDUMPER_H

#include "Towel42UtilsExport.h"

class QWidget;
class QStandardItemModel;
namespace NTowel42Utils
{
    TOWEL42_UTILS_EXPORT void dumpWidgetAndChildren( const QWidget *widget, QStandardItemModel *model );
}
#endif
