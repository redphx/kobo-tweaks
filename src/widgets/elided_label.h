#pragma once
#include "../common.h"

#include <QLabel>

class TwElidedLabel : public QLabel {
    Q_OBJECT

public:
    TwElidedLabel(QWidget* parent = nullptr) : QLabel(parent) {
        setObjectName(QStringLiteral("twks_label"));
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        setMinimumWidth(1);
        // setStyleSheet("border: 1px solid black;");
    }

    QSize sizeHint() const override {
        QFontMetrics fm = fontMetrics();

        int textWidth = fm.width(fullText); 
        // Add margins/padding to the text width
        int totalWidth = textWidth + (width() - contentsRect().width());
        return QSize(totalWidth, QLabel::sizeHint().height());
    }

protected:
    QString fullText;

    void resizeEvent(QResizeEvent *event) override {
        QLabel::resizeEvent(event);
        int width = contentsRect().width();
        if (width != lastWidth) {
            lastWidth = width;
            updateElidedText();
        }
    }

    void updateElidedText() {
        QString elidedText = fontMetrics().elidedText(fullText, Qt::ElideRight, contentsRect().width());
        if (elidedText != QLabel::text()) {
            QLabel::setText(elidedText);
        }
    }

private:
    int lastWidth = -1;
};
