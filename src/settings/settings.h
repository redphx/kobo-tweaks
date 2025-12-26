#pragma once

#include "../common.h"

#include <QSettings>
#include <QHash>
#include <QVector>


template <typename EnumType>
inline uint qHashEnum(EnumType e, uint seed = 0) {
    return ::qHash(static_cast<int>(e), seed);
}

template <typename EnumType>
inline uint qHash(const QVector<EnumType>& v, uint seed = 0) {
    uint h = seed;
    for (EnumType e : v) {
        h = qHashEnum(e, h);
    }
    return h;
}

template<typename Derived, typename EnumType>
class SettingEnumValue {
public:
    static EnumType fromSetting(const QSettings& settings, const char* key, EnumType defaultValue=EnumType::Invalid) {
        QString value = settings.value(key, "").toString();
        return fromString(value, defaultValue);
    }

    // QVector
    static QVector<EnumType> fromSetting(const QSettings& settings, const char* key, QVector<EnumType> defaultValue={}) {
        QStringList list = settings.value(key, QStringList()).toStringList();
        return fromString(list, defaultValue);
    }

    static EnumType fromString(const QString &str, EnumType defaultValue=EnumType::Invalid) {
        EnumType value = Derived::valueMap().value(str.toLower().trimmed(), EnumType::Invalid);
        if (value == EnumType::Invalid) {
            return defaultValue;
        }

        return value;
    }

    // QVector
    static QVector<EnumType> fromString(const QStringList &list, QVector<EnumType> defaultValue={}) {
        QVector<EnumType> values;

        for (const QString& str : list) {
            EnumType value = fromString(str, EnumType::Invalid);
            if (value == EnumType::Invalid) {  // Ignore Invalid value
                continue;
            }

            if (values.contains(value)) {  // Don't allow duplicated values
                continue;
            }

            values.append(value);
        }

        if (values.isEmpty()) {
            return defaultValue;
        }

        return values;
    }

    static QString toString(EnumType e) {
        const auto &map = Derived::reverseMap();
        return map.value(e, QString());
    }

    static QVariant toString(const QVector<EnumType>& list) {
        // Prevent an empty list from being saved as "@Invalid()"
        if (list.isEmpty()) {
            return "";
        }

        const auto &map = Derived::reverseMap();

        QStringList out;
        out.reserve(list.size());
        for (auto e : list) {
            const auto it = map.find(e);
            if (it != map.end() && !it.value().isEmpty()) {
                out.append(it.value());
            }
        }
        return out;
    }

    static const QHash<QString, EnumType>& valueMap();
    static const QHash<EnumType, QString>& reverseMap();
    static EnumType defaultValue();
};


// WidgetTypeSetting
enum class WidgetTypeEnum { Invalid, Battery, Clock, Separator };
inline uint qHash(WidgetTypeEnum e, uint seed = 0) { return qHashEnum(e, seed); }
class WidgetTypeSetting : public SettingEnumValue<WidgetTypeSetting, WidgetTypeEnum> {
public:
    using EnumType = WidgetTypeEnum;

    static const QHash<QString, EnumType>& valueMap() {
        static const QHash<QString, EnumType> map = {
            {QStringLiteral("battery"), EnumType::Battery},
            {QStringLiteral("clock"),   EnumType::Clock},
            {QStringLiteral("-"),       EnumType::Separator},
        };
        return map;
    }

    static const QHash<EnumType, QString>& reverseMap() {
        static const QHash<EnumType, QString> map = {
            {EnumType::Battery,   QStringLiteral("Battery")},
            {EnumType::Clock,     QStringLiteral("Clock")},
            {EnumType::Separator, QStringLiteral("-")},
            {EnumType::Invalid,   ""},
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
    int headerFooterMargins = 50;

    QVector<WidgetTypeEnum> widgetHeaderLeft  = {};
    QVector<WidgetTypeEnum> widgetHeaderRight = {};
    QVector<WidgetTypeEnum> widgetFooterLeft  = {};
    QVector<WidgetTypeEnum> widgetFooterRight = {};

    int widgetSpacing = 10;
    QString widgetSeparator = "";

    BatteryStyleEnum widgetBatteryStyle = BatteryStyleEnum::IconLevel;
    BatteryStyleEnum widgetBatteryStyleCharging = BatteryStyleEnum::IconLevel;
    int widgetBatteryShowWhenBelow = 100;

    bool widgetClock24hFormat = true;
};

class TweaksSettings {
    const char* READING_BOOKMARK_IMAGE = "Reading/BookmarkImage";
    const char* READING_HEADER_FOOTER_HEIGHT_SCALE = "Reading/HeaderFooterHeightScale";
    const char* READING_HEADER_FOOTER_MARGINS = "Reading/HeaderFooterMargins";

    const char* READING_WIDGET_HEADER_LEFT  = "Reading.Widget/HeaderLeft";
    const char* READING_WIDGET_HEADER_RIGHT = "Reading.Widget/HeaderRight";
    const char* READING_WIDGET_FOOTER_LEFT  = "Reading.Widget/FooterLeft";
    const char* READING_WIDGET_FOOTER_RIGHT = "Reading.Widget/FooterRight";

    const char* READING_WIDGET_SEPARATOR = "Reading.Widget/Separator";
    const char* READING_WIDGET_SPACING   = "Reading.Widget/Spacing";

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
        QString getStringValue(const QString& key, const QString& defaultValue);
};
