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

        QString title;
        Content_getTitle(&title, &content);

        // TODO: remove this line when we're able to get the non-uppercase chapter title
        title = title.toUpper();
        setFullText(title);
    }
};
