#include "settings.h"
#include "../utils.h"

#include <QVector>

TweaksSettings::TweaksSettings() : qSettings(DATA_DIR "/settings.ini", QSettings::IniFormat) {
    qSettings.setIniCodec("UTF-8");
}

QString validateImage(const QString& path) {
    QPixmap pix;
    const QString fullPath = QStringLiteral("%1/%2").arg(IMAGES_DIR).arg(path);
    if (!pix.load(fullPath) || pix.isNull()) {
        return QStringLiteral("");
    }

    return path;
}

void TweaksSettings::setMissingKeys() {
    bool changed = false;
    auto checkValue = [&](const QString& key, QVariant defaultValue) {
        if (!qSettings.contains(key)) {
            qSettings.setValue(key, defaultValue);
            changed = true;
        }
    };

    // Set default positions for Clock & Battery widgets
    checkValue(READING_WIDGET_HEADER_LEFT, WidgetTypeSetting::toString(WidgetTypeEnum::Clock));
    checkValue(READING_WIDGET_HEADER_RIGHT, WidgetTypeSetting::toString(WidgetTypeEnum::Battery));

    // Set default positions for Book & Chapter widgets
    checkValue(READING_WIDGET_HEADER_CENTER, QStringList() << WidgetTypeSetting::toString(WidgetTypeEnum::ChapterTitle) << WidgetTypeSetting::toString(WidgetTypeEnum::ChapterPage));
    checkValue(READING_WIDGET_FOOTER_CENTER, QStringList() << WidgetTypeSetting::toString(WidgetTypeEnum::BookTitle) << WidgetTypeSetting::toString(WidgetTypeEnum::BookPage));

    // Set default widget separator
    checkValue(READING_WIDGET_SEPARATOR, WidgetSeparatorSetting::toString(WidgetSeparatorEnum::Dot));

    if (changed) {
        qSettings.sync();
    }
}

void TweaksSettings::loadReadingSettings() {
    // [Reading]
    readingSettings = TweaksReadingSettings {};
    readingSettings.bookmarkImage = getStringValue(READING_BOOKMARK_IMAGE, readingSettings.bookmarkImage);
    if (!readingSettings.bookmarkImage.isEmpty()) {
        readingSettings.bookmarkImage = validateImage(readingSettings.bookmarkImage);

        if (!readingSettings.bookmarkImage.isEmpty()) {
            QString darkImage = Utils::appendFileName(readingSettings.bookmarkImage, QStringLiteral("_dark"));
            readingSettings.bookmarkImageDark = validateImage(darkImage);
        }
    }

    readingSettings.headerFooterHeightScale = qBound(50, getIntValue(READING_HEADER_FOOTER_HEIGHT_SCALE, readingSettings.headerFooterHeightScale), 100);
    readingSettings.headerFooterMargins = qBound(0, getIntValue(READING_HEADER_FOOTER_MARGINS, readingSettings.headerFooterMargins), 100);

    readingSettings.headerSpacerHeight = qBound(0, getIntValue(READING_HEADER_SPACER_HEIGHT, readingSettings.headerSpacerHeight), 100);
    readingSettings.footerSpacerHeight = qBound(0, getIntValue(READING_FOOTER_SPACER_HEIGHT, readingSettings.footerSpacerHeight), 100);

    // [Reading.Widget]
    readingSettings.widgetSeparator = WidgetSeparatorSetting::fromSetting(qSettings, READING_WIDGET_SEPARATOR, readingSettings.widgetSeparator);
    readingSettings.widgetSpacing = qBound(0, getIntValue(READING_WIDGET_SPACING, readingSettings.widgetSpacing), 20);

    readingSettings.widgetBatteryStyle = BatteryStyleSetting::fromSetting(qSettings, READING_WIDGET_BATTERY_STYLE, readingSettings.widgetBatteryStyle);
    readingSettings.widgetBatteryStyleCharging = BatteryStyleSetting::fromSetting(qSettings, READING_WIDGET_BATTERY_STYLE_CHARGING, readingSettings.widgetBatteryStyle);
    readingSettings.widgetBatteryShowWhenBelow = qBound(10, getIntValue(READING_WIDGET_BATTERY_SHOW_WHEN_BELOW, readingSettings.widgetBatteryShowWhenBelow), 100);

    readingSettings.widgetClock24hFormat = qSettings.value(READING_WIDGET_CLOCK_24H_FORMAT, readingSettings.widgetClock24hFormat).toBool();

    // It was designed this way to make it's possible
    // to have multiple widgets in the same slot in the future
    readingSettings.widgetHeaderLeft   = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_HEADER_LEFT, QVector<WidgetTypeEnum>{});
    readingSettings.widgetHeaderCenter = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_HEADER_CENTER, QVector<WidgetTypeEnum>{});
    readingSettings.widgetHeaderRight  = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_HEADER_RIGHT, QVector<WidgetTypeEnum>{});
    readingSettings.widgetFooterLeft   = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_FOOTER_LEFT, QVector<WidgetTypeEnum>{});
    readingSettings.widgetFooterCenter = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_FOOTER_CENTER, QVector<WidgetTypeEnum>{});
    readingSettings.widgetFooterRight  = WidgetTypeSetting::fromSetting(qSettings, READING_WIDGET_FOOTER_RIGHT, QVector<WidgetTypeEnum>{});

    validateWidgets();
}

