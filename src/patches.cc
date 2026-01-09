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
            const QVariant fn = QVariant::fromValue<QssPropertyFunc>([scale](const QString& property, const QString& value) {
                return Qss::scaleValue(property, value, scale);
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

        QString reduceSpacerHeight(const QString& qss, const QString& selector, int scale) {
            QString result(qss);

            const QVector<QPair<QString, int>> properties = {
                {QStringLiteral("Trilogy"), 24},
                {QStringLiteral("Phoenix"), 32},
                {QStringLiteral("Dragon"), 44},
                {QStringLiteral("Storm"), 50},
                {QStringLiteral("Daylight"), 56},
            };

            result += QStringLiteral("\n");

            for (const QPair<QString, int>& pair : properties) {
                QString device = pair.first;
                int newValue = pair.second * qMax(0, scale) / 100;

                result += QStringLiteral("%1[qApp_deviceIs%2=true] { min-height: %3px; max-height: %3px; }\n").arg(selector).arg(device).arg(newValue);
            }

            return result;
        }

        QString addBrightnessLabelQss(const QString& qss) {
            QString result(qss);

            result += QStringLiteral("\n")
                + QStringLiteral("#twksBrightnessLabel { border: 1px solid black; background: white; padding: 6px; }\n")
                + QStringLiteral("#gestureContainer[darkMode=true] #twksBrightnessLabel { border: 1px solid white; background: black; }\n")
                + QStringLiteral("#twksBrightnessLabel[qApp_deviceIsTrilogy=true] { font-size: 14px; }\n")
                + QStringLiteral("#twksBrightnessLabel[qApp_deviceIsPhoenix=true] { font-size: 17px; }\n")
                + QStringLiteral("#twksBrightnessLabel[qApp_deviceIsDragon=true] { font-size: 25px; }\n")
                + QStringLiteral("#twksBrightnessLabel[qApp_deviceIsStorm=true] { font-size: 29px; }\n")
                + QStringLiteral("#twksBrightnessLabel[qApp_deviceIsDaylight=true] { font-size: 32px; }\n");

            return result;
        }
    }
}
