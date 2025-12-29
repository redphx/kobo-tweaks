#pragma once
#include "../common.h"

#include <QLabel>

class TwChapterPageWidget : public QLabel {
    Q_OBJECT

public:
    TwChapterPageWidget(QWidget* parent = nullptr) : QLabel(parent) {
        setObjectName(QStringLiteral("twks_label"));
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    }

    void setProgress(int currentPage, int totalPages) {
        if (currentPage < 1 || totalPages < 1 || currentPage > totalPages) {
            setText(QString());
            return;
        }

        setText(QStringLiteral("%1 // %2").arg(currentPage).arg(totalPages));
    }
};
