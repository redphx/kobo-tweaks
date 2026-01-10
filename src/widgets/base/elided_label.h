#pragma once
#include "../../common.h"
#include "../../utils.h"

#include <QLabel>

class TwElidedLabel : public QLabel {
    Q_OBJECT

public:
    TwElidedLabel(QWidget* parent = nullptr) : QLabel(parent) {
        setObjectName(QStringLiteral("twksLabel"));
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setMinimumWidth(1);
        // setStyleSheet("border: 1px solid black;");
    }

    void setFullText(const QString& newText) {
        QString simplified = newText.simplified();
        if (!fullText.isNull() && fullText == simplified) {
            return;
        }

        lastWidth = -1;
        fullText = simplified;
        QLabel::setText(fullText);

        setVisible(!fullText.isEmpty());
        syncSeparatorVisibility();

        // Tell the layout the ideal size (sizeHint) has changed
        updateGeometry(); 
        // Update the actual elision for the current width
        update();
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
        if (width > 0 && width != lastWidth) {
            lastWidth = width;
            updateElidedText();
        }
    }

    void updateElidedText() {
        if (contentsRect().width() <= 0) {
            return;
        }

        QString elidedText = fontMetrics().elidedText(fullText, Qt::ElideRight, contentsRect().width());
        if (elidedText != QLabel::text()) {
            bool blocked = blockSignals(true);
            QLabel::setText(elidedText);
            blockSignals(blocked);
        }
    }

    void syncSeparatorVisibility() {
        WidgetUtils::syncSeparatorVisibility(this);
    }

private:
    int lastWidth = -1;
};