void TweaksSettings::load() {
    qSettings.sync();
    setMissingKeys();

    loadReadingSettings();
}

void TweaksSettings::sync() {
    qSettings.setValue(READING_BOOKMARK_IMAGE, readingSettings.bookmarkImage);
    qSettings.setValue(READING_HEADER_FOOTER_HEIGHT_SCALE, readingSettings.headerFooterHeightScale);
    qSettings.setValue(READING_HEADER_FOOTER_MARGINS, readingSettings.headerFooterMargins);

    qSettings.setValue(READING_HEADER_SPACER_HEIGHT, readingSettings.headerSpacerHeight);
    qSettings.setValue(READING_FOOTER_SPACER_HEIGHT, readingSettings.footerSpacerHeight);

    qSettings.setValue(READING_WIDGET_HEADER_LEFT, WidgetTypeSetting::toString(readingSettings.widgetHeaderLeft));
    qSettings.setValue(READING_WIDGET_HEADER_CENTER, WidgetTypeSetting::toString(readingSettings.widgetHeaderCenter));
    qSettings.setValue(READING_WIDGET_HEADER_RIGHT, WidgetTypeSetting::toString(readingSettings.widgetHeaderRight));

    qSettings.setValue(READING_WIDGET_FOOTER_LEFT, WidgetTypeSetting::toString(readingSettings.widgetFooterLeft));
    qSettings.setValue(READING_WIDGET_FOOTER_CENTER, WidgetTypeSetting::toString(readingSettings.widgetFooterCenter));
    qSettings.setValue(READING_WIDGET_FOOTER_RIGHT, WidgetTypeSetting::toString(readingSettings.widgetFooterRight));

    qSettings.setValue(READING_WIDGET_BATTERY_STYLE, BatteryStyleSetting::toString(readingSettings.widgetBatteryStyle));
    qSettings.setValue(READING_WIDGET_BATTERY_STYLE_CHARGING, BatteryStyleSetting::toString(readingSettings.widgetBatteryStyleCharging));
    qSettings.setValue(READING_WIDGET_BATTERY_SHOW_WHEN_BELOW, readingSettings.widgetBatteryShowWhenBelow);

    qSettings.setValue(READING_WIDGET_CLOCK_24H_FORMAT, readingSettings.widgetClock24hFormat);

    qSettings.setValue(READING_WIDGET_SEPARATOR, WidgetSeparatorSetting::toString(readingSettings.widgetSeparator));
    qSettings.setValue(READING_WIDGET_SPACING, readingSettings.widgetSpacing);

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

QString TweaksSettings::getStringValue(const QString& key, const QString& defaultValue) {
    return qSettings.value(key, defaultValue).toString().trimmed();
}

void TweaksSettings::validateWidgets() {
    // Only allow each widget to appear once
    uint32_t seenMask = 0;
    for (auto* zone : {&readingSettings.widgetHeaderLeft, &readingSettings.widgetHeaderRight, &readingSettings.widgetFooterLeft, &readingSettings.widgetFooterRight}) {
        auto it = std::remove_if(zone->begin(), zone->end(), [&](WidgetTypeEnum w) {
            // Map enum values to bit positions (0-31)
            uint32_t bit = 1 << static_cast<int>(w);

            // If bit is already set, return true to remove the duplicate
            if (seenMask & bit) {
                return true;
            }

            // Otherwise, mark as seen and keep it
            seenMask |= bit;
            return false;
        });

        zone->erase(it, zone->end());
    }
}

void TweaksSettings::migrate() {
    QVector<QPair<QString, QString>> migrations = {
        {QStringLiteral("Reading.Widget/BatteryStyle"), READING_WIDGET_BATTERY_STYLE},
        {QStringLiteral("Reading.Widget/BatteryStyleCharging"), READING_WIDGET_BATTERY_STYLE_CHARGING},
        {QStringLiteral("Reading.Widget/BatteryShowWhenBelow"), READING_WIDGET_BATTERY_SHOW_WHEN_BELOW},

        {QStringLiteral("Reading.Widget/Clock24hFormat"), READING_WIDGET_CLOCK_24H_FORMAT},
    };

    for (const auto& pair : migrations) {
        const QString& oldKey = pair.first;
        const QString& newKey = pair.second;

        if (qSettings.contains(oldKey)) {
            // Only migrate if newKey doesn't exist
            if (!qSettings.contains(newKey)) {
                qSettings.setValue(newKey, qSettings.value(oldKey));
            }
            qSettings.remove(oldKey);
        }
    }

    qSettings.sync();
}
