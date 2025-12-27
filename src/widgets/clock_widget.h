#pragma once

#include "../common.h"

#include <QLabel>
#include <QTime>
#include <QTimer>
#include <QEvent>
#include <QHBoxLayout>

struct TwClockWidgetConfig {
    bool is24hFormat = false;
};

class TwClockWidget : public QLabel {
    Q_OBJECT

    bool is24hFormat = true;
    QTimer* timer = nullptr;

public:
    TwClockWidget(TwClockWidgetConfig config, QWidget* parent = nullptr) : QLabel(parent), is24hFormat(config.is24hFormat)  {
        setObjectName(QStringLiteral("twks_label"));
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        // Setup timer
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TwClockWidget::updateTime);

        updateTime();
    }

    void updateTime() {
        // nh_log("updateTime");
        const QTime time = QTime::currentTime();

        const QString timeStr = time.toString(is24hFormat ? QStringLiteral("HH:mm") : QStringLiteral("h:mm AP"));
        setText(timeStr);

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
