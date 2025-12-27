#pragma once
#include "../common.h"

#include <QtGlobal>
#include <QLabel>

class TwBookTitleWidget : public QLabel {
    Q_OBJECT

public:
    TwBookTitleWidget(QWidget* parent = nullptr) : QLabel(parent) {
        setObjectName(QStringLiteral("twks_label"));
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    void setTitle(const Volume& volume) {
        if (!Content_getTitle) {
            return;
        }

        QString title;
        Content_getTitle(&title, &volume);

        QString elidedText = fontMetrics().elidedText(title, Qt::ElideRight, width());
        setText(elidedText);
    }
};
