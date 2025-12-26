#pragma once

#include "../common.h"

#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QEvent>
#include <QHBoxLayout>

struct TwClockWidgetConfig {
    bool isLeft = false;
    bool is24hFormat = false;
};

class TwClockWidget : public QWidget {
    Q_OBJECT

    bool isLeft = true;
    bool is24hFormat = true;
    QTimer* timer = nullptr;
    QLabel* label = nullptr;

public:
    TwClockWidget(TwClockWidgetConfig config, QWidget* parent = nullptr) : QWidget(parent), isLeft(config.isLeft), is24hFormat(config.is24hFormat)  {
        setObjectName(QStringLiteral("twks_clock"));

        label = new QLabel();
        label->setObjectName(QStringLiteral("twks_label"));
        label->setContentsMargins(0, 0, 0, 0);
        label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(6);
        setLayout(layout);

        // Align right -> stretch first
        if (!isLeft) {
            layout->addStretch(1);
        }

        layout->addWidget(label);

        // Align left -> stretch last
        if (isLeft) {
            layout->addStretch(1);
        }

        // Setup timer
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TwClockWidget::updateTime);

        updateTime();
    }

    void updateTime() {
        // nh_log("updateTime");
        const QTime time = QTime::currentTime();

        const QString timeStr = time.toString(is24hFormat ? QStringLiteral("HH:mm") : QStringLiteral("h:mm AP"));
        label->setText(timeStr);

        // schedule next update at next 2 full minutes
        const int msecToNextUpdate = (120 - QTime::currentTime().second()) * 1000 - QTime::currentTime().msec();
        timer->start(msecToNextUpdate);
    }

protected:
    bool event(QEvent* e) override {
        const QEvent::Type eventType = e->type();
        if (eventType == QEvent::Show) {
            // Update time when visible
            updateTime();
        } else if (eventType == QEvent::Hide) {
            // Stop timer when hidden
            timer->stop();
        }

        return QWidget::event(e);
    }
};
