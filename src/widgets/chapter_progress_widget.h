#pragma once
#include "../common.h"

#include <QLabel>
#include <QHBoxLayout>

struct TwChapterProgressWidgetConfig {
    bool isDarkMode = false;
};

class TwChapterProgressWidget : public QWidget {
    Q_OBJECT

public:
    TwChapterProgressWidget(TwChapterProgressWidgetConfig config, QWidget* parent = nullptr) : QWidget(parent), currentDarkMode(config.isDarkMode) {
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

    void setProgress(int currentPage, int totalPages) {
        if (currentPage < 1 || totalPages < 1 || currentPage > totalPages) {
            progressLabel->setText(QString());
            return;
        }

        currentPercentage = qBound(1, currentPage * 100 / totalPages, 100);
        updateDisplay();
    }

private:
    QLabel* iconLabel = nullptr;
    QLabel* progressLabel = nullptr;

    bool currentDarkMode = false;
    int currentPercentage = 1;

    bool lastDarkMode = false;

    void loadIcon() {
        QPixmap icon(currentDarkMode ? QStringLiteral(":/kobo_tweaks/images/chapter_progress_dark.png") : QStringLiteral(":/kobo_tweaks/images/chapter_progress.png"));
        iconLabel->setPixmap(icon);
        iconLabel->setFixedSize(icon.size());
    }

    void updateDisplay() {
        if (currentDarkMode != lastDarkMode) {
            lastDarkMode = currentDarkMode;
            loadIcon();
        }

        progressLabel->setText(QStringLiteral("%1%").arg(currentPercentage));
    }
};
