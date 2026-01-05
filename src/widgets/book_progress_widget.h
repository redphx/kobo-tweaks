#pragma once
#include "../common.h"
#include "../adapters/reading_view.h"
#include "base/icon_label.h"

#include <QLabel>
#include <QHBoxLayout>

struct TwBookProgressConfig : TwIconLabelConfig {};

class TwBookProgressWidget : public TwIconLabel {
    Q_OBJECT

public:
    TwBookProgressWidget(ReadingView* rdv, ReadingViewAdapters adapters, TwBookProgressConfig config, QWidget* parent = nullptr) : TwIconLabel(rdv, adapters, config, parent) {}

    void onPageChanged() override {
        if (!ReadingView_getCalculatedReadProgress) {
            return;
        }

        int percentage = ReadingView_getCalculatedReadProgress(readingView);
        textLabel->setText(QStringLiteral("%1%").arg(percentage));
    }

protected:
    QString iconSrc() const override {
        static const QString src = QStringLiteral(":/kobo_tweaks/images/book_progress.png");
        return src;
    }

    QString iconDarkSrc() const override {
        static const QString src = QStringLiteral(":/kobo_tweaks/images/book_progress_dark.png");
        return src;
    }
};
