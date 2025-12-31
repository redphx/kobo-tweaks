#pragma once
#include "../../common.h"

#include <QHBoxLayout>

class TwWidgetZone : public QWidget {
    Q_OBJECT

public:
    TwWidgetZone(int spacing, QWidget* parent = nullptr) : QWidget(parent) {
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // container->setStyleSheet("border: 1px solid black;");

        // Setup Widgets container's layout
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(spacing);
    }

private:
    QHBoxLayout* layout;
};
