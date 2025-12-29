#pragma once
#include "../common.h"
#include "elided_label.h"

#include <QtGlobal>
#include <QLabel>

class TwBookTitleWidget : public TwElidedLabel {
    Q_OBJECT

public:
    TwBookTitleWidget(QWidget* parent = nullptr) : TwElidedLabel(parent) {}

    void setTitle(const Content& content) {
        if (!Content_getTitle) {
            return;
        }

        Content_getTitle(&fullText, &content);
        updateElidedText();
    }
};
