#pragma once
#include "../common.h"
#include "../adapters/reading_view.h"
#include "base/icon_label.h"

#include <QLabel>
#include <QHBoxLayout>

struct TwChapterTimeConfig : TwIconLabelConfig {};

class TwChapterTimeWidget : public TwIconLabel {
    Q_OBJECT

public:
    TwChapterTimeWidget(ReadingView* rdv, ReadingViewAdapters adapters, TwChapterTimeConfig config, QWidget* parent = nullptr) : TwIconLabel(rdv, adapters, config, parent) {}

    void onPageChanged() override {
        if (!ReadingView_hasValidReadingStats || !ReadingView_readingStats || !ReadingStats_currentChapterEstimate || !ReadingView_chapterCurrentPage || !ReadingView_chapterTotalPages) {
            return;
        }

        bool hasValidStats = ReadingView_hasValidReadingStats(readingView);
        int seconds = 0;
        if (hasValidStats) {
            ReadingStats* stats = alloca(128);  // only needs 8
            ReadingView_readingStats(stats, readingView);
            seconds = ReadingStats_currentChapterEstimate(stats);
            ReadingStats_deconstructor(stats);
        } else {
            // No stats -> estimate 1 minute per page
            int currentPage = ReadingView_chapterCurrentPage(readingView);
            int totalPages = ReadingView_chapterTotalPages(readingView);
            seconds = qMax(1, totalPages - currentPage) * 60;
        }

        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        if (seconds <= 60) {
            minutes = 1;
        }

        QString text;
        if (hours > 0) {
            if (minutes == 0) {
                text = QStringLiteral("%1h").arg(hours);
            } else {
                text = QStringLiteral("%1h %2m").arg(hours).arg(minutes);
            }
        } else {
            text = QStringLiteral("%1m").arg(minutes);
        }

        textLabel->setText(text);
    }

protected:
    QString iconSrc() const override {
        static const QString src = QStringLiteral(":/kobo_tweaks/images/chapter_time.png");
        return src;
    }

    QString iconDarkSrc() const override {
        static const QString src = QStringLiteral(":/kobo_tweaks/images/chapter_time_dark.png");
        return src;
    }
};
