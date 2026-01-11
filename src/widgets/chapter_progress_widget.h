#pragma once
#include "../common.h"
#include "../adapters/reading_view.h"
#include "base/icon_label.h"

#include <QLabel>
#include <QHBoxLayout>

struct TwChapterProgressConfig : TwIconLabelConfig {};

class TwChapterProgressWidget : public TwIconLabel {
    Q_OBJECT

public:
    TwChapterProgressWidget(ReadingView* rdv, ReadingViewAdapters adapters, TwChapterProgressConfig config, QWidget* parent = nullptr) : TwIconLabel(rdv, adapters, config, parent) {}

    void onPageChanged() override {
        if (!ReadingView_chapterCurrentPage || !ReadingView_chapterTotalPages) {
            return;
        }

        int currentPage = ReadingView_chapterCurrentPage(readingView);
        int totalPages = ReadingView_chapterTotalPages(readingView);
        bool shouldBeVisible = (currentPage >= 1 && totalPages >= 1 && currentPage <= totalPages);

        if (shouldBeVisible) {
            int percentage = qBound(0, (currentPage - 1) * 100 / totalPages, 100);
            textLabel->setText(QStringLiteral("%1%").arg(percentage));
        }

        if (shouldBeVisible != !isHidden()) {
            setVisible(shouldBeVisible);
            syncSeparatorVisibility();
        }
    }

protected:
    QString iconSrc() const override {
        static const QString src = QStringLiteral(":/kobo_tweaks/images/chapter_progress.png");
        return src;
    }

    QString iconDarkSrc() const override {
        static const QString src = QStringLiteral(":/kobo_tweaks/images/chapter_progress_dark.png");
        return src;
    }
};
