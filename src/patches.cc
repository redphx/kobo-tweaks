#include "patches.h"
#include "utils.h"
#include <QString>
#include <QVariant>
#include <QPair>
#include <QVector>

namespace Patch {
    namespace ReadingView {
        QString scaleHeaderFooterHeight(const QString &qss, int scale) {
            // Based on: Reduce new header/footer height - jackie_w

            const QVariant fn = QVariant::fromValue<QssPropertyFunc>([&](const QString& property, const QString& value) {
                bool ok;
                int rawNumber = QString(value).replace(QStringLiteral("px"), "").toInt(&ok);
                if (ok) {
                    return QStringLiteral("%1:%2px;").arg(property).arg(rawNumber * scale / 100);
                }

                return QStringLiteral("%1:%2;").arg(property).arg(value);
            });

            const QVector<QPair<QString, QVariant>> properties = {
                {QStringLiteral("min-height"), fn},
                {QStringLiteral("max-height"), fn},
            };

            QString result(qss);

            // Touch/Mini (Trilogy)
            result = Qss::updateProperties(result, QStringLiteral("ReadingFooter[qApp_deviceIsTrilogy=true]"), properties);
            // Glo/Aura/Aura2/Nia (Phoenix)
            result = Qss::updateProperties(result, QStringLiteral("ReadingFooter[qApp_deviceIsPhoenix=true]"), properties);
            // AuraHD/AuraH2O/AuraH202/GloHD/ClaraHD/Clara2E (Dragon)
            result = Qss::updateProperties(result, QStringLiteral("ReadingFooter[qApp_deviceIsDragon=true]"), properties);
            // AuraOne/Forma/Sage/Elipsa/Elipsa2E (Daylight)
            result = Qss::updateProperties(result, QStringLiteral("ReadingFooter[qApp_deviceIsDaylight=true]"), properties);
            // LibraH2O/Libra2 (Storm)
            result = Qss::updateProperties(result, QStringLiteral("ReadingFooter[qApp_deviceIsStorm=true]"), properties);

            return result;
        }
    }
}
