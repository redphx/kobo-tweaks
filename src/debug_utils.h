#pragma once
#include <QString>
#include <QWidget>

namespace DebugUtils {
    QString dumpWidget(QWidget *w, int indent, QSet<QObject*> &seen);
    bool dumpWidgetToFile(const QString& path, QWidget* w);
}
