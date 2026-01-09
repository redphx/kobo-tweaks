#include "utils.h"
#include <QString>

namespace Patch {
    namespace ReadingView {
        QString scaleHeaderFooterHeight(const QString& qss, int scale);
        QString setFixedHeight(const QString& qss, const QString& selector, int height);
        QString addBrightnessLabelQss(const QString& qss);
    }
}
