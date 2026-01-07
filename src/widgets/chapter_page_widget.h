#pragma once
#include "../common.h"
#include "../adapters/reading_view.h"
#include "base/icon_label.h"

#include <QLabel>

struct TwChapterPageConfig : TwIconLabelConfig {};

class TwChapterPageWidget : public TwIconLabel {
    Q_OBJECT

public:
    TwChapterPageWidget(ReadingView* rdv, ReadingViewAdapters adapters, TwChapterPageConfig config, QWidget* parent = nullptr) : TwIconLabel(rdv, adapters, config, parent) {
        currentConfig.showIcon = false;
    }

    void onPageChanged() override {
        if (!ReadingView_chapterCurrentPage || !ReadingView_chapterTotalPages) {
            return;
        }

        int currentPage = ReadingView_chapterCurrentPage(readingView);
        int totalPages = ReadingView_chapterTotalPages(readingView);

        if (currentPage < 1 || totalPages < 1 || currentPage > totalPages) {
            textLabel->setText(QString());
            return;
        }

        textLabel->setText(QStringLiteral("%1/%2").arg(currentPage).arg(totalPages));
    }

protected:
    QString iconSrc() const override { return {}; }
    QString iconDarkSrc() const override { return {}; }
};
