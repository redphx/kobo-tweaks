#pragma once
#include "../../common.h"

#include <QWidget>
#include <QHBoxLayout>

class TwWidgetZonesContainer : public QWidget {
    Q_OBJECT

public:
    TwWidgetZonesContainer(const QString& qss, int spacing, QWidget* parent = nullptr) : QWidget(parent) {
        setContentsMargins(0, 0, 0, 0);
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
        setStyleSheet(qss);

        lay = new QHBoxLayout(this);
        lay->setSpacing(spacing);
        lay->setContentsMargins(0, 0, 0, 0);
    }

private:
    QHBoxLayout* lay;
};
