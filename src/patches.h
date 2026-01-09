#include "utils.h"
#include <QString>

namespace Patch {
    namespace ReadingView {
        QString scaleHeaderFooterHeight(const QString& qss, int scale);
        QString reduceSpacerHeight(const QString& qss);
        QString addBrightnessLabelQss(const QString& qss);
    }
}
