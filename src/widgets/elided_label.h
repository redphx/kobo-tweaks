#pragma once
#include "../common.h"

#include <QLabel>

class TwElidedLabel : public QLabel {
    Q_OBJECT

public:
    TwElidedLabel(QWidget* parent = nullptr) : QLabel(parent) {
        setObjectName(QStringLiteral("twks_label"));
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    }

protected:
    QString fullText;

    void resizeEvent(QResizeEvent *event) override {
        QLabel::resizeEvent(event);
        updateElidedText();
    }

    void updateElidedText() {
        QString elidedText = fontMetrics().elidedText(fullText, Qt::ElideRight, contentsRect().width());
        if (elidedText != QLabel::text()) {
            QLabel::setText(elidedText);
        }
    }
};
