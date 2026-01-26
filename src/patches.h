#include "utils.h"
#include <QString>

namespace Patch {
    namespace ReadingView {
        QString scaleHeaderFooterHeight(const QString& qss, int scale);
        QString setFixedHeight(QString qss, const QString& selector, int height);
        QString resetHeight(QString qss, const QString& selector);
        QString setPaddings(QString qss, const QString& selector, int top, int right, int bottom, int left);
        QString addBrightnessLabelQss(const QString& qss);
    }
}
