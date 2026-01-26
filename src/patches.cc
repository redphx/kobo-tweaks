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

        QString setFixedHeight(QString qss, const QString& selector, int height) {
            qss.append(
                QStringLiteral("\n%1 { min-height: %2px; max-height: %2px; }\n")
                    .arg(selector)
                    .arg(height)
            );
            return qss;
        }

        QString resetHeight(QString qss, const QString& selector) {
            // 16777215 = QWIDGETSIZE_MAX
            qss.append(
                QStringLiteral("\n%1 { min-height: 0px; max-height: 16777215px; }\n")
                    .arg(selector)
            );
            return qss;
        }

        QString setPaddings(QString qss, const QString& selector, int top, int right, int bottom, int left) {
            qss.append(
                QStringLiteral("\n%1 { padding-top: %2px; padding-right: %3px; padding-bottom: %4px; padding-left: %5px; }\n")
                    .arg(selector)
                    .arg(top)
                    .arg(right)
                    .arg(bottom)
                    .arg(left)
            );
            return qss;
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
