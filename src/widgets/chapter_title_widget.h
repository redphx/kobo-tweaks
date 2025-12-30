#pragma once
#include "../common.h"
#include "base/elided_label.h"

#include <QtGlobal>
#include <QLabel>

class TwChapterTitleWidget : public TwElidedLabel {
    Q_OBJECT

public:
    TwChapterTitleWidget(QWidget* parent = nullptr) : TwElidedLabel(parent) {}

    void setTitle(const QString& title) {
        setFullText(title);
    }
};
