#pragma once
#include "../common.h"
#include "../adapters/reading_view.h"
#include "base/icon_label.h"

#include <QLabel>

struct TwBookPageConfig : TwIconLabelConfig {};

class TwBookPageWidget : public TwIconLabel {
    Q_OBJECT

public:
    TwBookPageWidget(ReadingView* rdv, ReadingViewAdapters adapters, TwBookPageConfig config, QWidget* parent = nullptr) : TwIconLabel(rdv, adapters, config, parent) {
        currentConfig.showIcon = false;
    }

    void onPageChanged() override {
        if (!ReadingView_fullBookCurrentPage || !ReadingView_fullBookTotalPages) {
            return;
        }

        int currentPage = ReadingView_fullBookCurrentPage(readingView);
        int totalPages = ReadingView_fullBookTotalPages(readingView);

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
