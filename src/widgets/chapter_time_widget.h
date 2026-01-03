#pragma once
#include "../common.h"

#include <QLabel>
#include <QHBoxLayout>

struct TwChapterTimeWidgetConfig {
    bool isDarkMode = false;
};

class TwChapterTimeWidget : public QWidget {
    Q_OBJECT

public:
    TwChapterTimeWidget(TwChapterTimeWidgetConfig config, QWidget* parent = nullptr) : QWidget(parent), currentDarkMode(config.isDarkMode) {
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

        iconLabel = new QLabel();
        iconLabel->setContentsMargins(0, 0, 0, 0);
        iconLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        loadIcon();

        progressLabel = new QLabel();
        progressLabel->setObjectName(QStringLiteral("twks_label"));
        progressLabel->setContentsMargins(0, 0, 0, 0);
        progressLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(6);
        layout->addWidget(iconLabel, 0);
        layout->addWidget(progressLabel, 0);
        setLayout(layout);

        show();
    }

    void setDarkMode(bool dark) {
        currentDarkMode = dark;
        updateDisplay();
    }

    void updateEstimation(int seconds) {
        currentSeconds = seconds;
        updateDisplay();
    }

private:
    QLabel* iconLabel = nullptr;
    QLabel* progressLabel = nullptr;

    bool currentDarkMode = false;
    int currentSeconds = 0;

    bool lastDarkMode = false;

    void loadIcon() {
        QPixmap icon(currentDarkMode ? QStringLiteral(":/kobo_tweaks/images/chapter_time_dark.png") : QStringLiteral(":/kobo_tweaks/images/chapter_time.png"));
        iconLabel->setPixmap(icon);
        iconLabel->setFixedSize(icon.size());
    }

    void updateDisplay() {
        if (currentDarkMode != lastDarkMode) {
            lastDarkMode = currentDarkMode;
            loadIcon();
        }

        int hours = currentSeconds / 3600;
        int minutes = (currentSeconds % 3600) / 60;
        if (currentSeconds <= 60) {
            minutes = 1;
        }

        QString text;
        if (hours > 0) {
            if (minutes == 0) {
                text = QStringLiteral("%1h").arg(hours);
            } else {
                text = QStringLiteral("%1h%m").arg(hours).arg(minutes);
            }
        } else {
            text = QStringLiteral("%1m").arg(minutes);
        }

        progressLabel->setText(text);
    }
};
