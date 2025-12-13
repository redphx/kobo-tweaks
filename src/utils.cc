#include "utils.h"

#include <QFile>
#include <QTextStream>
#include <QVariant>
#include <QVector>
#include <QPair>
#include <QRegularExpression>

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
