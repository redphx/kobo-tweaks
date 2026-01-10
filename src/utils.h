#pragma once
#include <functional>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QVector>
#include <QPair>
#include <QVariant>

using QssPropertyFunc = std::function<QString(const QString&, const QString&)>;
Q_DECLARE_METATYPE(QssPropertyFunc)

namespace Qss {
    void dump(const QString& name, const QString& qss);
    QString getContent(const QString& path);
    QString updateProperties(const QString& qss, const QString& selector, const QVector<QPair<QString, QVariant>>& properties);
    QString copySelectors(const QString& qss, const QString& selector, const QStringList& list);
    QString scaleValue(const QString& property, const QString& value, int scale);
}

namespace Utils {
    QString appendFileName(const QString& filePath, const QString append);
}

namespace WidgetUtils {
    void syncSeparatorVisibility(QWidget* w);
}
