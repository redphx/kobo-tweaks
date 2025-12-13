#pragma once
#include <QWidget>
#include <QString>

#include "../common.h"
#include "../patches.h"
#include "../widgets/clock_widget.h"
#include "../widgets/battery_widget.h"


namespace ReadingViewHook {
    struct WidgetRefs {
        ReadingView* readingView = nullptr;
        QWidget* gestureContainer = nullptr;
        ReadingFooter* header = nullptr;
        ReadingFooter* footer = nullptr;

        TwClockWidget* clockWidget = nullptr;
        TwBatteryWidget* batteryWidget = nullptr;
    };

    void _insertWidgets(ReadingFooter footer, WidgetTypeEnum leftType, WidgetTypeEnum rightType);
    void _updateWidgets();

    ReadingView* constructor(ReadingView* self);
    void pageChanged(ReadingView* self, int page);
    void setFooterMargin(QWidget* self, int margin);

    namespace DogEarDelegate {
        QWidget* constructor(QWidget* self, QWidget* parent, const QString& image);
    }
}
