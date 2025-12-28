#include "patches.h"
#include "utils.h"
#include <QString>
#include <QVariant>
#include <QPair>
#include <QVector>

namespace Patch {
    namespace ReadingView {
        QString scaleHeaderFooterHeight(const QString& qss, int scale) {
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

        QString reduceSpacerHeight(const QString& qss) {
            QString result(qss);

            result += QStringLiteral("\n")
                + QStringLiteral("MediumVertSpacer[qApp_deviceIsTrilogy=true] { min-height: 12px; max-height: 12px; }\n")
                + QStringLiteral("MediumVertSpacer[qApp_deviceIsPhoenix=true] { min-height: 16px; max-height: 16px; }\n")
                + QStringLiteral("MediumVertSpacer[qApp_deviceIsDragon=true] { min-height: 22px; max-height: 22px; }\n")
                + QStringLiteral("MediumVertSpacer[qApp_deviceIsDaylight=true] { min-height: 28px; max-height: 28px; }\n")
                + QStringLiteral("MediumVertSpacer[qApp_deviceIsStorm=true] { min-height: 25px; max-height: 25px; }\n");

            return result;
        }
    }
}
