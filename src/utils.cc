#include "utils.h"
#include "widgets/separator_label.h"

#include <QFile>
#include <QTextStream>
#include <QVariant>
#include <QVector>
#include <QPair>
#include <QRegularExpression>
#include <QWidget>
#include <QLayout>

static const int qssMetaReg = qRegisterMetaType<QssPropertyFunc>();

namespace Qss {

    static QString resolveNewProperty(const QVariant& v, const QString& property, const QString& matchedValue) {
        if (v.canConvert<QString>()) {
            return v.toString();
        }

        return v.value<QssPropertyFunc>()(property, matchedValue);
    }

    void dump(const QString& name, const QString& qss) {
        QFile file("/mnt/onboard/" + name + ".qss");

        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << qss;
            file.close();
        }
    }

    QString getContent(const QString& path) {
        static QHash<QString, QString> cache;

        if (cache.contains(path))
            return cache[path];

        QFile file(path);
        if (!file.open(QFile::ReadOnly | QFile::Text))
            return QString();

        const QString content = QString::fromUtf8(file.readAll());
        cache.insert(path, content);

        return content;
    }

    QString updateProperties(const QString& qss, const QString& selector, const QVector<QPair<QString, QVariant>>& properties) {
        int startPos = 0;
        QString result = qss;

        while (true) {
            // Find the selector block
            int selectorStart = result.indexOf(selector + QStringLiteral(" {"), startPos);
            if (selectorStart == -1) {
                selectorStart = result.indexOf(selector + ',', startPos);
                if (selectorStart == -1) {
                    break;
                }
            }

            startPos = selectorStart;
            const int blockStart = result.indexOf('{', selectorStart);
            const int blockEnd = result.indexOf('}', blockStart);
            if (blockStart == -1 || blockEnd == -1) {
                continue;
            }

            startPos = blockEnd;
            QString blockContent = result.mid(blockStart + 1, blockEnd - blockStart - 1);

            for (const auto &property : properties) {
                const QString prop = property.first;

                const int propPos = blockContent.indexOf(prop + ':');
                if (propPos == -1) {
                    continue;
                }

                const int semicolonPos = blockContent.indexOf(';', propPos);
                if (semicolonPos == -1) {
                    continue;
                }

                // Replace the value
                const int valueStart = propPos + prop.length() + 1; // skip ":"
                const int valueLength = semicolonPos - valueStart;
                QString value = blockContent.mid(valueStart, valueLength);

                QString newProperty = resolveNewProperty(property.second, prop, value);
                blockContent.replace(propPos, semicolonPos + 1 - propPos, newProperty);
            }

            // Replace block content
            result.replace(blockStart + 1, blockEnd - blockStart - 1, blockContent);
        }

        return result;
    }

    QString copySelectors(const QString& qss, const QString& selector, const QStringList& list) {
        QString result = qss;

        // Match selector optionally followed by [...] one or more times, until {
        QString pattern = QString("(%1(?:\\[[^\\]]+\\])*)(\\s*\\{)").arg(QRegularExpression::escape(selector));
        QRegularExpression re(pattern);

        QRegularExpressionMatch match;
        int offset = 0;

        while ((match = re.match(result, offset)).hasMatch()) {
            int pos = match.capturedStart(1) + match.capturedLength(1);
            offset = pos;
            for (const QString& extra : list) {
                QString replace = match.captured(1).replace(selector, extra);
                result.insert(pos, ',' + replace);
                offset += replace.length() + 2; // move past inserted text
            }
        }

        return result;
    }

    QString scaleValue(const QString& property, const QString& value, int scale) {
        bool ok;
        int rawNumber = QString(value).replace(QStringLiteral("px"), "").toInt(&ok);
        if (ok) {
            return QStringLiteral("%1:%2px;").arg(property).arg(rawNumber * qMax(0, scale) / 100);
        }

        return QStringLiteral("%1:%2;").arg(property).arg(value);
    }
}

namespace Utils {
    QString appendFileName(const QString& filePath, const QString append) {
        const QFileInfo fi(filePath);

        const QString dir  = fi.absolutePath();
        const QString name = fi.completeBaseName();
        const QString ext  = fi.suffix();

        QString out = dir;
        if (!out.endsWith('/'))
            out += '/';

            out += name;
        out += append;

        if (!ext.isEmpty()) {
            out += '.';
            out += ext;
        }

        return out;
    }
}

namespace WidgetUtils {
    void syncSeparatorVisibility(QWidget* widget) {
        QWidget* parentWidget = widget->parentWidget();
        QLayout* parentLayout = parentWidget ? parentWidget->layout() : nullptr;
        if (!parentLayout) {
            return;
        }

        int widgetsCount = parentLayout->count();
        if (widgetsCount < 3) {
            // Count < 3 => no separator
            return;
        }

        int index = parentLayout->indexOf(widget);
        if (index == -1) {
            return;
        }

        int targetIndex;
        if (index == 0) {
            // at the beginning -> find separator on the right side
            targetIndex = index + 1;
        } else {
            // in the middle or at the end -> find separator on the left side
            targetIndex = index - 1;
        }

        if (targetIndex < 0 || targetIndex >= widgetsCount) {
            return;
        }

        QLayoutItem* item = parentLayout->itemAt(targetIndex);
        if (auto* separator = qobject_cast<TwSeparatorLabel*>(item->widget())) {
            bool thisVisible = !widget->isHidden();

            if (index == 0) {
                // When current widget is at the beginning, its separator has two owners
                // Get the widget after separator
                QLayoutItem* otherItem = parentLayout->itemAt(2);
                auto* otherWidget = qobject_cast<QWidget*>(otherItem->widget());
                // Separator is visible only when both widgets are visible
                if (otherWidget) {
                    bool otherVisible = !otherWidget->isHidden();
                    separator->setVisible(thisVisible && otherVisible);
                } else {
                    separator->setVisible(thisVisible);
                }
            } else {
                separator->setVisible(thisVisible);
            }
        }
    }
}
