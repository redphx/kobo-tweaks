#include "settings.h"
#include "../utils.h"

TweaksSettings::TweaksSettings() : qSettings(DATA_DIR "/settings.ini", QSettings::IniFormat) {
    // Check if the settings file is empty
    if (qSettings.allKeys().isEmpty()) {
        // Set default positions for Clock & Battery widgets
        qSettings.setValue(READING_WIDGET_HEADER_LEFT, WidgetTypeSetting::toString(WidgetTypeEnum::Clock));
        qSettings.setValue(READING_WIDGET_HEADER_RIGHT, WidgetTypeSetting::toString(WidgetTypeEnum::Battery));

        qSettings.sync();
    }
}

QString validateImage(const QString& path) {
    QPixmap pix;
    const QString fullPath = QStringLiteral("%1/%2").arg(IMAGES_DIR).arg(path);
    if (!pix.load(fullPath) || pix.isNull()) {
        return QStringLiteral("");
    }

    return path;
}

void TweaksSettings::loadReadingSettings() {
    // [Reading]
    readingSettings = TweaksReadingSettings {};
    readingSettings.bookmarkImage = qSettings.value(READING_BOOKMARK_IMAGE, readingSettings.bookmarkImage).toString().trimmed();
    if (!readingSettings.bookmarkImage.isEmpty()) {
        readingSettings.bookmarkImage = validateImage(readingSettings.bookmarkImage);

        if (!readingSettings.bookmarkImage.isEmpty()) {
            QString darkImage = Utils::appendFileName(readingSettings.bookmarkImage, "_dark");
            readingSettings.bookmarkImageDark = validateImage(darkImage);
        }
    }

    readingSettings.headerFooterHeightScale = qBound(50, getIntValue(READING_HEADER_FOOTER_HEIGHT_SCALE, readingSettings.headerFooterHeightScale), 100);
    readingSettings.headerFooterMargins = qBound(0, getIntValue(READING_HEADER_FOOTER_MARGINS, readingSettings.headerFooterMargins), 100);

    // [Reading.Widget]
    readingSettings.widgetBatteryStyle = BatteryStyleSetting::fromSetting(qSettings, READING_WIDGET_BATTERY_STYLE, readingSettings.widgetBatteryStyle);
    readingSettings.widgetBatteryStyleCharging = BatteryStyleSetting::fromSetting(qSettings, READING_WIDGET_BATTERY_STYLE_CHARGING, readingSettings.widgetBatteryStyle);
    readingSettings.widgetBatteryShowWhenBelow = qBound(10, getIntValue(READING_WIDGET_BATTERY_SHOW_WHEN_BELOW, readingSettings.widgetBatteryShowWhenBelow), 100);

    readingSettings.widgetClock24hFormat = qSettings.value(READING_WIDGET_CLOCK_24H_FORMAT, readingSettings.widgetClock24hFormat).toBool();

    // It was designed this way to make it's possible
    // to have multiple widgets in the same slot in the future
    readingSettings.widgetHeaderLeft  = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_HEADER_LEFT);
    readingSettings.widgetHeaderRight = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_HEADER_RIGHT);
    readingSettings.widgetFooterLeft  = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_FOOTER_LEFT);
    readingSettings.widgetFooterRight = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_FOOTER_RIGHT);

    // Only allow Battery/Clock appear once
    bool hasBattery = false;
    bool hasClock = false;
    auto checkWidget = [&](WidgetTypeEnum& w) {
        if (w == WidgetTypeEnum::Battery) {
            if (hasBattery) {
                w = WidgetTypeEnum::Invalid;
            } else {
                hasBattery = true;
            }
        } else if (w == WidgetTypeEnum::Clock) {
            if (hasClock) {
                w = WidgetTypeEnum::Invalid;
            } else {
                hasClock = true;
            }
        }
    };

    checkWidget(readingSettings.widgetHeaderLeft);
    checkWidget(readingSettings.widgetHeaderRight);
    checkWidget(readingSettings.widgetFooterLeft);
    checkWidget(readingSettings.widgetFooterRight);
}

void TweaksSettings::load() {
    qSettings.sync();
    loadReadingSettings();
}

void TweaksSettings::sync() {
    qSettings.setValue(READING_BOOKMARK_IMAGE, readingSettings.bookmarkImage);
    qSettings.setValue(READING_HEADER_FOOTER_HEIGHT_SCALE, readingSettings.headerFooterHeightScale);
    qSettings.setValue(READING_HEADER_FOOTER_MARGINS, readingSettings.headerFooterMargins);

    qSettings.setValue(READING_WIDGET_HEADER_LEFT, WidgetTypeSetting::toString(readingSettings.widgetHeaderLeft));
    qSettings.setValue(READING_WIDGET_HEADER_RIGHT, WidgetTypeSetting::toString(readingSettings.widgetHeaderRight));
    qSettings.setValue(READING_WIDGET_FOOTER_LEFT, WidgetTypeSetting::toString(readingSettings.widgetFooterLeft));
    qSettings.setValue(READING_WIDGET_FOOTER_RIGHT, WidgetTypeSetting::toString(readingSettings.widgetFooterRight));

    qSettings.setValue(READING_WIDGET_BATTERY_STYLE, BatteryStyleSetting::toString(readingSettings.widgetBatteryStyle));
    qSettings.setValue(READING_WIDGET_BATTERY_STYLE_CHARGING, BatteryStyleSetting::toString(readingSettings.widgetBatteryStyleCharging));
    qSettings.setValue(READING_WIDGET_BATTERY_SHOW_WHEN_BELOW, readingSettings.widgetBatteryShowWhenBelow);

    qSettings.setValue(READING_WIDGET_CLOCK_24H_FORMAT, readingSettings.widgetClock24hFormat);

    qSettings.sync();
}

QString TweaksSettings::getReadingBookmarkImage(bool isDarkMode) {
    if (isDarkMode && !readingSettings.bookmarkImageDark.isEmpty()) {
        return QStringLiteral("%1/%2").arg(IMAGES_DIR).arg(readingSettings.bookmarkImageDark);
    }

    if (!readingSettings.bookmarkImage.isEmpty()) {
        return QStringLiteral("%1/%2").arg(IMAGES_DIR).arg(readingSettings.bookmarkImage);
    }

    return "";
}

const TweaksReadingSettings& TweaksSettings::getReadingSettings() const {
    return readingSettings;
}

int TweaksSettings::getIntValue(const QString& key, int defaultValue) {
    // Return the default value when the key is missing or the value is not a number
    bool ok;
    int value = qSettings.value(key, defaultValue).toInt(&ok);
    if (!ok) {
        value = defaultValue;
    }

    return value;
}
