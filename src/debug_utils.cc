#include "debug_utils.h"
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QLayout>
#include <QSet>
#include <QStringBuilder>

namespace DebugUtils {
    QString dumpLayout(QLayout *layout, int indent, QSet<QObject*> &seen) {
        if (seen.contains(layout))
            return QString();
        seen.insert(layout);

        QString pad(indent, ' ');
        QString s;

        s += pad + "Layout: ";
        s += layout->metaObject()->className();
        s += "\n";

        for (int i = 0; i < layout->count(); i++) {
            QLayoutItem *item = layout->itemAt(i);

            if (QWidget *w = item->widget()) {
                s += dumpWidget(w, indent + 2, seen);
                continue;
            }

            if (QLayout *child = item->layout()) {
                s += dumpLayout(child, indent + 2, seen);
                continue;
            }

            s += pad + "  Spacer\n";
        }

        return s;
    }

    QString dumpWidget(QWidget *w, int indent, QSet<QObject*> &seen) {
        if (!w || seen.contains(w))
            return QString();
        seen.insert(w);

        QString pad(indent, ' ');
        QString s;

        s += pad + "Widget: ";
        s += w->objectName();
        s += " (" + QString(w->metaObject()->className()) + ")";

        const QString ss = w->styleSheet();
        if (!ss.isEmpty())
            s += "  style=\"" + ss + "\"";

        const auto dp = w->dynamicPropertyNames();
        if (!dp.isEmpty()) {
            s += "  props=[";
            for (int i = 0; i < dp.size(); i++) {
                const QByteArray &k = dp[i];
                s += QString::fromUtf8(k);
                s += "=";
                s += w->property(k).toString();
                if (i + 1 < dp.size())
                    s += ", ";
            }
            s += "]";
        }

        s += "\n";

        if (QLayout *layout = w->layout())
            s += dumpLayout(layout, indent + 2, seen);

        for (QObject *c : w->children()) {
            if (QWidget *cw = qobject_cast<QWidget*>(c))
                s += dumpWidget(cw, indent + 2, seen);
        }

        return s;
    }

    QString dumpTree(QWidget *root) {
        QSet<QObject*> seen;
        return dumpWidget(root, 0, seen);
    }

    bool dumpWidgetToFile(const QString& path, QWidget* w) {
        // QTimer::singleShot(2000, self, [self] { DebugUtils::dumpWidgetToFile(QString("/mnt/onboard/_layout.txt"), self); });
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            return false;
        }

        QTextStream out(&file);
        out << dumpTree(w);
        file.close();

        return true;
    }
}
