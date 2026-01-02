#pragma once
#include "../common.h"
#include "base/elided_label.h"

#include <QtGlobal>
#include <QLabel>

class TwBookTitleWidget : public TwElidedLabel {
    Q_OBJECT

public:
    TwBookTitleWidget(const QString& contentTitle, QWidget* parent = nullptr) : TwElidedLabel(parent) {
        // TODO: remove toUpper() when we're able to get the non-uppercase chapter title
        setFullText(contentTitle.toUpper());
    }
};
