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
        setAttribute(Qt::WA_TransparentForMouseEvents);

        lay = new QHBoxLayout(this);
        lay->setSpacing(qMax(20, readingSettings.widgetSpacing));
        lay->setContentsMargins(0, 0, 0, 0);
    }

    void setupZones(ReadingView* readingView, ReadingViewAdapters adapters, const QString& contentTitle, int minimumSideWidth, QVector<WidgetTypeEnum> leftWidgets, QVector<WidgetTypeEnum> centerWidgets, QVector<WidgetTypeEnum> rightWidgets) {
        if (addedWidgets) {
            return;
        }
        addedWidgets = true;

        bool hasLeft = !leftWidgets.isEmpty();
        bool hasCenter = !centerWidgets.isEmpty();
        bool hasRight = !rightWidgets.isEmpty();

        // 1. Nothing
        if (!hasLeft && !hasCenter && !hasRight) {
            return;
        }

        TwWidgetZone* leftZone = new TwWidgetZone(readingSettings, readingView, adapters, contentTitle, leftWidgets);
        TwWidgetZone* centerZone = new TwWidgetZone(readingSettings, readingView, adapters, contentTitle, centerWidgets);
        TwWidgetZone* rightZone = new TwWidgetZone(readingSettings, readingView, adapters, contentTitle, rightWidgets, true);

        // 2. Only Left
        if (hasLeft && !hasCenter && !hasRight) {
            lay->addWidget(leftZone, 1, Qt::AlignLeft);
            return;
        }

        // 3. Only Center
        if (!hasLeft && hasCenter && !hasRight) {
            lay->addWidget(centerZone, 1, Qt::AlignCenter);
            return;
        }

        // 4. Only Right
        if (!hasLeft && !hasCenter && hasRight) {
            lay->addWidget(rightZone, 1, Qt::AlignRight);
            return;
        }

        // 5. Left + Right, no Center
        if (hasLeft && !hasCenter && hasRight) {
            lay->addWidget(leftZone, 0, Qt::AlignLeft);
            lay->addStretch(0);
            lay->addWidget(rightZone, 0, Qt::AlignRight);
            return;
        }

        // 6. Center and either Left/Right or both
        if (hasCenter) {
            lay->addWidget(leftZone, 0, Qt::AlignLeft);
            lay->addWidget(centerZone, 1, Qt::AlignCenter);
            lay->addWidget(rightZone, 0, Qt::AlignRight);

            // Set min width for left & right sides
            leftZone->setMinimumWidth(minimumSideWidth);
            leftZone->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
            rightZone->setMinimumWidth(minimumSideWidth);
            rightZone->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        }
    }

private:
    TweaksReadingSettings readingSettings;
    QHBoxLayout* lay;
    bool addedWidgets = false;
};
