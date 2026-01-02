#pragma once
#include "../../common.h"
#include "../../settings/settings.h"
#include "../../adapters/reading_view.h"
#include "../../widgets/base/widget_zone.h"

#include <QWidget>
#include <QHBoxLayout>

class TwWidgetZonesContainer : public QWidget {
    Q_OBJECT

public:
    TwWidgetZonesContainer(TweaksReadingSettings stt, const QString& qss, QWidget* parent = nullptr) : QWidget(parent), readingSettings(stt) {
        setContentsMargins(readingSettings.headerFooterMargins, 0, readingSettings.headerFooterMargins, 0);
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
        setStyleSheet(qss);

        lay = new QHBoxLayout(this);
        lay->setSpacing(readingSettings.widgetSpacing);
        lay->setContentsMargins(0, 0, 0, 0);
    }

    void setupZones(ReadingView* readingView, ReadingViewAdapters adapters, const Volume* volume, QVector<WidgetTypeEnum> leftWidgets, QVector<WidgetTypeEnum>, QVector<WidgetTypeEnum> rightWidgets) {
        if (addedWidgets) {
            return;
        }
        addedWidgets = true;

        bool isLeft = true;
        for (auto widgetTypes : {leftWidgets, rightWidgets}) {
            TwWidgetZone* zone = new TwWidgetZone(readingSettings);
            zone->setupWidgets(readingView, adapters, volume, readingSettings, isLeft, widgetTypes);

            // Insert widgets container into parent layout
            if (isLeft) {
                // Insert left
                lay->addWidget(zone, 0, Qt::AlignLeft);
                // lay->addStretch(1);
            } else {
                // Insert right
                lay->addWidget(zone, 0, Qt::AlignRight);
            }

            if (isLeft) {
                // containerLayout->addStretch(1);
                isLeft = false;
            }
        }
    }

private:
    TweaksReadingSettings readingSettings;
    QHBoxLayout* lay;
    bool addedWidgets = false;
};
