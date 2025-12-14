#pragma once

#include "../common.h"

#include <QSettings>
#include <QHash>


template <typename EnumType>
inline uint qHashEnum(EnumType e, uint seed = 0) {
    return ::qHash(static_cast<int>(e), seed);
}

template<typename Derived, typename EnumType>
class SettingEnumValue {
public:
    static EnumType fromSetting(const QSettings& settings, const char* key, EnumType defaultValue=EnumType::Invalid) {
        QString value = settings.value(key, "").toString();
        return fromString(value, defaultValue);
    }

    static EnumType fromString(const QString &str, EnumType defaultValue=EnumType::Invalid) {
        EnumType value = Derived::valueMap().value(str.toLower(), EnumType::Invalid);
        if (value == EnumType::Invalid) {
            return defaultValue;
        }

        return value;
    }

    static QString toString(EnumType e) {
        const auto &map = Derived::reverseMap();
        return map.value(e, QString());
    }

    static const QHash<QString, EnumType>& valueMap();
    static const QHash<EnumType, QString>& reverseMap();
    static EnumType defaultValue();
};


// WidgetTypeSetting
enum class WidgetTypeEnum { Invalid, Battery, Clock };
inline uint qHash(WidgetTypeEnum e, uint seed = 0) { return qHashEnum(e, seed); }
class WidgetTypeSetting : public SettingEnumValue<WidgetTypeSetting, WidgetTypeEnum> {
public:
    using EnumType = WidgetTypeEnum;

    static const QHash<QString, EnumType>& valueMap() {
        static const QHash<QString, EnumType> map = {
            {QStringLiteral("battery"), EnumType::Battery},
            {QStringLiteral("clock"),   EnumType::Clock},
        };
        return map;
    }

    static const QHash<EnumType, QString>& reverseMap() {
        static const QHash<EnumType, QString> map = {
            {EnumType::Battery, QStringLiteral("Battery")},
            {EnumType::Clock,   QStringLiteral("Clock")},
            {EnumType::Invalid, ""},
        };
        return map;
    }

    static EnumType defaultValue() { return EnumType::Invalid; }
};

// BatteryStyleSetting
enum class BatteryStyleEnum { Invalid, Icon, Level, IconLevel, LevelIcon };
inline uint qHash(BatteryStyleEnum e, uint seed = 0) { return qHashEnum(e, seed); }
class BatteryStyleSetting : public SettingEnumValue<BatteryStyleSetting, BatteryStyleEnum> {
public:
    using EnumType = BatteryStyleEnum;

    static const QHash<QString, EnumType>& valueMap() {
        static const QHash<QString, EnumType> map = {
            {QStringLiteral("icon"),      EnumType::Icon},
            {QStringLiteral("level"),     EnumType::Level},
            {QStringLiteral("iconlevel"), EnumType::IconLevel},
            {QStringLiteral("levelicon"), EnumType::LevelIcon},
        };
        return map;
    }

    static const QHash<EnumType, QString>& reverseMap() {
        static const QHash<EnumType, QString> map = {
            {EnumType::Icon,      QStringLiteral("Icon")},
            {EnumType::Level,     QStringLiteral("Level")},
            {EnumType::IconLevel, QStringLiteral("IconLevel")},
            {EnumType::LevelIcon, QStringLiteral("LevelIcon")},
            {EnumType::Invalid,   ""},
        };
        return map;
    }

    static EnumType defaultValue() { return EnumType::Invalid; }
};


struct TweaksReadingSettings {
    QString bookmarkImage = "";
    QString bookmarkImageDark = "";
    int headerFooterHeightScale = 100;

    WidgetTypeEnum widgetHeaderLeft = WidgetTypeEnum::Invalid;
    WidgetTypeEnum widgetHeaderRight = WidgetTypeEnum::Invalid;
    WidgetTypeEnum widgetFooterLeft = WidgetTypeEnum::Invalid;
    WidgetTypeEnum widgetFooterRight = WidgetTypeEnum::Invalid;

    BatteryStyleEnum widgetBatteryStyle = BatteryStyleEnum::IconLevel;
    BatteryStyleEnum widgetBatteryStyleCharging = BatteryStyleEnum::IconLevel;
    int widgetBatteryShowWhenBelow = 100;

    bool widgetClock24hFormat = true;
};

class TweaksSettings {
    const char* READING_BOOKMARK_IMAGE = "Reading/BookmarkImage";
    const char* READING_HEADER_FOOTER_HEIGHT_SCALE = "Reading/HeaderFooterHeightScale";

    const char* READING_WIDGET_HEADER_LEFT  = "Reading.Widget/HeaderLeft";
    const char* READING_WIDGET_HEADER_RIGHT = "Reading.Widget/HeaderRight";
    const char* READING_WIDGET_FOOTER_LEFT  = "Reading.Widget/FooterLeft";
    const char* READING_WIDGET_FOOTER_RIGHT = "Reading.Widget/FooterRight";

    const char* READING_WIDGET_BATTERY_STYLE           = "Reading.Widget/BatteryStyle";
    const char* READING_WIDGET_BATTERY_STYLE_CHARGING  = "Reading.Widget/BatteryStyleCharging";
    const char* READING_WIDGET_BATTERY_SHOW_WHEN_BELOW = "Reading.Widget/BatteryShowWhenBelow";

    const char* READING_WIDGET_CLOCK_24H_FORMAT = "Reading.Widget/Clock24hFormat";

    public:
        QString getReadingBookmarkImage(bool isDarkMode);
        const TweaksReadingSettings& getReadingSettings() const;

        TweaksSettings();
        void sync();
        void load();

    private:
        QSettings qSettings;
        TweaksReadingSettings readingSettings;

        void loadReadingSettings();
        int getIntValue(const QString& key, int defaultValue);
};
