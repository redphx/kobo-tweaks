#pragma once
#include "../../common.h"
#include "../../settings/settings.h"
#include "../../adapters/reading_view.h"

#include <QWidget>
#include <QHBoxLayout>

class TwWidgetZonesContainer : public QWidget {
    Q_OBJECT

public:
    TwWidgetZonesContainer(TweaksReadingSettings stt, const QString& qss, QWidget* parent = nullptr) : QWidget(parent), readingSettings(stt) {
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
        setStyleSheet(qss);

        lay = new QHBoxLayout(this);
        lay->setSpacing(readingSettings.widgetSpacing);
        lay->setContentsMargins(0, 0, 0, 0);
    }

    void setupZones(QVector<WidgetTypeEnum> leftWidgets, QVector<WidgetTypeEnum>, QVector<WidgetTypeEnum> rightWidgets) {
        if (addedWidgets) {
            return;
        }
        addedWidgets = true;

        bool isLeft = true;
        for (auto widgetTypes : {leftWidgets, rightWidgets}) {
            // Setup Widgets container
            QWidget* container = new QWidget;
            // Set container's width to the original margin value
            // container->setMinimumWidth(originalContentsMargins);
            container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            // container->setStyleSheet("border: 1px solid black;");

            // Setup Widgets container's layout
            QHBoxLayout* containerLayout = new QHBoxLayout(container);
            containerLayout->setContentsMargins(0, 0, 0, 0);
            containerLayout->setSpacing(readingSettings.widgetSpacing);

            if (!isLeft) {
                containerLayout->addStretch(1);
            }

            // Insert widgets container into parent layout
            if (isLeft) {
                // Insert left
                container->setContentsMargins(readingSettings.headerFooterMargins, 0, 0, 0);
                lay->addWidget(container, 0, Qt::AlignLeft);
                // lay->addStretch(1);
            } else {
                // Insert right
                container->setContentsMargins(0, 0, readingSettings.headerFooterMargins, 0);
                lay->addWidget(container, 0, Qt::AlignRight);
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
